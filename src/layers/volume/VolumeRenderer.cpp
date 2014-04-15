#include "VolumeRenderer.h"
#include "gl/math/Transform.h"
#include "BoxSlicer.h"
#include "gl/util/Draw.h"
#include "main/MainConfig.h"

using namespace gl;
using namespace std;

VolumeRenderer::VolumeRenderer() : cursorGeom(1, 2)
{
    dirty = true;
    opacityScale = 1.0f;
    renderMode = VR;
    shading = true;
    drawnHighRes = false;
	lightBackground = false;
	cursorActive = false;
	cursorRadius = 0.1;
	useJitter = false;
	bgColor = Vec3(1, 1, 1);
}

VolumeRenderer::~VolumeRenderer()
{
}

Camera& VolumeRenderer::getCamera()
{
    return camera;
}

void VolumeRenderer::setVolume(VolumeData* volume)
{
    this->volume = volume;
    volume->loadTexture3D(volumeTexture);
    volume->loadGradientTexture(gradientTexture);
}

void VolumeRenderer::markDirty()
{
    dirty = true;
}

void VolumeRenderer::setMode(VolumeRenderer::RenderMode mode)
{
    this->renderMode = mode;
    markDirty();
}

void VolumeRenderer::cycleMode()
{
    renderMode = (VolumeRenderer::RenderMode)((renderMode + 1) % VolumeRenderer::NUM_OF_MODES);
    markDirty();
}

VolumeRenderer::RenderMode VolumeRenderer::getMode()
{
    return renderMode;
}

void VolumeRenderer::toggleShading()
{
    shading = !shading;
    markDirty();
}

bool VolumeRenderer::useShading()
{
    return shading;
}

void VolumeRenderer::setCLUTTexture(Texture& texture)
{
    this->clutTexture = texture;
    markDirty();
}

void VolumeRenderer::init()
{
	volumeTexture.generate(GL_TEXTURE_3D);
	gradientTexture.generate(GL_TEXTURE_3D);

	proxyVertices = Buffer::genVertexBuffer(GL_DYNAMIC_DRAW);
	proxyIndices = Buffer::genIndexBuffer(GL_DYNAMIC_DRAW);

    camera.setView(lookAt(1, 1, 1, 0, 0, 0, 0, 1, 0));
     
    boxShader = Program::create("shaders/volume_clut.vert", "shaders/volume_clut.frag");
    boxShader.enable();
    glUniform1i(boxShader.getUniform("tex_volume"), 0);
    glUniform1i(boxShader.getUniform("tex_gradients"), 1);
    glUniform1i(boxShader.getUniform("tex_clut"), 2);
	glUniform1i(boxShader.getUniform("tex_jitter"), 3);

	fullResRT.setInternalColorFormat(GL_RGB16F);
	fullResRT.generate(viewport.width, viewport.height, true);
	lowResRT.setInternalColorFormat(GL_RGB16F);
	lowResRT.generate(viewport.width/2, viewport.height/2, true);
	fullScreenQuad.generate();

	cursor3DShader = Program::create("shaders/menu.vert", "shaders/menu.frag");
	cursor3DVBO = Buffer::genVertexBuffer();
	cursor3DVBO.bind();
	cursorGeom.fill(cursor3DVBO);

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

float VolumeRenderer::getOpacityScale()
{
    return opacityScale;
}

void VolumeRenderer::setOpacityScale(float scale)
{
    this->opacityScale = min(max(0.0f, scale), 1.0f);
    markDirty();
}

void VolumeRenderer::resize(int width, int height)
{
	fullResRT.resize(width, height);
	lowResRT.resize(width/2, height/2);
	camera.setProjection(perspective(0.8726388f, viewport.aspect(), 0.1f, 100.0f));
    markDirty();
}

unsigned VolumeRenderer::getCurrentNumSlices()
{
	return currentNumSlices;
}

Vec3 VolumeRenderer::getBackgroundColor()
{
	return renderMode == MIP ? Vec3(0.0f) : bgColor;
}

void VolumeRenderer::updateSlices(double samplingScale, bool limitSamples)
{
	// calculate ideal number of samples as twice the number of voxels along view direction
	float ax = abs(camera.getForward().x);
	float ay = abs(camera.getForward().y);
	float az = abs(camera.getForward().z);
	Vec3 forwardRay(ax, ay, az);
	Vec3 volumeDim(volume->getWidth(), volume->getHeight(), volume->getDepth());
	int idealNumSamples = forwardRay.dot(volumeDim);

	// calculate number of slices to use, but don't go over max
	currentNumSlices = idealNumSamples * samplingScale;
	if (limitSamples)
		currentNumSlices = min(max(currentNumSlices, minSlices), maxSlices);

	glUniform1f(boxShader.getUniform("opacity_correction"), static_cast<float>(idealNumSamples) / currentNumSlices);
	glUniform1f(boxShader.getUniform("sampling_length"), 1.0f / currentNumSlices);
	glUniform1f(boxShader.getUniform("jitter_size"),  32.0f);

	// upload geometry
    BoxSlicer slicer;
	slicer.slice(volume->getBounds(), camera, currentNumSlices);
    proxyIndices.bind();
    proxyIndices.setData(&slicer.getIndices()[0], slicer.getIndices().size() * sizeof(GLushort));
    proxyVertices.bind();
    proxyVertices.setData(&slicer.getVertices()[0], slicer.getVertices().size() * sizeof(slicer.getVertices()[0]));
    numSliceIndices = static_cast<int>(slicer.getIndices().size());
}

void VolumeRenderer::draw(double samplingScale, bool limitSamples, int w, int h)
{



	//glEnable(GL_DEPTH_TEST);
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

    
    Mat4 mvp = camera.getProjection() * camera.getView();
    glUniformMatrix4fv(boxShader.getUniform("modelViewProjection"), 1, false, mvp);
	glUniformMatrix4fv(boxShader.getUniform("modelView"), 1, false, camera.getView());

    glUniform3fv(boxShader.getUniform("volumeMin"), 1, volume->getBounds().getMinimum());
    glUniform3fv(boxShader.getUniform("volumeDimensions"), 1, (volume->getBounds().getMaximum() - volume->getBounds().getMinimum()));
    glUniform1i(boxShader.getUniform("signed_normalized"), volume->isSigned());
    glUniform1i(boxShader.getUniform("use_shading"), (renderMode != MIP && shading));
    glUniform1f(boxShader.getUniform("window_min"), volume->getCurrentWindow().getMinNorm());
    glUniform1f(boxShader.getUniform("window_multiplier"), 1.0f / volume->getCurrentWindow().getWidthNorm());

	glUniform1i(boxShader.getUniform("render_mode"), renderMode);

	glUniform1i(boxShader.getUniform("use_jitter"), useJitter);




    glUniform3f(boxShader.getUniform("lightDirection"), -camera.getForward().x, -camera.getForward().y, -camera.getForward().z);
    
    glUniform3f(boxShader.getUniform("minGradient"), volume->getMinGradient().x, volume->getMinGradient().y, volume->getMinGradient().z);
    glUniform1f(boxShader.getUniform("opacity_scale"), opacityScale);
    Vec3 r = volume->getMaxGradient() - volume->getMinGradient();
    glUniform3f(boxShader.getUniform("rangeGradient"), r.x, r.y, r.z);
    
	glUniform3f(boxShader.getUniform("cursor_position"), cursor3D.x, cursor3D.y, cursor3D.z);




	

	Vec4 cpss = mvp * Vec4(cursor3D.x, cursor3D.y, cursor3D.z, 1.0);
	cpss /= cpss.w;
	cpss.x = (cpss.x + 1.0) * (viewport.width / 2.0);
	cpss.y = (cpss.y + 1.0) * (viewport.height / 2.0);
	glUniform3f(boxShader.getUniform("cursor_position_ss"), cpss.x, cpss.y, cpss.z);




	Vec4 cpee = camera.getView() * Vec4(cursor3D.x, cursor3D.y, cursor3D.z, 1.0f);
	glUniform3f(boxShader.getUniform("cursor_position_es"), cpee.x, cpee.y, cpee.z);


	glUniform1f(boxShader.getUniform("cursor_radius_ws"), cursorRadius);
	float cursorRadiusSS = gl::projectedRadius(0.8726388, (cursor3D - camera.getEye()).length(), cursorRadius) * viewport.height / 2.0f;
	glUniform1f(boxShader.getUniform("cursor_radius_ss"), cursorRadiusSS);

	glUniform2f(boxShader.getUniform("window_size"), w, h);


	glUniform1i(boxShader.getUniform("cursor_on"), true);

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
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            break;
        case ISOSURFACE:
            glDisable(GL_BLEND);
            glUniform1f(boxShader.getUniform("isoValue"), volume->getCurrentWindow().getCenterNorm());
            break;
        default:
            break;
    }
    
    
    
    glEnable(GL_PRIMITIVE_RESTART);
    glPrimitiveRestartIndex(65535);
    glDrawElements(GL_TRIANGLE_FAN, numSliceIndices, GL_UNSIGNED_SHORT, 0);
    glDisable(GL_PRIMITIVE_RESTART);
    
    
	glDisable(GL_DEPTH_TEST);

    glDisable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquation(GL_FUNC_ADD);
    
	//static Draw d;
	//d.setModelViewProj(gl::ortho2D(0, viewport.width, 0, viewport.height));
	//d.begin(GL_LINES);
	//d.color(1, 0, 0);
	//d.circle(cpss.x, cpss.y, cursorRadiusSS, 32);
	//d.end();
	//d.draw();

}

void VolumeRenderer::draw()
{
	if (renderMode == MIP)
		glClearColor(0, 0, 0, 0);
	else
		glClearColor(bgColor.x, bgColor.y, bgColor.z, 1);
 
    static int cleanFrames = 0;
	static gl::Texture currentTexture;

    // draw to texture
    if (dirty) {
		lowResRT.bind();
		lowResRT.clear();
        draw(0.5, true, lowResRT.getColorTarget().getWidth(), lowResRT.getColorTarget().getHeight());
		lowResRT.unbind();
        dirty = false;
        drawnHighRes = false;
        cleanFrames = 1;
        currentTexture = lowResRT.getColorTarget();
    } else if (!drawnHighRes && cleanFrames++ > 30) {
		fullResRT.bind();
		fullResRT.clear();
		draw(2.0, false, fullResRT.getColorTarget().getWidth(), fullResRT.getColorTarget().getHeight());
		fullResRT.unbind();
        drawnHighRes = true;
        currentTexture = fullResRT.getColorTarget();
    }

    // draw from texture to screen
    viewport.apply();
	fullScreenQuad.draw(currentTexture);
}