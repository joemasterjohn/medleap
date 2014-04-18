#include "MainRenderer.h"
#include "MainConfig.h"

MainRenderer::MainRenderer()
{
    leftDocking.layerIndex = -1;
    rightDocking.layerIndex = -1;
    bottomDocking.layerIndex = -1;
    topDocking.layerIndex = -1;
	bgColor = Vec3(1, 1, 1);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
}

MainRenderer::~MainRenderer()
{
	glDeleteVertexArrays(1, &vao);
}

Vec3 MainRenderer::getBackgroundColor() const
{
	return bgColor;
}

Vec3 MainRenderer::getInverseBGColor() const
{
	return Vec3(1.0f) - bgColor;
}

void MainRenderer::setBackgroundColor(const Vec3& c) {
	bgColor = c;
}

void MainRenderer::setLightBG() {
	bgColor = Vec3(1.0f);
}

void MainRenderer::setDarkBG() {
	bgColor = Vec3(0.0f);
}

void MainRenderer::draw(int width, int height)
{
    glClearColor(bgColor.x, bgColor.y, bgColor.z, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    
    // draw from stack
    int layer = 0;
    for (Renderer* r : activeLayers) {
        updateViewport(r, layer, width, height);
        r->draw();
        layer++;
    }
}

void MainRenderer::updateViewport(Renderer* renderer, int layer, int width, int height)
{
    int x = 0;
    int y = 0;
    int w = width;
    int h = height;
    
    if (layer == bottomDocking.layerIndex) {
		h = static_cast<int>(h * bottomDocking.percent);
    } else if (layer == topDocking.layerIndex) {
        y += static_cast<int>(height * (1.0 - topDocking.percent));
		h = static_cast<int>(h * topDocking.percent);
    } else if (layer == leftDocking.layerIndex) {
		w = static_cast<int>(w * leftDocking.percent);
    } else if (layer == rightDocking.layerIndex) {
		x += static_cast<int>(width * (1.0 - rightDocking.percent));
		w = static_cast<int>(w * rightDocking.percent);
    }
    
    if (layer < bottomDocking.layerIndex) {
		y += static_cast<int>(height * bottomDocking.percent);
		h = static_cast<int>(h * (1.0 - bottomDocking.percent));
    }
    
    if (layer < topDocking.layerIndex) {
		h = static_cast<int>(h * (1.0 - topDocking.percent));
    }
    
    if (layer < leftDocking.layerIndex) {
		x += static_cast<int>(width * leftDocking.percent);
		w = static_cast<int>(w * (1.0 - leftDocking.percent));
    }
    
    if (layer < rightDocking.layerIndex) {
		w = static_cast<int>(w * (1.0 - leftDocking.percent));
    }
    
    renderer->setViewport(x, y, w, h);
    renderer->getViewport().apply();
}

void MainRenderer::pushLayer(Renderer* layer)
{
    activeLayers.push_back(layer);
}

void MainRenderer::dockLeft(Renderer* layer, double percent)
{
    leftDocking.percent = percent;
    leftDocking.layerIndex = static_cast<int>(activeLayers.size());
    pushLayer(layer);
}

void MainRenderer::dockRight(Renderer* layer, double percent)
{
    rightDocking.percent = percent;
    rightDocking.layerIndex = static_cast<int>(activeLayers.size());
    pushLayer(layer);
}

void MainRenderer::dockBottom(Renderer* layer, double percent)
{
    bottomDocking.percent = percent;
	bottomDocking.layerIndex = static_cast<int>(activeLayers.size());
    pushLayer(layer);
}

void MainRenderer::dockTop(Renderer* layer, double percent)
{
    topDocking.percent = percent;
	topDocking.layerIndex = static_cast<int>(activeLayers.size());
    pushLayer(layer);
}

Renderer* MainRenderer::popLayer()
{
    if (activeLayers.empty())
        return NULL;
    
    Renderer* popped = activeLayers.back();
    activeLayers.pop_back();
    
    // check if this was a docked layer and remove if so
    if (leftDocking.layerIndex == activeLayers.size())
        leftDocking.layerIndex = -1;
    if (rightDocking.layerIndex == activeLayers.size())
        rightDocking.layerIndex = -1;
    if (bottomDocking.layerIndex == activeLayers.size())
        bottomDocking.layerIndex = -1;
    if (topDocking.layerIndex == activeLayers.size())
        topDocking.layerIndex = -1;
    
    return popped;
}

void MainRenderer::clearLayers()
{
    activeLayers.clear();
    leftDocking.layerIndex = -1;
    rightDocking.layerIndex = -1;
    bottomDocking.layerIndex = -1;
    topDocking.layerIndex = -1;
}
