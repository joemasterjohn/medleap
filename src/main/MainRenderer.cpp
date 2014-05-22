#include "MainRenderer.h"
#include "MainConfig.h"

using namespace gl;

MainRenderer::MainRenderer()
{
    leftDocking.layerIndex = -1;
    rightDocking.layerIndex = -1;
    bottomDocking.layerIndex = -1;
    topDocking.layerIndex = -1;
	bgColor = Vec3(0.0f);

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

void MainRenderer::invertBG()
{
	bgColor = getInverseBGColor();
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
	for (Controller* r : activeLayers) {
        updateViewport(r, layer, width, height);
        r->draw();
        layer++;
    }
}

void MainRenderer::updateViewport(Controller* renderer, int layer, int width, int height)
{
    int x = 0;
    int y = 0;
    int w = width;
    int h = height;
    
    if (layer == bottomDocking.layerIndex) {
		h = std::max(static_cast<int>(h * bottomDocking.percent), bottomDocking.pixels);
    } else if (layer == topDocking.layerIndex) {
        y += std::max(static_cast<int>(height * (1.0 - topDocking.percent)), topDocking.pixels);
		h = std::max(static_cast<int>(h * topDocking.percent), topDocking.pixels);
    } else if (layer == leftDocking.layerIndex) {
		w = std::max(static_cast<int>(w * leftDocking.percent), leftDocking.pixels);
    } else if (layer == rightDocking.layerIndex) {
		x += std::max(static_cast<int>(width * (1.0 - rightDocking.percent)), rightDocking.pixels);
		w = std::max(static_cast<int>(w * rightDocking.percent), rightDocking.pixels);
    }
    
    if (layer < bottomDocking.layerIndex) {
		y += std::max(static_cast<int>(height * bottomDocking.percent), bottomDocking.pixels);
		h = std::min(static_cast<int>(h * (1.0 - bottomDocking.percent)), height - bottomDocking.pixels);
    }
    
    if (layer < topDocking.layerIndex) {
		h = std::min(static_cast<int>(h * (1.0 - topDocking.percent)), h - topDocking.pixels);
    }
    
    if (layer < leftDocking.layerIndex) {
		x += std::max(static_cast<int>(width * leftDocking.percent), leftDocking.pixels);
		w = std::min(static_cast<int>(w * (1.0 - leftDocking.percent)), w - leftDocking.pixels);
    }
    
    if (layer < rightDocking.layerIndex) {
		w = static_cast<int>(w * (1.0 - leftDocking.percent));
    }
    
    renderer->setViewport(x, y, w, h);
    renderer->getViewport().apply();
}

void MainRenderer::pushLayer(Controller* layer)
{
    activeLayers.push_back(layer);
}

void MainRenderer::dockLeft(Controller* layer, double percent, int pixels)
{
    leftDocking.percent = percent;
    leftDocking.layerIndex = static_cast<int>(activeLayers.size());
	leftDocking.pixels = pixels;
    pushLayer(layer);
}

void MainRenderer::dockRight(Controller* layer, double percent, int pixels)
{
    rightDocking.percent = percent;
    rightDocking.layerIndex = static_cast<int>(activeLayers.size());
	rightDocking.pixels = pixels;
    pushLayer(layer);
}

void MainRenderer::dockBottom(Controller* layer, double percent, int pixels)
{
    bottomDocking.percent = percent;
	bottomDocking.layerIndex = static_cast<int>(activeLayers.size());
	bottomDocking.pixels = pixels;
    pushLayer(layer);
}

void MainRenderer::dockTop(Controller* layer, double percent, int pixels)
{
    topDocking.percent = percent;
	topDocking.layerIndex = static_cast<int>(activeLayers.size());
	topDocking.pixels = pixels;
    pushLayer(layer);
}

Controller* MainRenderer::popLayer()
{
    if (activeLayers.empty())
        return NULL;
    
	Controller* popped = activeLayers.back();
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
