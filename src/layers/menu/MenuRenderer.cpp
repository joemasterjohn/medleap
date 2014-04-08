#include "MenuRenderer.h"
#include "main/MainController.h"
#include "util/Util.h"

MenuRenderer::MenuRenderer(MenuManager* menuManager) : menuManager(menuManager), highlighted(-1)
{
}

MenuRenderer::~MenuRenderer()
{
}

void MenuRenderer::init()
{
}

void MenuRenderer::highlight(int menuIndex)
{
	highlighted = menuIndex;
}

void MenuRenderer::draw()
{
	// TODO: draw background
	drawMenu(menuManager->topMenu());
}

void MenuRenderer::drawMenu(Menu& menu)
{
	TextRenderer& text = MainController::getInstance().getText();

	text.begin(viewport.width, viewport.height);

	text.setColor(.5f, .5f, .5f);
	text.add(
		menu.getName(),
		viewport.width / 2,
		viewport.height,
		TextRenderer::CENTER,
		TextRenderer::TOP);

	double radius = std::min(viewport.width, viewport.height) * 0.35;
	double angle = 0.0;
	double angleStep = gl::PI2 / menu.getItems().size();
	for (MenuItem& item : menu.getItems()) {
		int x = static_cast<int>(std::cos(angle) * radius + viewport.width / 2);
		int y = static_cast<int>(std::sin(angle) * radius + viewport.height / 2);
		text.add(item.getName(), x, y, TextRenderer::CENTER, TextRenderer::CENTER);
		angle += angleStep;
	}

	text.end();

	// draw highlighted
	if (highlighted >= 0 && highlighted < menu.getItems().size()) {
		double angle = angleStep * highlighted;
		int x = static_cast<int>(std::cos(angle) * radius + viewport.width / 2);
		int y = static_cast<int>(std::sin(angle) * radius + viewport.height / 2);

		text.setColor(1, 1, 1);
		text.begin(viewport.width, viewport.height);
		text.add(menu.getItems()[highlighted].getName(), x, y, 
			TextRenderer::CENTER, TextRenderer::CENTER);
		text.end();
	}

}

void MenuRenderer::resize(int width, int height)
{
}
