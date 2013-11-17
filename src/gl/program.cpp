#include "program.h"

Program* generateProgram(Shader* vShader, Shader* fShader);

Program::Program()
{
}

Program::Program(GLuint id, Shader* vShader, Shader* fShader) : 
	id(id),
	vShader(vShader),
	fShader(fShader)
{
}

Program::~Program()
{
	glDetachShader(id, vShader->getID());
	glDetachShader(id, fShader->getID());
	glDeleteProgram(id);
	delete vShader;
	delete fShader;
}

GLuint Program::getID() const
{
	return id;
}

GLint Program::getUniform(const GLchar* name) const
{
	return glGetUniformLocation(id, name);
}

GLint Program::getAttribute(const GLchar* name) const
{
	return glGetAttribLocation(id, name);
}

void Program::enable()
{
	glUseProgram(id);
}

void Program::disable()
{
	glUseProgram(0);
}

Program* Program::create(const char* vsrc, const char* fsrc)
{
	Shader* vShader = new Shader;
	if (!vShader->compileFile(vsrc, GL_VERTEX_SHADER)) {
		std::cerr << "ERROR compiling vertex shader:" << std::endl << vShader->log() << std::endl;
		delete vShader;
		return NULL;
	}

	Shader* fShader = new Shader;
	if (!fShader->compileFile(fsrc, GL_FRAGMENT_SHADER)) {
		std::cerr << "ERROR compiling vertex shader:" << std::endl << fShader->log() << std::endl;
		delete fShader;
		return NULL;
	}

    return create(vShader, fShader);
}

Program* Program::createFromSrc(const char* vsrc, const char* fsrc)
{
	Shader* vShader = new Shader;
	if (!vShader->compile(vsrc, GL_VERTEX_SHADER)) {
		std::cerr << "ERROR compiling vertex shader:" << std::endl << vShader->log() << std::endl;
		delete vShader;
		return NULL;
	}
    
	Shader* fShader = new Shader;
	if (!fShader->compile(fsrc, GL_FRAGMENT_SHADER)) {
		std::cerr << "ERROR compiling vertex shader:" << std::endl << fShader->log() << std::endl;
		delete fShader;
		return NULL;
	}
    
    return create(vShader, fShader);
}

Program* Program::create(Shader* vShader, Shader* fShader)
{
    GLuint id = glCreateProgram();
	glAttachShader(id, vShader->getID());
	glAttachShader(id, fShader->getID());
    
	glLinkProgram(id);
	GLint status = 0;
	glGetProgramiv(id, GL_LINK_STATUS, &status);
	if (!status) {
		std::cerr << "ERROR linking shader program:" << std::endl;
        
		GLint maxLength = 0;
		glGetProgramiv(id, GL_INFO_LOG_LENGTH, &maxLength);
		GLchar* buf = new GLchar[maxLength];
        
		glGetProgramInfoLog(id, maxLength, &maxLength, buf);
		std::string log(buf);
		std::cerr << log << std::endl;
        
		delete[] buf;
		glDeleteProgram(id);
		delete vShader;
		delete fShader;
		return NULL;
	}
    
    return new Program(id, vShader, fShader);
}