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
        icon_point_circle,
        icon_three_circle,
        icon_l_closed,
        icon_l_open,
        icon_v_closed,
        icon_v_open,
        icon_carry,
        icon_fist,
        icon_palms_face,
        icon_pinch,
        icon_point,
        icon_point2,
        icon_three,
        icon_thumb,
		num_icons,
	};

	LeapStateController();
	void draw() override;
	void clear();
	void add(Icon icon, const std::string& label);
    void increaseBrightness(Icon icon);

private:
	struct DisplayedIcon
	{
		Icon icon;
		std::string label;
        float brightness;
	};
	
	TextRenderer text_;
	std::vector<gl::Texture> textures_;
	std::vector<DisplayedIcon> displayed_;
	gl::Buffer icon_vbo_;
	gl::Program icon_prog_;
};

#endif // __medleap_LeapStateController__
