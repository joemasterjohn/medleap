#include "viewport.h"
#include "gl/glew.h"

using namespace gl;

Viewport::Viewport() : x(0), y(0), width(1), height(1)
{
}

Viewport::Viewport(int x, int y, int width, int height)
: x(x), y(y), width(width), height(height)
{
}

float Viewport::aspect() const
{
    return (float)width/height;
}

void Viewport::apply() const
{
    glViewport(x, y, width, height);
}

bool Viewport::operator==(const Viewport& v) const
{
    return (x == v.x && y == v.y && width == v.width && height == v.height);
}

bool Viewport::operator!=(const Viewport& v) const
{
    return (x != v.x || y != v.y || width != v.width || height != v.height);
}

bool Viewport::contains(int x, int y) const
{
    return (x >= this->x &&
            x <= this->x + width &&
            y >= this->y &&
            y <= this->y + height);
}