#include "VolumeController.h"
#include "BoxSlicer.h"
#include "gl/util/Draw.h"
#include "main/MainConfig.h"
#include "main/MainController.h"
#include "gl/geom/Plane.h"

using namespace gl;
using namespace std;

VolumeController::VolumeController()
{
	draw_bounds = true;
	draw_planes = false;
    draw_lines = true;
	draw_cursor3D = false;

	dirty = true;
	opacityScale = 1.0f;
	renderMode = VR;
	shading = true;
	drawnHighRes = false;
	cursorActive = false;
	cursorRadius = 0.1;
    isovalue = 0.5f;
	useJitter = true;

	volumeTexture.generate(GL_TEXTURE_3D);
	gradientTexture.generate(GL_TEXTURE_3D);
	maskTexture.generate(GL_TEXTURE_3D);

	proxyVertices.generateVBO(GL_DYNAMIC_DRAW);
	proxyIndices.generateIBO(GL_DYNAMIC_DRAW);

	//camera.setView(lookAt(1, 1, 1, 0, 0, 0, 0, 1, 0));
	//camera.setView(lookAt(0, 0, 1.5f, 0, 0, 0, 0, 1, 0));
	boxShader = Program::create("shaders/volume_clut.vert", "shaders/volume_clut.frag");
	boxShader.enable();
	glUniform1i(boxShader.getUniform("tex_volume"), 0);
	glUniform1i(boxShader.getUniform("tex_gradients"), 1);
	glUniform1i(boxShader.getUniform("tex_clut"), 2);
	glUniform1i(boxShader.getUniform("tex_jitter"), 3);
	glUniform1i(boxShader.getUniform("tex_mask"), 4);
	glUniform1i(boxShader.getUniform("tex_context"), 5);

	fullResRT.setInternalColorFormat(GL_RGB16F);
	fullResRT.generate(viewport_.width, viewport_.height, true);
	lowResRT.setInternalColorFormat(GL_RGB16F);
	lowResRT.generate(viewport_.width / 2, viewport_.height / 2, true);
	fullScreenQuad.generate();

	cursor3DShader = Program::create("shaders/menu.vert", "shaders/menu.frag");
	cursor3DVBO.generateVBO(GL_STATIC_DRAW);
	cursor3DVBO.bind();

	MainConfig cfg;
	minSlices = cfg.getValue<unsigned>(MainConfig::MIN_SLICES);
	maxSlices = cfg.getValue<unsigned>(MainConfig::MAX_SLICES);


	// stochastic jittering texture
	{
		unsigned size = 32;
		vector<unsigned char> buf;
		srand((unsigned)time(NULL));
		for (unsigned i = 0; i < size*size; ++i)
			buf.push_back(static_cast<unsigned char>(rand() * 255.0 / RAND_MAX));

		jitterTexture.generate(GL_TEXTURE_2D);
		jitterTexture.bind();
		jitterTexture.setParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		jitterTexture.setParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		jitterTexture.setParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
		jitterTexture.setParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);
		jitterTexture.setData2D(GL_RED, size, size, GL_RED, GL_UNSIGNED_BYTE, &buf[0]);
	}
}

void VolumeController::gainFocus()
{
	MainController::getInstance().showTransfer1D(false);
	MainController::getInstance().setMode(MainController::MODE_3D);
	auto& lsc = MainController::getInstance().leapStateController();
	lsc.clear();
	lsc.add(LeapStateController::icon_point_circle, "Main Menu");
	lsc.add(LeapStateController::icon_three_circle, "Options");
    lsc.add(LeapStateController::icon_fist, "Rotate/Zoom");
	lsc.add(LeapStateController::icon_l_open, "Center");
}

Camera& VolumeController::getCamera()
{
	return camera;
}

void VolumeController::setVolume(VolumeData* volume)
{
	this->volume = volume;

	GLenum internalFormat;
	switch (volume->getType()) {
	case GL_UNSIGNED_BYTE: internalFormat = GL_R8; break;
	case GL_UNSIGNED_SHORT: internalFormat = GL_R16; break;
	case GL_BYTE: internalFormat = GL_R8_SNORM; break;
	case GL_SHORT: internalFormat = GL_R16_SNORM; break;
	default: internalFormat = GL_RED; break;
	}

	volumeTexture.bind();
	volumeTexture.setParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	volumeTexture.setParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	volumeTexture.setParameter(GL_TEXTURE_WRAP_S, GL_CLAMP);
	volumeTexture.setParameter(GL_TEXTURE_WRAP_R, GL_CLAMP);
	volumeTexture.setParameter(GL_TEXTURE_WRAP_T, GL_CLAMP);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	volumeTexture.setData3D(
		internalFormat,
		volume->getWidth(),
		volume->getHeight(),
		volume->getDepth(),
		volume->getFormat(),
		volume->getType(),
		volume->getData());

	// gradient texture will be 8-bits per channel (RGB format)
	{
		Vec3 minG = volume->getMinGradient();
		Vec3 maxG = volume->getMaxGradient();
		Vec3 dG = maxG - minG;
		const std::vector<Vec3>& gradients = volume->getGradients();

		unsigned char* data = new unsigned char[gradients.size() * 3];
		unsigned char* p = data;

		for (const Vec3& g : gradients) {
			*p++ = static_cast<unsigned char>(((g.x - minG.x) / dG.x) * 255);
			*p++ = static_cast<unsigned char>(((g.y - minG.y) / dG.y) * 255);
			*p++ = static_cast<unsigned char>(((g.z - minG.z) / dG.z) * 255);
		}

		gradientTexture.bind();
		gradientTexture.setParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		gradientTexture.setParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		gradientTexture.setParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		gradientTexture.setParameter(GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		gradientTexture.setParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		gradientTexture.setData3D(
			GL_RGB,
			volume->getWidth(),
			volume->getHeight(),
			volume->getDepth(),
			GL_RGB,
			GL_UNSIGNED_BYTE,
			data);

		delete[] data;
	}

	// mask texture
	{
		vector<GLubyte> dat;
		dat.resize(volume->getNumVoxels(), 0);
		maskTexture.bind();
		maskTexture.setParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		maskTexture.setParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		maskTexture.setParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		maskTexture.setParameter(GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		maskTexture.setParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		maskTexture.setData3D(
			GL_R8,
			volume->getWidth(),
			volume->getHeight(),
			volume->getDepth(),
			GL_RED,
			GL_UNSIGNED_BYTE,
			&dat[0]);	
	}
}

bool VolumeController::keyboardInput(GLFWwindow* window, int key, int action, int mods)
{
	switch (key)
	{
	case GLFW_KEY_1:
		if (action == GLFW_PRESS)
			setOpacityScale(getOpacityScale() - 0.1f);
		break;
	case GLFW_KEY_2:
		if (action == GLFW_PRESS)
			setOpacityScale(getOpacityScale() + 0.1f);
		break;
	case GLFW_KEY_V:
		if (action == GLFW_PRESS)
			cycleMode();
		break;
	case GLFW_KEY_L:
		if (action == GLFW_PRESS)
			toggleShading();
		break;
	case GLFW_KEY_J:
		if (action == GLFW_PRESS) {
			useJitter = !useJitter;
			markDirty();
		}
		break;
        case GLFW_KEY_I:
            if (action == GLFW_PRESS) {
                draw_lines = !draw_lines;
                markDirty();
            }
            break;
	}

    return true;
}

bool VolumeController::mouseButton(GLFWwindow* window, int button, int action, int mods, double x, double y)
{
	leap_cam_control_.mouseButton(button, action, mods, x, y);
    
    return true;
}

bool VolumeController::mouseMotion(GLFWwindow* window, double x, double y)
{
    if (!viewport_.contains(x, y))
        return true;

	leap_cam_control_.mouseMotion(x, y);


    return true;
}

bool VolumeController::scroll(GLFWwindow* window, double dx, double dy)
{
	leap_cam_control_.mouseScroll(dx, dy);
    return true;
}

bool VolumeController::leapInput(const Leap::Controller& leapController, const Leap::Frame& currentFrame)
{
	leap_cam_control_.update(leapController, currentFrame);

	return false;
}

void VolumeController::draw()
{
	static int cleanFrames = 0;
	static gl::Texture currentTexture;

	// draw to texture
	if (dirty) {
		lowResRT.bind();
		lowResRT.clear();
		draw(4.0, true, lowResRT.getColorTarget().width(), lowResRT.getColorTarget().height());
		lowResRT.unbind();
		dirty = false;
		drawnHighRes = false;
		cleanFrames = 1;
		currentTexture = lowResRT.getColorTarget();
	} else if (!drawnHighRes && cleanFrames++ > 30) {
		fullResRT.bind();
		fullResRT.clear();
		draw(1.0, false, fullResRT.getColorTarget().width(), fullResRT.getColorTarget().height());
		fullResRT.unbind();
		drawnHighRes = true;
		currentTexture = fullResRT.getColorTarget();
	}

	// draw from texture to screen
	viewport_.apply();
	fullScreenQuad.draw(currentTexture);
}

void VolumeController::resize()
{
	fullResRT.resize(viewport_.width, viewport_.height);
	lowResRT.resize(viewport_.width / 2, viewport_.height / 2);
	camera.aspect(viewport_.aspect());
	markDirty();
}

void VolumeController::draw(double samplingScale, bool limitSamples, int w, int h)
{
	if (!volume) {
		return;
	}

	Mat4 modelView = camera.view();
	Mat4 mvp = camera.projection() * modelView;
	static Draw d;
	glEnable(GL_DEPTH_TEST);

    if (draw_lines) {
	if (draw_bounds) {
		const Box& bb = volume->getBounds();
		d.setModelViewProj(mvp);
		d.begin(GL_LINES);
		d.color(0.5f, 0.5f, 0.5f);
		d.geometry(volume->getBounds().lines());
		d.end();
		d.draw();
	}

	// clipping plane lines: this needs to be improved
	if (draw_planes && clip_planes_.size() > 0) {
		d.color(1, 0, 1);
		for (Plane& p : clip_planes_) {
			d.begin(GL_LINE_LOOP);
			vector<Vec3> verts = volume->getBounds().intersect(p);
			for (Vec3& v : verts) {
				d.vertex(v.x, v.y, v.z);
			}
			d.end();
			d.draw();
		}
	}

	// mask cursor
	if (draw_cursor3D) {
		d.begin(GL_LINES);
		d.color(maskColor.x, maskColor.y, maskColor.z);
		d.geometry(maskGeometry);

		Vec3 min = volume->getBounds().min();
		Vec3 max = volume->getBounds().max();

		d.vertex(maskCenter.x, maskCenter.y, volume->getBounds().min().z);
		d.vertex(maskCenter.x, maskCenter.y, volume->getBounds().max().z);
		d.vertex(maskCenter.x, volume->getBounds().min().y, maskCenter.z);
		d.vertex(maskCenter.x, volume->getBounds().max().y, maskCenter.z);
		d.vertex(volume->getBounds().min().x, maskCenter.y, maskCenter.z);
		d.vertex(volume->getBounds().max().x, maskCenter.y, maskCenter.z);

		//gl::Plane plane((Vec3(camera.getEye()) - maskCenter).normalize(), maskCenter);
		//std::vector<gl::Vec3> verts = volume->getBounds().intersect(plane);

		//for (int i = 0; i < verts.size(); i++) {
		//	Vec3 v = verts[i];
		//	Vec3 u = verts[(i+1)%verts.size()];
		//	d.vertex(v.x, v.y, v.z);
		//	d.vertex(u.x, u.y, u.z);
		//}

		//d.vertex(min.x, min.y, maskCenter.z);
		//d.vertex(min.x, max.y, maskCenter.z);
		//d.vertex(min.x, max.y, maskCenter.z);
		//d.vertex(max.x, max.y, maskCenter.z);
		//d.vertex(max.x, max.y, maskCenter.z);
		//d.vertex(max.x, min.y, maskCenter.z);
		//d.vertex(max.x, min.y, maskCenter.z);
		//d.vertex(min.x, min.y, maskCenter.z);

		//d.vertex(min.x, maskCenter.y, min.z );
		//d.vertex(min.x, maskCenter.y, max.z );
		//d.vertex(min.x, maskCenter.y, max.z );
		//d.vertex(max.x, maskCenter.y, max.z );
		//d.vertex(max.x, maskCenter.y, max.z );
		//d.vertex(max.x, maskCenter.y, min.z );
		//d.vertex(max.x, maskCenter.y, min.z );
		//d.vertex(min.x, maskCenter.y, min.z );


		//d.vertex(maskCenter.x, min.y, min.z);
		//d.vertex(maskCenter.x, min.y, max.z);
		//d.vertex(maskCenter.x, min.y, max.z);
		//d.vertex(maskCenter.x, max.y, max.z);
		//d.vertex(maskCenter.x, max.y, max.z);
		//d.vertex(maskCenter.x, max.y, min.z);
		//d.vertex(maskCenter.x, max.y, min.z);
		//d.vertex(maskCenter.x, min.y, min.z);

		// around the x

		d.end();
		d.draw();
	}
    }

	//{
	//	glEnable(GL_CULL_FACE);
	//	glCullFace(GL_FRONT);
	//	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//	cursor3DShader.enable();
	//	cursor3DVBO.bind();
	//	glUniform4f(cursor3DShader.getUniform("color"), 0.0f, cursorActive ? 1.0f : 0.0f, 1.0f, 1.0f);
	//	glUniformMatrix4fv(cursor3DShader.getUniform("modelViewProjection"), 1, false, camera.getProjection() * camera.getView() * translation(cursor3D) * scale(cursorRadius, cursorRadius, cursorRadius));
	//	int loc = cursor3DShader.getAttribute("vs_position");
	//	glEnableVertexAttribArray(loc);
	//	glVertexAttribPointer(loc, 3, GL_FLOAT, false, 0, 0);
	//	glDrawArrays(GL_TRIANGLES, 0, cursorGeom.getIndices().size());
	//	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//	glDisable(GL_CULL_FACE);
	//}

	// proxy geometry
	glActiveTexture(GL_TEXTURE5);
	tex_context_.bind();
	glActiveTexture(GL_TEXTURE4);
	maskTexture.bind();
	glActiveTexture(GL_TEXTURE3);
	jitterTexture.bind();
	glActiveTexture(GL_TEXTURE2);
	clutTexture.bind();
	glActiveTexture(GL_TEXTURE1);
	gradientTexture.bind();
	glActiveTexture(GL_TEXTURE0);
	volumeTexture.bind();




	boxShader.enable();

	updateSlices(samplingScale, limitSamples);


	glUniformMatrix4fv(boxShader.getUniform("modelViewProjection"), 1, false, mvp);
	glUniformMatrix4fv(boxShader.getUniform("modelView"), 1, false, modelView);

	glUniform3fv(boxShader.getUniform("volumeMin"), 1, volume->getBounds().min());
	glUniform3fv(boxShader.getUniform("volumeDimensions"), 1, (volume->getBounds().max() - volume->getBounds().min()));
	glUniform1i(boxShader.getUniform("signed_normalized"), volume->isSigned());
	glUniform1i(boxShader.getUniform("use_shading"), (renderMode != MIP && shading));


	boxShader.uniform("visible_min", volume->visible().left());
	boxShader.uniform("visible_scale", 1.0f / volume->visible().width());

	glUniform1i(boxShader.getUniform("render_mode"), renderMode);

	glUniform1i(boxShader.getUniform("use_jitter"), useJitter);

	boxShader.uniform("num_clip_planes", static_cast<GLint>(clip_planes_.size()));
	std::vector<Vec4> planes;
	for (Plane& p : clip_planes_) {
		planes.push_back(Vec4(p.normal(), p.distFromOrigin()));
	}
	boxShader.uniform("clip_planes", planes);

	glUniform3f(boxShader.getUniform("lightDirection"), -camera.forward().x, -camera.forward().y, -camera.forward().z);

	glUniform3f(boxShader.getUniform("camera_pos"), camera.eye().x, camera.eye().y, camera.eye().z);

	glUniform3f(boxShader.getUniform("minGradient"), volume->getMinGradient().x, volume->getMinGradient().y, volume->getMinGradient().z);
	glUniform1f(boxShader.getUniform("opacity_scale"), opacityScale);
	Vec3 r = volume->getMaxGradient() - volume->getMinGradient();
	glUniform3f(boxShader.getUniform("rangeGradient"), r.x, r.y, r.z);


	boxShader.uniform("cursor_position", maskCenter);






	Vec4 cpss = mvp * Vec4(maskCenter, 1.0f);
	cpss /= cpss.w;
	cpss.x = (cpss.x + 1.0) * (w / 2.0);
	cpss.y = (cpss.y + 1.0) * (h / 2.0);
	glUniform3f(boxShader.getUniform("cursor_position_ss"), cpss.x, cpss.y, cpss.z);




	Vec4 cpee = camera.view() * Vec4(maskCenter, 1.0f);
	glUniform3f(boxShader.getUniform("cursor_position_es"), cpee.x, cpee.y, cpee.z);


	glUniform1f(boxShader.getUniform("cursor_radius_ws"), cursorRadius);
	float cursorRadiusSS = gl::projectedRadius(0.8726388, (maskCenter - camera.eye()).length(), cursorRadius) * h / 2.0f;
	glUniform1f(boxShader.getUniform("cursor_radius_ss"), cursorRadiusSS);

	glUniform2f(boxShader.getUniform("window_size"), w, h);


	glUniform1i(boxShader.getUniform("cursor_on"), use_context);

	int loc = boxShader.getAttribute("vs_position");
	glEnableVertexAttribArray(loc);
	glVertexAttribPointer(loc, 3, GL_FLOAT, false, 0, 0);



	switch (renderMode)
	{
	case MIP:
		glEnable(GL_BLEND);
		glBlendEquation(GL_MAX);
		glBlendFunc(GL_ONE, GL_ONE);
		break;
	case VR:
		glEnable(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
		break;
	case ISOSURFACE:
		glDisable(GL_BLEND);
		glUniform1f(boxShader.getUniform("isoValue"), isovalue);
		break;
	default:
		break;
	}

	glEnable(GL_PRIMITIVE_RESTART);
	glPrimitiveRestartIndex(65535);
	glDrawElements(GL_TRIANGLE_FAN, numSliceIndices, GL_UNSIGNED_SHORT, 0);
	glDisable(GL_PRIMITIVE_RESTART);


	//if (clip_planes_.size() > 0) {
	//	d.color(1, 0, 1);
	//	for (Plane& p : clip_planes_) {
	//		d.begin(GL_TRIANGLE_FAN);
	//		vector<Vec3> verts = volume->getBounds().intersect(p);
	//		for (Vec3& v : verts) {
	//			d.vertex(v.x, v.y, v.z);
	//		}
	//		d.end();
	//		d.draw();
	//	}
	//}

	glDisable(GL_DEPTH_TEST);

	glDisable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBlendEquation(GL_FUNC_ADD);

	//static Draw d;
	//d.setModelViewProj(gl::ortho2D(0, viewport_.width, 0, viewport_.height));
	//d.begin(GL_LINES);
	//d.color(1, 0, 0);
	//d.circle(cpss.x, cpss.y, cursorRadiusSS, 32);
	//d.end();
	//d.draw();



}

void VolumeController::updateSlices(double samplingScale, bool limitSamples)
{
	float refSampleLength = volume->getBounds().size().length() / Vec3(volume->getWidth(), volume->getHeight(), volume->getDepth()).length();

	// upload geometry
	BoxSlicer slicer;
	slicer.slice(volume->getBounds(), camera, refSampleLength * samplingScale, limitSamples ? minSlices : -1, limitSamples ? maxSlices : -1);
	proxyIndices.bind();
	proxyIndices.data(&slicer.getIndices()[0], slicer.getIndices().size() * sizeof(GLushort));
	proxyVertices.bind();
	proxyVertices.data(&slicer.getVertices()[0], slicer.getVertices().size() * sizeof(slicer.getVertices()[0]));
	numSliceIndices = static_cast<int>(slicer.getIndices().size());

	this->currentNumSlices = slicer.sliceCount();
	float actualSamplingLength = slicer.samplingLength();
	float slRatio = actualSamplingLength / refSampleLength;

	glUniform1f(boxShader.getUniform("opacity_correction"), slRatio);
	glUniform1f(boxShader.getUniform("sampling_length"), actualSamplingLength);
	glUniform1f(boxShader.getUniform("jitter_size"), 32.0f);
}


unsigned VolumeController::getCurrentNumSlices()
{
	return currentNumSlices;
}

void VolumeController::markDirty()
{
	dirty = true;
}

void VolumeController::setMode(VolumeController::RenderMode mode)
{
	this->renderMode = mode;
	markDirty();
}

void VolumeController::cycleMode()
{
	renderMode = (VolumeController::RenderMode)((renderMode + 1) % VolumeController::NUM_OF_MODES);
	markDirty();
}

VolumeController::RenderMode VolumeController::getMode()
{
	return renderMode;
}

void VolumeController::toggleShading()
{
	shading = !shading;
	markDirty();
}

bool VolumeController::useShading()
{
	return shading;
}

void VolumeController::setCLUTTexture(Texture& texture)
{
	this->clutTexture = texture;
	markDirty();
}

float VolumeController::getOpacityScale()
{
	return opacityScale;
}

void VolumeController::setOpacityScale(float scale)
{
	this->opacityScale = min(max(0.0f, scale), 1.0f);
	markDirty();
}

std::unique_ptr<Menu> VolumeController::contextMenu()
{
	Menu* menu = new Menu("Volume Renderer");

	MenuItem& mip = menu->createItem("MIP");
	mip.setAction([&]{
		setMode(MIP);
		MainController::getInstance().menuController().hideMenu();
	});

	MenuItem& dvr = menu->createItem("DVR");
	dvr.setAction([&]{ 
		setMode(VR);
		MainController::getInstance().menuController().hideMenu();
	});

	MenuItem& iso = menu->createItem("Isosurface");
	iso.setAction([&]{
		setMode(ISOSURFACE);
		MainController::getInstance().menuController().hideMenu();
	});

	MenuItem& mi_projection = menu->createItem("Projection");
	mi_projection.setAction([&]{
		camera.perspective(!camera.perspective());
		markDirty();
		MainController::getInstance().menuController().hideMenu();
	});


	MenuItem& mi_background = menu->createItem("Background");
	mi_background.setAction([&]{
		MainController::getInstance().getRenderer().invertBG();
		markDirty();
		MainController::getInstance().menuController().hideMenu();
	});

	return std::unique_ptr<Menu>(menu);
}