#include "VolumeRenderer.h"
#include "gl/math/Transform.h"
#include "BoxSlicer.h"
#include "gl/geom/Box.h"
#include "gl/util/Draw.h"

using namespace gl;
using namespace std;

VolumeRenderer::VolumeRenderer()
{
    dirty = true;
    numSamples = 256;
    opacityScale = 1.0f;
    renderMode = VR;
    shading = true;
    drawnHighRes = false;
	lightBackground = false;
	cursorActive = false;
	cursorRadius = 0.05;
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

int VolumeRenderer::getNumSamples()
{
    return numSamples;
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

	fullResRT.generate(viewport.width, viewport.height, true);
	lowResRT.generate(viewport.width/2, viewport.height/2, true);
	fullScreenQuad.generate();

	cursor3DShader = Program::create("shaders/menu.vert", "shaders/menu.frag");
	cursor3DVBO = Buffer::genVertexBuffer();
	cursor3DVBO.bind();
	Box(cursorRadius*2).fill(cursor3DVBO);
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
	lowResRT.resize(width, height);
	camera.setProjection(perspective(0.8726388f, viewport.aspect(), 0.1f, 100.0f));
    markDirty();
}

void VolumeRenderer::updateSlices(int numSlices)
{
    BoxSlicer slicer;
    slicer.slice(volume->getBounds(), camera, numSlices);
    
    proxyIndices.bind();
    proxyIndices.setData(&slicer.getIndices()[0], slicer.getIndices().size() * sizeof(GLushort));
    
    proxyVertices.bind();
    proxyVertices.setData(&slicer.getVertices()[0], slicer.getVertices().size() * sizeof(slicer.getVertices()[0]));
    
    numSliceIndices = static_cast<int>(slicer.getIndices().size());
}

void VolumeRenderer::draw(int numSlices)
{
	glEnable(GL_DEPTH_TEST);
	{
		cursor3DShader.enable();
		cursor3DVBO.bind();
		glUniform4f(cursor3DShader.getUniform("color"), 0.0f, cursorActive ? 1.0f : 0.0f, 1.0f, 1.0f);
		glUniformMatrix4fv(cursor3DShader.getUniform("modelViewProjection"), 1, false, camera.getProjection() * camera.getView() * translation(cursor3D));
		int loc = cursor3DShader.getAttribute("vs_position");
		glEnableVertexAttribArray(loc);
		glVertexAttribPointer(loc, 3, GL_FLOAT, false, 0, 0);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

    // proxy geometry
    glActiveTexture(GL_TEXTURE2);
    clutTexture.bind();
    glActiveTexture(GL_TEXTURE1);
    gradientTexture.bind();
    glActiveTexture(GL_TEXTURE0);
    volumeTexture.bind();
    
    updateSlices(numSlices);
    
    
    boxShader.enable();
    
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

	glUniform1f(boxShader.getUniform("opacity_correction"), static_cast<float>(numSamples) / numSlices);
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
	std::cout << cursorRadiusSS << std::endl;
	glUniform1f(boxShader.getUniform("cursor_radius_ss"), cursorRadius);

	glUniform2f(boxShader.getUniform("window_size"), viewport.width, viewport.height);


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
    
	static Draw d;
	d.setModelViewProj(gl::ortho2D(0, viewport.width, 0, viewport.height));
	d.begin(GL_LINES);
	d.color(1, 0, 0);
	d.circle(cpss.x, cpss.y, cursorRadiusSS, 32);
	d.end();
	d.draw();

}

void VolumeRenderer::draw()
{
    glClearColor(1, 1, 1, 1);
 
    static int cleanFrames = 0;
	static gl::Texture currentTexture;

    // draw to texture
    if (dirty) {
		lowResRT.bind();
		lowResRT.clear();
        draw(numSamples);
		lowResRT.unbind();
        dirty = false;
        drawnHighRes = false;
        cleanFrames = 1;
        currentTexture = lowResRT.getColorTarget();
    } else if (!drawnHighRes && cleanFrames++ > 30) {
		fullResRT.bind();
		fullResRT.clear();
        draw(numSamples * 8);
		fullResRT.unbind();
        drawnHighRes = true;
        currentTexture = fullResRT.getColorTarget();
    }

    // draw from texture to screen
    viewport.apply();
	fullScreenQuad.draw(currentTexture);
}