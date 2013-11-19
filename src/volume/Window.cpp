#include "Window.h"

Window::Window(float center, float width) : center(center), width(width)
{
    updateMinMax();
}

void Window::setCenter(float center)
{
    this->center = center;
    updateMinMax();
}

void Window::setWidth(float width)
{
    this->width = width;
    updateMinMax();
}

float Window::getCenter()
{
    return center;
}

float Window::getWidth()
{
    return width;
}

float Window::getMin()
{
    return min;
}

float Window::getMax()
{
    return max;
}

void Window::updateMinMax()
{
    min = center - width / 2.0f;
    max = center + width / 2.0f;
}

void Window::setCenterNormalized(float value, GLenum type)
{
    switch (type)
    {
        case GL_BYTE:
            center = value * 255 - 128;
            break;
        case GL_UNSIGNED_BYTE:
            center = value * 255.0;
            break;
        case GL_SHORT:
            center = value * 65535 - 32768;
            break;
        case GL_UNSIGNED_SHORT:
            center = value * 65535;
            break;
        default:
            center = value;
    }
}

void Window::setWidthNormalized(float value, GLenum type)
{
    switch (type)
    {
        case GL_BYTE:
        case GL_UNSIGNED_BYTE:
            width = value * 255.0;
            break;
        case GL_SHORT:
        case GL_UNSIGNED_SHORT:
            width = value * 65535;
            break;
        default:
            width = value;
    }
}

float Window::getCenterNormalized(GLenum type)
{
    switch (type)
    {
        case GL_BYTE:
            return (center + 128.0) / 255.0;
        case GL_UNSIGNED_BYTE:
            return center / 255.0;
        case GL_SHORT:
            return (center + 32768.0) / 65535.0;
        case GL_UNSIGNED_SHORT:
            return center / 65535.0;
        default:
            return center;
    }
}

float Window::getWidthNormalized(GLenum type)
{
    switch (type)
    {
        case GL_BYTE:
        case GL_UNSIGNED_BYTE:
            return width / 255.0;
        case GL_SHORT:
        case GL_UNSIGNED_SHORT:
            return width / 65535.0;
        default:
            return center;
    }
}