#include "Program.h"
#include <iostream>

using namespace gl;

Program::Program() : handle_(nullptr)
{
}

Program::~Program()
{
	if (handle_.use_count() == 1) {
		for (Shader& shader : attached_) {
			if (shader.id()) {
				glDetachShader(id(), shader.id());
			}
		}
	}
}

GLuint Program::id() const
{
	return handle_ ? *(handle_.get()) : 0;
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
	handle_ = std::shared_ptr<GLuint>(p, deleteFunction);
}

void Program::release()
{
	handle_ = nullptr;
	attached_.clear();
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
	attached_.push_back(shader);
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

void Program::uniform(const GLchar* name, bool value)
{
	glUniform1i(glGetUniformLocation(id(), name), value);
}

void Program::uniform(const GLchar* name, float x)
{
	glUniform1f(glGetUniformLocation(id(), name), x);
}

void Program::uniform(const GLchar* name, float x, float y)
{
	glUniform2f(glGetUniformLocation(id(), name), x, y);
}

void Program::uniform(const GLchar* name, float x, float y, float z)
{
	glUniform3f(glGetUniformLocation(id(), name), x, y, z);
}

void Program::uniform(const GLchar* name, float x, float y, float z, float w)
{
	glUniform4f(glGetUniformLocation(id(), name), x, y, z, w);
}

void Program::uniform(const GLchar* name, const Vec2& v)
{
	glUniform2f(glGetUniformLocation(id(), name), v.x, v.y);
}

void Program::uniform(const GLchar* name, const Vec3& v)
{
	glUniform3f(glGetUniformLocation(id(), name), v.x, v.y, v.z);
}

void Program::uniform(const GLchar* name, const Vec4& v)
{
	glUniform4f(glGetUniformLocation(id(), name), v.x, v.y, v.z, v.w);
}

void Program::uniform(const GLchar* name, const Mat4& m)
{
	glUniformMatrix4fv(glGetUniformLocation(id(), name), 1, false, m);
}