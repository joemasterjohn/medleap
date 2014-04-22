#ifndef __GL_PROGRAM_H__
#define __GL_PROGRAM_H__

#include "gl/glew.h"
#include "gl/Shader.h"
#include <memory>
#include <vector>
#include "gl/math/Math.h"

namespace gl
{
    /** A compiled and linked set of vertex and fragment shaders. */
    class Program
    {
    public:
        Program();

		~Program();
        
		/** Returns the handle to the OpenGL resource, or 0 if none. */
		GLuint id() const;

		/** Clears this pointer. Detaches all shaders. */
		void release();
        
		/** Creates a new OpenGL resource. This object will point to it. */
		void generate();

		/** Attached a shader to this program */
		void attach(const Shader& shader);

		/** Attempts to link attached shaders. Returns true if successful */
		bool link();

        GLint getUniform(const GLchar* name) const;
        
        GLint getAttribute(const GLchar* name) const;
        
        void enable();
        void disable();

		void uniform(const GLchar* name, bool value);
		void uniform(const GLchar* name, float x);
		void uniform(const GLchar* name, float x, float y);
		void uniform(const GLchar* name, float x, float y, float z);
		void uniform(const GLchar* name, float x, float y, float z, float w);
		void uniform(const GLchar* name, const Vec2& v);
		void uniform(const GLchar* name, const Vec3& v);
		void uniform(const GLchar* name, const Vec4& v);
		void uniform(const GLchar* name, const Mat4& m);

        
        static Program create(const char* vsrc, const char* fsrc);
        static Program createFromSrc(const char* vsrc, const char* fsrc);
        static Program create(const Shader& vShader, const Shader& fShader);

    private:
		std::shared_ptr<GLuint> handle_;
		std::vector<gl::Shader> attached_;
    };
}

#endif // __GL_PROGRAM_H__
