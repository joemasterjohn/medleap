#include "Util.h"
#include <cstring>
#include <vector>
#include <cmath>

namespace gl
{

	int sizeOf(GLenum typeEnum)
	{
		switch (typeEnum)
		{
		case GL_BYTE:
		case GL_UNSIGNED_BYTE:
			return sizeof(GLbyte);
		case GL_SHORT:
		case GL_UNSIGNED_SHORT:
			return sizeof(GLshort);
		case GL_INT:
		case GL_UNSIGNED_INT:
			return sizeof(GLint);
		case GL_FLOAT:
			return sizeof(GLfloat);
		case GL_DOUBLE:
			return sizeof(GLdouble);
		default:
			return 0;
		}
	}

	const char* toString(GLenum glEnum)
	{
		switch (glEnum)
		{
		case GL_UNSIGNED_BYTE:
			return "GL_UNSIGNED_BYTE";
		case GL_BYTE:
			return "GL_BYTE";
		case GL_UNSIGNED_SHORT:
			return "GL_UNSIGNED_SHORT";
		case GL_SHORT:
			return "GL_SHORT";
		default:
			return "UNKNOWN";
		}
	}

	void flipImage(void* buffer, int width, int height, int pixelSize)
	{
		char* buf = (char*)buffer;
		int rowSize = pixelSize * width;

		std::vector<char> temp(rowSize);
		for (int i = 0; i < height / 2; i++) {

			// swap rows A and B
			char* a = buf + i * rowSize;
			char* b = buf + (height - 1 - i) * rowSize;
			std::copy(a, a + rowSize, temp.begin());
			std::copy(b, b + rowSize, a);
			std::copy(temp.begin(), temp.end(), b);

		}
	}


	float linearR(float x)
	{
		return 0.5f * (x + 1.0f);
	}

	float linearC(float x)
	{
		return 1.0f - std::abs(x);
	}

	float linearL(float x)
	{
		return -0.5f * (x - 1.0f);
	}

	float quadraticR(float x)
	{
		float a = x - 1.0f;
		return 1.0f - 0.25f * a * a;
	}

	float quadraticC(float x)
	{
		return 1.0f - x * x;
	}

	float quadraticL(float x)
	{
		float a = x + 1.0f;
		return 1.0f - 0.25f * a * a;
	}

	std::function<float(float)> exponentialR(float s)
	{
		const float b = 1.0f / (1.0f + std::exp(s));
		const float c = 1.0f / (1.0f + std::exp(-s));
		const float d = 1.0f / (c - b);

		return [=](float x)->float
		{
			return (1.0f / (1.0f + std::exp(-s * x)) - b) * d;
		};
	}

	std::function<float(float)> exponentialC(float s)
	{
		const float b = std::exp(-s);
		const float c = 1.0f / (1.0f - b);

		return[=](float x)->float
		{
			return (std::exp(-s * x * x) - b) * c;
		};
	}

	std::function<float(float)> exponentialL(float s)
	{
		const float b = 1.0f / (1.0f + std::exp(s));
		const float c = 1.0f / (1.0f + std::exp(-s));
		const float d = 1.0f / (b - c);

		return[=](float x)->float
		{
			return (1.0f / (1.0f + std::exp(-s * x)) - c) * d;
		};
	}

	Vec2 denormalize(const Viewport& viewport, const Leap::Frame& frame, const Leap::Vector& vector)
	{
		Leap::Vector v = frame.interactionBox().normalizePoint(vector);
		float x = viewport.width * v.x;
		float y = viewport.height * v.y;
		return{ x, y };
	}
}