#include "Window.h"
#include <iostream>

Window::Window(GLenum type)
{
    switch (type) {
        case GL_BYTE:
            typeMin = -128;
            typeRange = 255;
            break;
        case GL_UNSIGNED_BYTE:
            typeMin = 0;
            typeRange = 255;
            break;
        case GL_SHORT:
            typeMin = -32768;
            typeRange = 65535;
            break;
        case GL_UNSIGNED_SHORT:
            typeMin = 0;
            typeRange = 65535;
            break;
        default:
            std::cerr << "Window constructed with unknown type" << std::endl;
    }
    
    setNorm(0.5f, 1.0f);
}

void Window::setReal(float center, float width)
{
    centerReal = center;
    widthReal = width;
    centerNorm = (centerReal - typeMin) / typeRange;
    widthNorm = widthReal / typeRange;
    
    minReal = centerReal - widthReal / 2.0f;
    maxReal = centerReal + widthReal / 2.0f;
    minNorm = centerNorm - widthNorm / 2.0f;
    maxNorm = centerNorm + widthNorm / 2.0f;
}

void Window::setNorm(float center, float width)
{
    centerNorm = center;
    widthNorm = width;
    centerReal = centerNorm * typeRange + typeMin;
    widthReal = widthNorm * typeRange;
    
    minReal = centerReal - widthReal / 2.0f;
    maxReal = centerReal + widthReal / 2.0f;
    minNorm = centerNorm - widthNorm / 2.0f;
    maxNorm = centerNorm + widthNorm / 2.0f;
}

float Window::getCenterReal() { return centerReal; }
float Window::getWidthReal() { return widthReal; }
float Window::getMinReal() { return minReal; }
float Window::getMaxReal() { return maxReal; }

float Window::getCenterNorm() { return centerNorm; }
float Window::getWidthNorm() { return widthNorm; }
float Window::getMinNorm() { return minNorm; }
float Window::getMaxNorm() { return maxNorm; }