#ifndef __medleap_LeapStateController__
#define __medleap_LeapStateController__

#include "layers/Controller.h"
#include "gl/Buffer.h"
#include "gl/Program.h"
#include "gl/Texture.h"
#include "util/TextRenderer.h"
#include <vector>
#include <set>

class LeapStateController : public Controller
{
public:
	enum Icon
	{
		icon_h1f1_point,
		icon_h1f2_point,
		icon_h1f3_point,
		icon_h1f4_point,
		icon_h1f5_point,
		icon_h1f1_circle,
		icon_h1f2_circle,
		icon_h1f2_trigger,
		icon_h2f1_point,
		num_icons,
		icon_none
	};

	LeapStateController();
	void draw() override;
	void clear();
	void add(Icon icon, const std::string& label);
	void active(Icon icon);

private:
	struct DisplayedIcon
	{
		Icon icon;
		std::string label;
	};
	
	TextRenderer text_;
	std::vector<gl::Texture> textures_;
	std::vector<DisplayedIcon> displayed_;
	Icon active_;
	gl::Buffer icon_vbo_;
	gl::Program icon_prog_;
};

#endif // __medleap_LeapStateController__
