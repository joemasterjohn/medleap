#include "BoundingBox.h"


using namespace std;

BoundingBox::BoundingBox(float width, float height, float length) : size(width, height, length)
{
    float hw = width / 2.0f;
    float hh = height / 2.0f;
    float hl = length / 2.0f;
    
    vertices.push_back(Vec4(-hw, -hh,  hl, 1));
    vertices.push_back(Vec4( hw, -hh,  hl, 1));
    vertices.push_back(Vec4( hw,  hh,  hl, 1));
    vertices.push_back(Vec4(-hw,  hh,  hl, 1));
    vertices.push_back(Vec4(-hw, -hh, -hl, 1));
    vertices.push_back(Vec4( hw, -hh, -hl, 1));
    vertices.push_back(Vec4( hw,  hh, -hl, 1));
    vertices.push_back(Vec4(-hw,  hh, -hl, 1));
    
    edges.push_back(BoundingBox::Edge(0,1));
    edges.push_back(BoundingBox::Edge(1,2));
    edges.push_back(BoundingBox::Edge(2,3));
    edges.push_back(BoundingBox::Edge(3,0));
    edges.push_back(BoundingBox::Edge(4,5));
    edges.push_back(BoundingBox::Edge(5,6));
    edges.push_back(BoundingBox::Edge(6,7));
    edges.push_back(BoundingBox::Edge(7,4));
    edges.push_back(BoundingBox::Edge(0,4));
    edges.push_back(BoundingBox::Edge(1,5));
    edges.push_back(BoundingBox::Edge(2,6));
    edges.push_back(BoundingBox::Edge(3,7));
}

float BoundingBox::getWidth() const
{
    return size.x;
}

float BoundingBox::getHeight() const
{
    return size.y;
}

float BoundingBox::getLength() const
{
    return size.z;
}

Vec3 BoundingBox::getMinimum() const
{
    return -size / 2;
}

Vec3 BoundingBox::getMaximum() const
{
    return size / 2;
}

const vector<Vec4>& BoundingBox::getVertices() const
{
    return vertices;
}

const vector<BoundingBox::Edge>& BoundingBox::getEdges() const
{
    return edges;
}