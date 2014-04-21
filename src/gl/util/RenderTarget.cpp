#include "RenderTarget.h"

using namespace gl;

RenderTarget::RenderTarget() : intColorFormat(GL_RGB), colorFormat(GL_RGB), colorType(GL_UNSIGNED_BYTE), depthFormat(GL_DEPTH_COMPONENT16)
{
}

void RenderTarget::setColorFormat(GLenum format) {
	colorFormat = format;
}

void RenderTarget::setInternalColorFormat(GLenum format) {
	intColorFormat = format;
}

void RenderTarget::setColorType(GLenum type) {
	colorType = type;
}

void RenderTarget::setDepthFormat(GLenum format) {
	depthFormat = format;
}

const gl::Texture& RenderTarget::getColorTarget() {
	return colorTarget;
}

void RenderTarget::clear()
{
	if (useDepth)
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	else
		glClear(GL_COLOR_BUFFER_BIT);
}

void RenderTarget::generate(GLsizei width, GLsizei height, bool useDepth) {
	this->useDepth = useDepth;

	framebuffer.generate();
	framebuffer.bind();

	colorTarget.generate(GL_TEXTURE_2D);
	colorTarget.bind();
	colorTarget.setParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	colorTarget.setParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	colorTarget.setParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	colorTarget.setParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	colorTarget.setData2D(intColorFormat, width, height, colorFormat, colorType, 0);
	colorTarget.unbind();
	framebuffer.colorTarget(0, colorTarget);

	if (useDepth) {
		depthTarget.generate();
		depthTarget.bind();
		depthTarget.storage(GL_DEPTH_COMPONENT16, width, height);
		depthTarget.unbind();
		framebuffer.depthTarget(depthTarget);
	}

	framebuffer.unbind();
}

void RenderTarget::resize(GLsizei width, GLsizei height) {
	colorTarget.bind();
	colorTarget.setData2D(intColorFormat, width, height, colorFormat, colorType, 0);
	colorTarget.unbind();
	if (useDepth) {
		depthTarget.bind();
		depthTarget.storage(depthFormat, width, height);
		depthTarget.unbind();
	}
}

void RenderTarget::bind() {
	framebuffer.bind();
	glViewport(0, 0, colorTarget.width(), colorTarget.height());
}

void RenderTarget::unbind() {
	framebuffer.unbind();
}