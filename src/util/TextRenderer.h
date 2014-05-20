#ifndef __MEDLEAP_TEXT_RENDERER__
#define __MEDLEAP_TEXT_RENDERER__

#include "gl/glew.h"
#include "gl/Program.h"
#include "gl/Buffer.h"
#include "gl/Texture.h"
#include "gl/Viewport.h"
#include <string>
#include <vector>
#include <map>

class TextRenderer
{
public:
	enum class HAlign {
		left, center, right
	};

	enum class VAlign {
		bottom, center, top
	};
    
    TextRenderer();
    
    ~TextRenderer();
    
    /**
     * Loads a font in the fonts directory by name. Example: menlo14 will load the files
     * fonts/menlo14.bmp and fonts/menlo14.dat. The first font to be loaded will be used
     * by default. Multiple fonts may be loaded.
     */
    bool loadFont(const std::string& fontName);
    
	/// Sets font for all strings
    void font(const std::string& name);

	/// Sets viewing area
	void viewport(const gl::Viewport& viewport);

	/// Sets model matrix
	void model(const gl::Mat4& model);
    
	/// Sets color for next string
	void color(float r, float g, float b) { draw_state_.color.set(r, g, b, 1.0f); }

	/// Sets color for next string
	void color(float r, float g, float b, float a) { draw_state_.color.set(r, g, b, a); }

	/// Sets color for next string
	void color(const gl::Vec3& c) { draw_state_.color.set(c.x, c.y, c.z, 1.0f); }

	/// Sets color for next string
	void color(const gl::Vec4& c) { draw_state_.color.set(c.x, c.y, c.z, c.w); }

	/// Sets horizontal alignment for next string
	void hAlign(HAlign align) { draw_state_.h_align = align; }

	/// Sets vertical alignment for next string
	void vAlign(VAlign align) { draw_state_.v_align = align; }
    
	/// Adds a string at window coordinates (x, y)
	void add(const std::string& text, float x, float y);

	/// Adds a string at window coordinates position
	void add(const std::string& text, const gl::Vec2& position);

	/// Removes all strings
	void clear();

	/// Draws all strings
	void draw();

	/// Height of current font in pixels
    int fontHeight();

	/// Width of string with current font in pixels
    int fontWidth(const std::string& text);
  
private:
	struct DrawState
	{
		gl::Vec4 color;
		HAlign h_align;
		VAlign v_align;
	};
    
	struct Label
	{
		std::string text;
		gl::Vec2 position;
		DrawState draw_state;
	};

    class Font
    {
    public:        
		gl::Texture texture;
        unsigned char glyphWidths[256];
        unsigned char glyphHeight;
        
        unsigned int width(const std::string& s);
        bool load(const char* bmpFileName, const char* metricsFileName);
    };
    
	gl::Mat4 model_;
	gl::Mat4 model_projection_;
	gl::Viewport viewport_;
	std::vector<Label> labels_;
	DrawState draw_state_;
    gl::Program prog_;
	gl::Buffer vbo_;
    std::map<std::string, Font*> fonts;
    Font* font_;
    int windowWidth, windowHeight;
	bool dirty_;
	GLsizei vertex_count_;
    
	void buffer();
};

#endif // __MEDLEAP_TEXT_RENDERER__