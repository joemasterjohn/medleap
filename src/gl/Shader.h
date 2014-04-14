#ifndef __CSC529_SHADER__
#define __CSC529_SHADER__

#include "gl/glew.h"
#include <string>
#include <memory>

namespace gl
{
    /** Pointer to an OpenGL shader object. */
    class Shader
    {
    public:
        /** Creates a new empty shader reference. */
        Shader();
        
		/** Returns the handle to the OpenGL resource, or 0 if none. */
		GLuint id() const;

		/** Clears this pointer. If no other objects point to the OpenGL resource, it will be destroyed. */
		void release();
        
        /** Returns the type of the referenced shader (0 if no shader referenced). */
        GLenum getType() const;
        
        /** Returns the source code (empty string if no shader referenced). */
        std::string getSource() const;
        
        /** Returns the error log from compiling (empty string if no errors). */
        std::string log() const;
        
        /** Assigns source and compiles. Returns false if compiler errors. If this object does not yet reference a shader, a new one will be created. */
        bool compile(const char* source, GLenum type);
        
        /** Assigns source from a file and compiles. Returns false if compiler errors. If this object does not yet reference a shader, a new one will be created. */
        bool compileFile(const char* fileName, GLenum type);
        
    private:
		std::shared_ptr<GLuint> handle;
        GLenum type;
        std::string source;

		void generate(GLenum type);
    };
    
}

#endif // __CSC529_SHADER__
