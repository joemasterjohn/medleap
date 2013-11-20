#include <fstream>
#include <sstream>
#include "Shader.h"

Shader::Shader() : id(0), type(0)
{
}

Shader::~Shader()
{
    release();
}

GLuint Shader::getID() const
{
    return id;
}

GLenum Shader::getType() const
{
    return type;
}

std::string Shader::getSource() const
{
    return (id ? source : std::string());
}

std::string Shader::log() const
{
    if (!id)
        return std::string();
    
    GLint maxLength = 0;
    glGetShaderiv(id, GL_INFO_LOG_LENGTH, &maxLength);
    GLchar* buf = new GLchar[maxLength];
    
    glGetShaderInfoLog(id, maxLength, &maxLength, buf);
    std::string log(buf);
    delete[] buf;
    
    return log;
}

bool Shader::compile(const char* src, GLenum type)
{
    if (!id)
        id = glCreateShader(type);
    
    this->type = type;
    this->source = std::string(src);
    
    glShaderSource(id, 1, &src, NULL);
    glCompileShader(id);
    
    GLint status;
    glGetShaderiv(id, GL_COMPILE_STATUS, &status);

    return status;
}

bool Shader::compileFile(const char* fileName, GLenum type)
{
    std::ifstream in(fileName);
    std::string src((std::istreambuf_iterator<char>(in)),
                    std::istreambuf_iterator<char>());
    return compile(src.c_str(), type);
}

void Shader::release()
{
    if (id) {
        glDeleteShader(id);
        id = 0;
    }
}
