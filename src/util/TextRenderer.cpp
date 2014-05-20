#include "TextRenderer.h"
#include <fstream>
#include <iostream>

using namespace gl;
using namespace std;

static const char* vSrc = "\
#version 330\n\
uniform mat4 model_projection;\n\
layout (location = 0) in vec4 vs_position;\n\
layout (location = 1) in vec2 vs_texcoord;\n\
layout (location = 2) in vec4 vs_color;\n\
out vec2 fs_texcoord;\n\
out vec4 fs_color;\n\
void main()\n\
{\n\
    gl_Position = model_projection * vs_position;\n\
    fs_texcoord = vs_texcoord;\n\
	fs_color = vs_color;\n\
}";

static const char* fSrc = "\
#version 330\n\
uniform sampler2D texSampler;\n\
in vec2 fs_texcoord;\n\
in vec4 fs_color;\n\
out vec4 display_color;\n\
void main()\n\
{\n\
    float intensity = texture(texSampler, fs_texcoord).r;\n\
    display_color = fs_color * intensity;\n\
}";

// texture coordinate stepping based on a 16x16 grid of glyphs
static const float GLYPH_STEP = 0.0625f;

TextRenderer::TextRenderer() : font_(nullptr), dirty_(true)
{
}

TextRenderer::~TextRenderer()
{
    for (map<string, Font*>::iterator it = fonts.begin(); it != fonts.end(); it++) {
        delete it->second;
    }
}

bool TextRenderer::loadFont(const std::string& fontName)
{
	if (!prog_.id()) {
		prog_ = Program::createFromSrc(vSrc, fSrc);
	}

	std::string bmpFileName = std::string("fonts/" + fontName + ".bmp");
	std::string metricsFileName = std::string("fonts/" + fontName + ".dat");

	Font* font = new Font;
	if (!font->load(bmpFileName.c_str(), metricsFileName.c_str())) {
		delete font;
		return false;
	}

	fonts[fontName] = font;
	if (!font_) {
		font_ = font;
		dirty_ = true;
	}

	return (font != 0);
}

void TextRenderer::font(const std::string& fontName)
{
	if (fonts.find(fontName) != fonts.end()) {
		font_ = fonts[fontName];
		dirty_ = true;
	}
}

void TextRenderer::model(const Mat4& model)
{
	model_ = model;
	model_projection_ = viewport_.orthoProjection() * model_;
}

void TextRenderer::viewport(const Viewport& viewport)
{
	viewport_ = viewport;
	model_projection_ = viewport_.orthoProjection() * model_;
	dirty_ = true;
}

void TextRenderer::add(const std::string& text, float x, float y)
{
	labels_.push_back({ text, { x, y }, draw_state_ });
	dirty_ = true;
}

void TextRenderer::add(const std::string& text, const Vec2& position)
{
	labels_.push_back({ text, position, draw_state_ });
	dirty_ = true;
}

void TextRenderer::clear()
{
	labels_.clear();
	dirty_ = true;
}

void TextRenderer::draw()
{
	if (dirty_) {
		buffer();
	}

	if (vertex_count_ == 0) {
		return;
	}

	viewport_.apply();

	prog_.enable();
	prog_.uniform("model_projection", model_projection_);

	vbo_.bind();
	font_->texture.bind();

	const GLsizei stride = 8 * sizeof(GLfloat);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, false, stride, (GLvoid*)(0 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, false, stride, (GLvoid*)(2 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 4, GL_FLOAT, false, stride, (GLvoid*)(4 * sizeof(GLfloat)));

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDrawArrays(GL_TRIANGLES, 0, vertex_count_);
	glDisable(GL_BLEND);
}

void TextRenderer::buffer()
{
	if (!vbo_.id()) {
		vbo_.generateVBO(GL_DYNAMIC_DRAW);
	}

	vertex_count_ = 0;
	vector<GLfloat> vertices;

	for (const Label& label : labels_) {

		auto vertex = [&](float x, float y, float u, float v) {
			vertices.push_back(x);
			vertices.push_back(y);
			vertices.push_back(u);
			vertices.push_back(v);
			vertices.push_back(label.draw_state.color.x);
			vertices.push_back(label.draw_state.color.y);
			vertices.push_back(label.draw_state.color.z);
			vertices.push_back(label.draw_state.color.w);
			vertex_count_++;
		};

		float x = label.position.x;
		float y = label.position.y;

		if (label.draw_state.h_align == HAlign::center) {
			x -= font_->width(label.text) / 2;
		} else if (label.draw_state.h_align == HAlign::right) {
			x -= font_->width(label.text);
		}

		if (label.draw_state.v_align == VAlign::center) {
			y -= font_->glyphHeight / 2;
		} else if (label.draw_state.v_align == VAlign::top) {
			y -= font_->glyphHeight;
		}

		for (const char& c : label.text) {
			int glyphWidth = font_->glyphWidths[static_cast<unsigned>(c)];
			int glyphHeight = font_->glyphHeight;

			GLfloat u = GLYPH_STEP * (c % 16);
			GLfloat v = 1.0f - GLYPH_STEP * (c / 16 + 1);
			float uStep = (float)glyphWidth / font_->texture.width();

			vertex(x, y, u, v);
			vertex(x + glyphWidth, y, u + uStep, v);
			vertex(x + glyphWidth, y + glyphHeight, u + uStep, v + GLYPH_STEP);

			vertex(x, y, u, v);
			vertex(x + glyphWidth, y + glyphHeight, u + uStep, v + GLYPH_STEP);
			vertex(x, y + glyphHeight, u, v + GLYPH_STEP);

			x += glyphWidth;
		}
	}

	vbo_.bind();

	if (!vertices.empty()) {
		vbo_.data(&vertices[0], vertices.size() * sizeof(GLfloat));
	}

	dirty_ = false;
}

int TextRenderer::fontHeight()
{
    return font_->glyphHeight;
}

int TextRenderer::fontWidth(const std::string& text)
{
	return font_->width(text);
}

unsigned int TextRenderer::Font::width(const std::string& s)
{
	int width = 0;
	for (const char& c : s)
		width += glyphWidths[static_cast<unsigned>(c)];
	return width;
}

bool TextRenderer::Font::load(const char* bmpFileName, const char* metricsFileName)
{
    ifstream fin(bmpFileName, ios::in | ios::binary);
    if (!fin) {
        cerr << "File not found: " + std::string(bmpFileName) << endl;
        return false;
    }
    
    // read the byte offset for pixel data start
    fin.seekg(0x0A, ios::beg);
    unsigned short dataStart = 0;
    fin.read((char*)&dataStart, sizeof(unsigned char));
    
    // read the image dimensions
	int width, height;
    fin.seekg(0x12, ios::beg);
    fin.read((char*)&width, sizeof(unsigned int));
    fin.read((char*)&height, sizeof(unsigned int));
    
    // read bit planes
    unsigned short planes = 0;
    fin.read((char*)&planes, sizeof(unsigned short));
    if (planes != 1) {
        cerr << "Number of color planes must be 1" << endl;
        return false;
    }
    
    // read bits per pixel
    unsigned short bpp = 0;
    fin.read((char*)&bpp, sizeof(unsigned short));
    if (bpp != 24) {
        cerr << "Image must be 24 bits per pixel (BGR format)" << endl;
        return false;
    }
    
    // read the pixel data
	unsigned size = width * height * 3;
    unsigned char* data = new unsigned char[size];
    fin.seekg(dataStart, ios::beg);
    fin.read((char*)data, sizeof(unsigned char) * size);
    fin.close();
    
    // process into texture
	texture.generate(GL_TEXTURE_2D);
	texture.bind();
	texture.setParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	texture.setParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	texture.setParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	texture.setParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	texture.setData2D(GL_RGB, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);
    delete[] data;
    
    // glyphs (assuming 16x16 in texture) and monospaced
    ifstream mfile(metricsFileName, ios::in | ios::binary);
    if (mfile) {
        mfile.read((char*)glyphWidths, sizeof(unsigned char)*256);
        mfile.close();
    } else {
        for (int i = 0; i < 256; i++)
			glyphWidths[i] = width / 16;
    }
    glyphHeight = height / 16;
    
    return true;
}