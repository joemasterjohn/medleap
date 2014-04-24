#include "Transfer1DRenderer.h"
#include "main/MainController.h"
#include <string>
#include <sstream>
#include <iomanip>
#include <vector>
#include "gl/util/Draw.h"

using namespace std;
using namespace gl;

Transfer1DRenderer::Transfer1DRenderer()
{
	histo1D.generate(GL_TEXTURE_2D);
	transferFn.generate(GL_TEXTURE_2D);

	shader = Program::create("shaders/tf1d_histo.vert", "shaders/tf1d_histo.frag");
	histoProg = Program::create("shaders/tf1d_histo.vert", "shaders/tf1d_histo.frag");
	histoOutlineProg = Program::create("shaders/tf1d_histo_outline.vert", "shaders/tf1d_histo_outline.frag");
	colorShader = Program::create("shaders/histo_line.vert", "shaders/histo_line.frag");

	shader.enable();
	glUniform1i(shader.getUniform("tex_histogram"), 0);
	glUniform1i(shader.getUniform("tex_transfer"), 1);

	// vertex buffer for geometry: contains vertices for
	// 1) the histogram quad (drawn as a texture)
	// 2) the cursor / value marker line
	GLfloat vertexData[] = {
		// start of texture quad vertices
		-1, -0.5, 0, 0,
		1, -0.5, 1, 0,
		1, 1, 1, 1,
		-1, -0.5, 0, 0,
		1, 1, 1, 1,
		-1, 1, 0, 1,
		// start of cursor line vertices
		-1, -1, 0, 0,
		-1, 1, 0, 0

	};
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);

	stride = 4 * sizeof(GLfloat);

	// initialize clut strip
	clutTexture.generate(GL_TEXTURE_1D);

	{
		bgShader = Program::create("shaders/menu.vert", "shaders/menu.frag");
		bgShader.enable();

		GLfloat vertices[] = {
			-1, -1,
			+1, -1,
			+1, +1,
			-1, -1,
			+1, +1,
			-1, +1
		};

		bgBuffer.generate(GL_ARRAY_BUFFER, GL_STATIC_DRAW);
		bgBuffer.bind();
		bgBuffer.data(vertices, sizeof(vertices));
	}

}

Transfer1DRenderer::~Transfer1DRenderer()
{
}

void Transfer1DRenderer::setCLUT(CLUT* clut)
{
    this->clut = clut;
    if (clutTexture.id())
        clut->saveTexture(clutTexture);
}

Texture& Transfer1DRenderer::getCLUTTexture()
{
    return clutTexture;
}

void Transfer1DRenderer::draw()
{
	static const int totalHeight = 80;
	static const float colorBarHeight = 0.3f;
	static const int histoHeight = totalHeight;

	glViewport(viewport.x, viewport.y + viewport.height * 0.2f, viewport.width, viewport.height * 0.8f);
	drawHistogram();
	glViewport(viewport.x, viewport.y, viewport.width, viewport.height * 0.2f);
	drawBackground();
	drawMarkerBar();
}

void Transfer1DRenderer::drawMarkerBar()
{
	static Draw d;
	d.begin(GL_TRIANGLES);
	for (const CLUT::Marker& marker : clut->markers()) {
		Vec3 c = marker.color().vec3();
		float x = marker.interval().center();

		if (clut->mode() == CLUT::continuous) {
			x = x * clut->interval().width() + clut->interval().left();
		}

		// [0,1] to [-1,1]
		x = (x - 0.5f) * 2.0f;

		float l = x - 0.05f * 800.0f / viewport.width;
		float r = x + 0.05f * 800.0f / viewport.width;
		d.color(c.x, c.y, c.z);
		d.vertex(l, -1);
		d.vertex(x, .9f);
		d.vertex(r, -1);
	}
	d.end();
	d.draw();

	d.begin(GL_LINES);
	d.color(0.5f, 0.5f, 0.5f);
	for (const CLUT::Marker& marker : clut->markers()) {
		Vec3 c = marker.color().vec3();
		float x = marker.interval().center();
		if (clut->mode() == CLUT::continuous) {
			x = x * clut->interval().width() + clut->interval().left();
		}

		x = (x - 0.5f) * 2.0f;
		float l = x - 0.05f * 800.0f / viewport.width;
		float r = x + 0.05f * 800.0f / viewport.width;
		d.vertex(l, -1);
		d.vertex(x, .9f);
		d.vertex(x, .9f);
		d.vertex(r, -1);
	}
	d.end();
	d.draw();
}

void Transfer1DRenderer::drawBackground()
{
	bgBuffer.bind();
	bgShader.enable();
	glUniform4f(bgShader.getUniform("color"), 0.0f, 0.0f, 0.0f, 1.0f);
	glUniformMatrix4fv(bgShader.getUniform("modelViewProjection"), 1, false, Mat4());

	GLint loc = bgShader.getAttribute("vs_position");
	glEnableVertexAttribArray(loc);
	glVertexAttribPointer(loc, 2, GL_FLOAT, false, 2*sizeof(GLfloat), 0);

	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Transfer1DRenderer::drawHistogram()
{
	histoVBO.bind();
	histoProg.enable();
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, false, 0, 0);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, histoVBOCount);

	histoOutlineProg.enable();
	histoOutlineProg.uniform("color", 0.5f, 0.5f, 0.5f, 1.0f);
	glVertexAttribPointer(0, 2, GL_FLOAT, false, 4 * sizeof(GLfloat), 0);
	glDrawArrays(GL_LINE_STRIP, 0, histoVBOCount / 2);
}

void Transfer1DRenderer::resize(int width, int height)
{
}

void Transfer1DRenderer::setVolume(VolumeData* volume)
{
    this->volume = volume;
}

Texture& Transfer1DRenderer::getTransferFn()
{
    return transferFn;
}

void Transfer1DRenderer::setHistogram(Histogram* histogram)
{
    this->histogram = histogram;
    int drawWidth = histogram->getNumBins();
    int drawHeight = 256;
    
	std::vector<unsigned char> pixels(drawWidth * drawHeight);
    std::fill(pixels.begin(), pixels.end(), 0);
    
    double logMaxFreq = std::log(histogram->getMaxFrequency()+1);
    
    for (int bin = 0; bin < histogram->getNumBins(); bin++) {
        int size = histogram->getSize(bin);
        double sizeNorm = std::log(size+1) / logMaxFreq;
        
        int binHeight = (int)(sizeNorm * drawHeight);
        
        for (int j = 0; j < binHeight; j++) {
            pixels[bin + j * drawWidth] = 255;
        }
    }
    
    histo1D.bind();
    histo1D.setParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    histo1D.setParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    histo1D.setParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    histo1D.setParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	histo1D.setData2D(GL_RED, drawWidth, drawHeight, GL_RED, GL_UNSIGNED_BYTE, &pixels[0]);


	// histo vbo triangle strip
	{
		vector<Vec2> buffer;
		for (unsigned i = 0; i < histogram->getNumBins(); ++i) {
			float x = (float)i / (histogram->getNumBins()-1);
			float y = std::log(histogram->getSize(i) + 1) / logMaxFreq;
			x = (x - 0.5f) * 2.0f;
			y = (y - 0.5f) * 2.0f;
			buffer.push_back({ x, y });
			buffer.push_back({ x, -1.0f });
		}

		histoVBO.generateVBO(GL_STATIC_DRAW);
		histoVBO.bind();
		histoVBO.data(&buffer[0], buffer.size() * sizeof(Vec2));
		histoVBOCount = buffer.size();
	}
}

void Transfer1DRenderer::setCursor(int x, int y)
{
    if (!viewport.contains(x, y)) {
        return;
    }
    cursorX = x;
    cursorY = y;
    float px = static_cast<float>(x) / viewport.width;
    cursorValue = px * (histogram->getMax() - histogram->getMin()) + histogram->getMin();
    cursorShaderOffset = px * 2.0f;
}