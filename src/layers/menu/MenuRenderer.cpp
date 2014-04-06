#include "MenuRenderer.h"

MenuRenderer::MenuRenderer()
{
}

MenuRenderer::~MenuRenderer()
{
}

void MenuRenderer::init()
{
}

#include <iostream>
void MenuRenderer::draw()
{
	static int frame = 0;
	std::cout << frame++ << std::endl;
}

void MenuRenderer::resize(int width, int height)
{
}
