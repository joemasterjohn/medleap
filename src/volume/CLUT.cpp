#include "CLUT.h"
#include <algorithm>

CLUT::ColorStop::ColorStop(float position, float r, float g, float b, float a) : position(position), color(r,g,b,a)
{
}

CLUT::ColorStop::ColorStop(float position, cgl::Vec4 color) : position(position), color(color)
{
}

float CLUT::ColorStop::getPosition()
{
    return position;
}

void CLUT::ColorStop::setPosition(float position)
{
    this->position = position;
}

cgl::Vec4 CLUT::ColorStop::getColor()
{
    return color;
}

void CLUT::ColorStop::setColor(float r, float g, float b, float a)
{
    color.x = r;
    color.y = g;
    color.z = b;
    color.w = a;
}

void CLUT::ColorStop::setColor(cgl::Vec4 color)
{
    this->color = color;
}



CLUT::CLUT()
{
    stops.push_back(ColorStop(0.0f, 0.0f, 0.0f, 0.0f, 0.0f));
    stops.push_back(ColorStop(1.0f, 1.0f, 1.0f, 1.0f, 1.0f));
}

CLUT::~CLUT()
{
}

int CLUT::findLeftStop(float position)
{
    int i = stops.size() - 1;
    while (stops[i].getPosition() > position) { i--; }
    return i;
}

int CLUT::findRightStop(float position)
{
    int i = 0;
    while (stops[i].getPosition() < position) { i++; }
    return i;
}

int CLUT::findNearestStop(float pos)
{
    // find the nearest color stop on the left of position
    int left = findLeftStop(pos);
    
    // if left is the last color stop, it must be the closest
    if (left == stops.size() - 1)
        return left;
    
    // otherwise, return the closer of the left and right stops
    ColorStop& l = stops[left];
    ColorStop& r = stops[left+1];
    if (pos - l.getPosition() < r.getPosition() - pos)
        return left;
    return left+1;
}

void CLUT::addColorStop(float position, cgl::Vec4 color)
{
    // if no stops, just add it
    if (stops.empty()) {
        stops.push_back(ColorStop(position, color));
        return;
    }
    
    int left = findLeftStop(position);
    
    std::min(3,4);
    
    // if the new color stop has the same position as an existing color stop we silently fail to add it
    int right = std::min((int)(stops.size()-1), left+1);
    if (stops[left].getPosition() == position || stops[right].getPosition() == position)
        return;
    
    stops.insert(stops.begin() + left + 1, ColorStop(position, color));
}

void CLUT::removeColorStop(float position)
{
    int nearest = findNearestStop(position);
    stops.erase(stops.begin() + nearest);
}

CLUT::ColorStop& CLUT::getColorStop(float position)
{
    return stops[findNearestStop(position)];
}

cgl::Vec4 CLUT::getColor(float position)
{
    // make sure position is in [0,1]
    position = std::min(std::max(position, 0.0f), 1.0f);
    
    // find the nearest color stops on the left and right of position
    ColorStop* left = &stops[findLeftStop(position)];
    ColorStop* right = &stops[findRightStop(position)];
    
    // left and right will be the same color stop if position is exactly the same as the stop's position
    if (left == right)
        return left->getColor();
    
    // calculate the position normalized w.r.t. left and right color stops
    float lp = left->getPosition();
    float rp = right->getPosition();
    float np = (position - lp) / (rp - lp);
    
    // return linearly interpolated color
    return left->getColor() * (1.0f - np) + right->getColor() * np;
}

void CLUT::clearStops()
{
    stops.clear();
}

void CLUT::saveTexture(cgl::Texture* texture)
{
    int l = 0;
    int r = 1;
    ColorStop* left = &stops[l];
    ColorStop* right = &stops[r];
    
    unsigned char buf[255*4];
    long ptr = 0;
    for (int i = 0; i < 256; i++) {
        float p = i/255.0f;
        if (p > right->getPosition()) {
            left = &stops[++l];
            right = &stops[++r];
        }
        
        float pn = (p - left->getPosition()) / (right->getPosition() - left->getPosition());
        
        cgl::Vec4 color = left->getColor() * (1.0f - pn) + right->getColor() * pn;
        buf[ptr++] = (unsigned char)(color.x * 255);
        buf[ptr++] = (unsigned char)(color.y * 255);
        buf[ptr++] = (unsigned char)(color.z * 255);
        buf[ptr++] = (unsigned char)(color.w * 255);
    }
    
    texture->bind();
    texture->setParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    texture->setParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    texture->setParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    texture->setData1D(0, GL_RGBA, 256, GL_RGBA, GL_UNSIGNED_BYTE, buf);
}