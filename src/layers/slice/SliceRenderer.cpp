#include "SliceRenderer.h"
#include "gl/math/Transform.h"
#include "main/MainController.h"

using namespace gl;

SliceRenderer::SliceRenderer() :
    volume(NULL),
    currentSlice(0)
{
	sliceShader = Program::create("shaders/slice_clut.vert", "shaders/slice_clut.frag");
	sliceShader.enable();
	glUniform1i(sliceShader.getUniform("tex_slice"), 0);
	glUniform1i(sliceShader.getUniform("tex_clut"), 1);

	sliceTexture.generate(GL_TEXTURE_2D);

	// geometry is simply a textured quad
	// uniform matrix will scale to correct aspect ratio
	GLfloat vertexData[] = {
		-1, -1, 0, 0,
		1, -1, 1, 0,
		1, 1, 1, 1,
		-1, -1, 0, 0,
		1, 1, 1, 1,
		-1, 1, 0, 1
	};
	sliceVBO = Buffer::genVertexBuffer();
	sliceVBO.bind();
	sliceVBO.setData(vertexData, sizeof(vertexData));
}

void SliceRenderer::setCLUTTexture(gl::Texture& texture)
{
    this->clutTexture = texture;
}

void SliceRenderer::updateTexture()
{
    sliceTexture.bind();
    sliceTexture.setParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    sliceTexture.setParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    
	GLenum internalFormat;
	switch (volume->getType()) {
		case GL_UNSIGNED_BYTE: internalFormat = GL_R8; break;
		case GL_UNSIGNED_SHORT: internalFormat = GL_R16; break;
		case GL_BYTE: internalFormat = GL_R8_SNORM; break;
		case GL_SHORT: internalFormat = GL_R16_SNORM; break;
		default: internalFormat = GL_RED; break;
	}

    sliceTexture.setData2D(0,
                            internalFormat,
                            volume->getWidth(),
                            volume->getHeight(),
                            volume->getFormat(),
                            volume->getType(),
                            volume->getData() + currentSlice * volume->getSliceSizeBytes());
}

void SliceRenderer::setVolume(VolumeData* volume)
{
    this->volume = volume;
	currentSlice = 0;
    updateTexture();
    resize(viewport.width, viewport.height);
}

void SliceRenderer::resize(int width, int height)
{
    // model matrix will scale to keep the displayed image in proportion to its
    // intended dimensions without changing the input vertices in NDC
    float windowAspect = (float)width / height;
    float sliceAspect = (float)volume->getWidth() / volume->getHeight();
    
    modelMatrix = (sliceAspect <= 1.0f) ?
        scale(sliceAspect / windowAspect, 1.0f, 1.0f) :
        scale(1.0f, windowAspect / sliceAspect, 1.0f);
}

void SliceRenderer::draw()
{
	sliceShader.enable();

	glActiveTexture(GL_TEXTURE1);
	clutTexture.bind();
	glActiveTexture(GL_TEXTURE0);
	sliceTexture.bind();

	// set the uniforms
	glUniform1i(sliceShader.getUniform("signed_normalized"), volume->isSigned());
	glUniform1f(sliceShader.getUniform("window_min"), volume->getCurrentWindow().getMinNorm());
	glUniform1f(sliceShader.getUniform("window_multiplier"), 1.0f / volume->getCurrentWindow().getWidthNorm());
	glUniformMatrix4fv(sliceShader.getUniform("model"), 1, false, modelMatrix);

	// set state and shader for drawing medical stuff
	GLsizei stride = 4 * sizeof(GLfloat);
	sliceVBO.bind();

	int loc = sliceShader.getAttribute("vs_position");
	glEnableVertexAttribArray(loc);
	glVertexAttribPointer(loc, 2, GL_FLOAT, false, stride, 0);

	loc = sliceShader.getAttribute("vs_texcoord");
	glEnableVertexAttribArray(loc);
	glVertexAttribPointer(loc, 2, GL_FLOAT, false, stride, (GLvoid*)(2 * sizeof(GLfloat)));

	glDrawArrays(GL_TRIANGLES, 0, 6);
}

int SliceRenderer::getCurrentSlice()
{
    return currentSlice;
}

void SliceRenderer::setCurrentSlice(int sliceIndex)
{
    currentSlice = sliceIndex;
    updateTexture();
}