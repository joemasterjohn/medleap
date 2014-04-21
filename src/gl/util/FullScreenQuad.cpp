#include "FullScreenQuad.h"

using namespace gl;

static const char* vsrc = "\
#version 150\n\
in  vec4 vs_position;\n\
in  vec2 vs_texcoord;\n\
out vec2 fs_texcoord;\n\
void main()\n\
{\n\
    gl_Position = vs_position;\n\
    fs_texcoord = vs_texcoord;\n\
}";

static const char* fsrc = "\
#version 150\n\
uniform sampler2D texSampler;\n\
in vec2 fs_texcoord;\n\
out vec4 display_color;\n\
void main()\n\
{\n\
    display_color = texture(texSampler, fs_texcoord);\n\
}";

FullScreenQuad::FullScreenQuad()
{
}

void FullScreenQuad::generate() {
	GLfloat data[] = {
		-1.0f, -1.0f, 0.0f, 0.0f,
		+1.0f, -1.0f, 1.0f, 0.0f,
		+1.0f, +1.0f, 1.0f, 1.0f,
		-1.0f, -1.0f, 0.0f, 0.0f,
		+1.0f, +1.0f, 1.0f, 1.0f,
		-1.0f, +1.0f, 0.0f, 1.0f
	};

	vertexBuffer.generate(GL_ARRAY_BUFFER, GL_STATIC_DRAW);
	vertexBuffer.bind();
	vertexBuffer.data(data, sizeof(data));

	shaderProgram = Program::createFromSrc(vsrc, fsrc);
}

void FullScreenQuad::draw(const gl::Texture& texture) {
	texture.bind();
	shaderProgram.enable();
	vertexBuffer.bind();

	GLint loc = shaderProgram.getAttribute("vs_position");
	glEnableVertexAttribArray(loc);
	glVertexAttribPointer(loc, 2, GL_FLOAT, false, 4 * sizeof(GLfloat), 0);

	loc = shaderProgram.getAttribute("vs_texcoord");
	glEnableVertexAttribArray(loc);
	glVertexAttribPointer(loc, 2, GL_FLOAT, false, 4 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));

	glDrawArrays(GL_TRIANGLES, 0, 6);
}