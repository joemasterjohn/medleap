#ifndef __CSC529_SHADER__
#define __CSC529_SHADER__

#include <GL/glew.h>
#include <iostream>
#include <string>

/// GLSL shader object reference.
class Shader
{
public:
    /// Creates a new empty shader reference.
    Shader();
    
    // Deletes the shader object; also calls release.
    ~Shader();
    
    /// Creates a new shader reference.
    Shader(GLuint id);
    
    /// Returns the referenced shader's ID (0 if no shader referenced).
    GLuint getID() const;
    
    /// Returns the type of the referenced shader (0 if no shader referenced).
    GLenum getType() const;
    
    /// Returns the source code (empty string if no shader referenced).
    std::string getSource() const;
    
    /// Returns the error log from compiling (empty string if no errors).
    std::string log() const;
    
    /// Assigns source and compiles. Returns false if compiler errors. If this
    /// object does not yet reference a shader, a new one will be created.
    bool compile(const char* source, GLenum type);
    
    /// Assigns source from a file and compiles. Returns false if compiler
    /// errors. If this object does not yet reference a shader, a new one will
    /// be created.
    bool compileFile(const char* fileName, GLenum type);
    
    /// Deletes the referenced shader from the OpenGL context.
    void release();
    
private:
    GLuint id;
    GLenum type;
    std::string source;
};

#endif // __CSC529_SHADER__
