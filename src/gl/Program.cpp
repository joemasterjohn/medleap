#include "Program.h"
#include <iostream>

using namespace gl;

Program::Program() : handle(nullptr)
{
}

Program::~Program()
{
	if (handle.use_count() == 1) {
		for (Shader& shader : attached) {
			if (shader.id()) {
				glDetachShader(id(), shader.id());
			}
		}
	}
}

GLuint Program::id() const
{
	return handle ? *(handle.get()) : 0;
}

void Program::generate()
{
	auto deleteFunction = [=](GLuint* p) {
		if (p) {
			glDeleteProgram(*p);
			delete p;
		}
	};

	GLuint* p = new GLuint(glCreateProgram());
	handle = std::shared_ptr<GLuint>(p, deleteFunction);
}

void Program::release()
{
	handle = nullptr;
	attached.clear();
}

GLint Program::getUniform(const GLchar* name) const
{
	return glGetUniformLocation(id(), name);
}

GLint Program::getAttribute(const GLchar* name) const
{
	return glGetAttribLocation(id(), name);
}

void Program::enable()
{
	glUseProgram(id());
}

void Program::disable()
{
	glUseProgram(0);
}

void Program::attach(const Shader& shader)
{
	glAttachShader(id(), shader.id());
	attached.push_back(shader);
}

bool Program::link()
{
	glLinkProgram(id());

	GLint status = 0;
	glGetProgramiv(id(), GL_LINK_STATUS, &status);
	if (!status) {
		std::cerr << "ERROR linking shader program:" << std::endl;

		GLint maxLength = 0;
		glGetProgramiv(id(), GL_INFO_LOG_LENGTH, &maxLength);
		GLchar* buf = new GLchar[maxLength];

		glGetProgramInfoLog(id(), maxLength, &maxLength, buf);
		std::string log(buf);
		std::cerr << log << std::endl;

		delete[] buf;
		return false;
	}

	return true;
}

Program Program::create(const char* vsrc, const char* fsrc)
{
	Shader vShader;
	if (!vShader.compileFile(vsrc, GL_VERTEX_SHADER)) {
		std::cerr << "ERROR compiling vertex shader:" << std::endl << vShader.log() << std::endl;
		return Program();
	}

	Shader fShader;
	if (!fShader.compileFile(fsrc, GL_FRAGMENT_SHADER)) {
		std::cerr << "ERROR compiling fragment shader:" << std::endl << fShader.log() << std::endl;
		return Program();
	}

    return create(vShader, fShader);
}

Program Program::createFromSrc(const char* vsrc, const char* fsrc)
{
	Shader vShader;
	if (!vShader.compile(vsrc, GL_VERTEX_SHADER)) {
		std::cerr << "ERROR compiling vertex shader:" << std::endl << vShader.log() << std::endl;
		return Program();
	}

	Shader fShader;
	if (!fShader.compile(fsrc, GL_FRAGMENT_SHADER)) {
		std::cerr << "ERROR compiling fragment shader:" << std::endl << fShader.log() << std::endl;
		return Program();
	}

	return create(vShader, fShader);
}

Program Program::create(const Shader& vShader, const Shader& fShader)
{
	Program prog;
	prog.generate();
	prog.attach(vShader);
	prog.attach(fShader); 
	prog.link();
	return prog;
}