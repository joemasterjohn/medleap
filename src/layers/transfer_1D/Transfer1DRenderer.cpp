#include "Transfer1DRenderer.h"
#include "gl/math/Transform.h"
#include "main/MainController.h"
#include <string>
#include <sstream>
#include <iomanip>
#include <vector>
#include "gl/util/Draw.h"

using namespace std;
using namespace gl;

// this only applies to CT modality
const char* hounsfield(float value)
{
    if (value <= -900)
        return "AIR";
    
    if (value <= -450 && value >= -550)
        return "LUNG";
    
    if (value <= -50 && value >= -100)
        return "FAT";
    
    if (value <= 5 && value >= -5)
        return "WATER";
    
    if (value <= 20 && value >= 10)
        return "MUSCLE";
    
    if (value <= 30 && value >= 20)
        return "MUSCLE/WHITE MATTER";
    
    if (value <= 37 && value >= 30)
        return "MUSCLE/BLOOD";
    
    if (value <= 40 && value >= 35)
        return "MUSCLE/BLOOD/GREY MATTER";
    
    if (value <= 45 && value >= 40)
        return "LIVER/BLOOD/GREY MATTER";
    
    if (value <= 60 && value >= 45)
        return "LIVER";
    
    if (value <= 300 && value >= 100)
        return "SOFT TISSUE";
    
    if (value >= 700)
        return "BONE";
    
    return "";
}

Transfer1DRenderer::Transfer1DRenderer()
{
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

void Transfer1DRenderer::init()
{
	histo1D.generate(GL_TEXTURE_2D);
	transferFn.generate(GL_TEXTURE_2D);

	shader = Program::create("shaders/histogram.vert", "shaders/histogram.frag");
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
	clut->saveTexture(clutTexture);
	clutStripShader = Program::create("shaders/clut_strip.vert",
		"shaders/clut_strip.frag");

	GLfloat clutVerts[] = {
		-1, -1, 0, 0, 0,
		0, -1, 1, 0, 0,
		0, -1, 0, 1, 1,
		1, -1, 0, 0, 1,
		-1, 1, 0, 0, 0,
		0, 1, 1, 0, 0,
		0, 1, 0, 1, 1,
		1, 1, 0, 0, 1
	};
	clutStripStride = 5 * sizeof(GLfloat);

	GLushort clutIndices[] = {
		0, 1, 5,
		0, 5, 4,
		1, 2, 6,
		1, 6, 5,
		2, 3, 7,
		2, 7, 6
	};

	clutStripVBO = Buffer::genVertexBuffer();
	clutStripVBO.bind();
	clutStripVBO.setData(clutVerts, sizeof(clutVerts));

	clutStripIBO = Buffer::genIndexBuffer();
	clutStripIBO.bind();
	clutStripIBO.setData(clutIndices, sizeof(clutIndices));

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
		bgBuffer.setData(vertices, sizeof(vertices));
	}
}

void Transfer1DRenderer::draw()
{
	static const int totalHeight = 80;
	static const int colorBarHeight = 30;
	static const int histoHeight = totalHeight;

	glViewport(viewport.x, viewport.y, viewport.width, totalHeight);
	//drawBackground();

	//glViewport(viewport.x, viewport.y, viewport.width, colorBarHeight);
	//drawColorBar();

	glViewport(viewport.x, viewport.y, viewport.width, viewport.height);
	drawHistogram();
    
	//glViewport(viewport.x, viewport.y + colorBarHeight, viewport.width, histoHeight);
    //drawWindowMarkers();
    
    if (drawCursor)
        drawCursorValue();
}

void Transfer1DRenderer::drawBackground()
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	bgBuffer.bind();

	bgShader.enable();
	glUniform4f(bgShader.getUniform("color"), 1.0f, 1.0f, 1.0f, 0.8f);
	glUniformMatrix4fv(bgShader.getUniform("modelViewProjection"), 1, false, Mat4());

	GLint loc = bgShader.getAttribute("vs_position");
	glEnableVertexAttribArray(loc);
	glVertexAttribPointer(loc, 2, GL_FLOAT, false, 2*sizeof(GLfloat), 0);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisable(GL_BLEND);
}

void Transfer1DRenderer::drawHistogram()
{
	static Draw d;
	double logMaxFreq = std::log(histogram->getMaxFrequency() + 1);

	float wc = volume->getCurrentWindow().getCenterReal();
	float ww = volume->getCurrentWindow().getWidthReal();
	float markL = (wc - ww / 2 - histogram->getMin()) / (histogram->getMax() - histogram->getMin()) * 2 - 1;
	float markR = (wc + ww / 2 - histogram->getMin()) / (histogram->getMax() - histogram->getMin()) * 2 - 1;

	d.begin(GL_TRIANGLE_STRIP);
	for (unsigned i = 0; i < histogram->getNumBins(); ++i) {
		float x = (float)i / histogram->getNumBins();
		float y = std::log(histogram->getSize(i) + 1) / logMaxFreq;

		x = (x - 0.5f) * 2.0f;
		y = (y - 0.5f) * 2.0f;


		Vec4 c = this->clut->getColor((x - markL) / (markR - markL));
		d.color(c.x, c.y, c.z);

		d.vertex(x, -1);
		d.vertex(x, y);
	}
	d.vertex(1, -1);
	d.vertex(1, -1);
	d.end();
	d.draw();


	d.begin(GL_LINE_STRIP);
	d.color(0, 0, 0);
	for (unsigned i = 0; i < histogram->getNumBins(); ++i) {
		float x = (float)i / histogram->getNumBins();
		float y = std::log(histogram->getSize(i) + 1) / logMaxFreq;
		x = (x - 0.5f) * 2.0f;
		y = (y - 0.5f) * 2.0f;

		d.vertex(x, y);
	}
	d.vertex(1, -1);
	d.vertex(1, -1);
	d.end();
	d.draw();

	d.begin(GL_LINES);
	d.color(0, 0, 0);
	d.vertex(-1, 1);
	d.vertex(1, 1);

	d.color(0, 0, 0);
	d.vertex(markL, -1);
	d.vertex(markL, 1);

	d.vertex(markR, -1);
	d.vertex(markR, 1);

	if (drawCursor) {
		d.color(0.5f, 0.5f, 0.5f);
		float x = ((float)cursorX / viewport.width - 0.5f) * 2.0f;
		d.vertex(x, -1);
		d.vertex(x, 1);
	}

	d.end();
	d.draw();



}

void Transfer1DRenderer::drawColorBar()
{
	clutStripShader.enable();
	clutStripVBO.bind();
	clutStripIBO.bind();
	clutTexture.bind();

	int loc = clutStripShader.getAttribute("vs_position");
	glEnableVertexAttribArray(loc);
	glVertexAttribPointer(loc, 4, GL_FLOAT, false, clutStripStride, 0);

	loc = clutStripShader.getAttribute("vs_texcoord");
	glEnableVertexAttribArray(loc);
	glVertexAttribPointer(loc, 1, GL_FLOAT, false, clutStripStride, (GLvoid*)(4 * sizeof(GLfloat)));

	float wc = volume->getCurrentWindow().getCenterReal();
	float ww = volume->getCurrentWindow().getWidthReal();
	float markL = (wc - ww / 2 - histogram->getMin()) / (histogram->getMax() - histogram->getMin()) * 2 - 1;
	float markR = (wc + ww / 2 - histogram->getMin()) / (histogram->getMax() - histogram->getMin()) * 2 - 1;
	glUniform2f(clutStripShader.getUniform("x_offsets"), markL, markR);

	glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_SHORT, 0);
}

void Transfer1DRenderer::drawWindowMarkers()
{
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
    colorShader.enable();
    int loc = colorShader.getAttribute("vs_position");
    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(loc, 2, GL_FLOAT, false, stride, 0);
    
	float wc = volume->getCurrentWindow().getCenterReal();
	float ww = volume->getCurrentWindow().getWidthReal();
	float markL = (wc - ww / 2 - histogram->getMin()) / (histogram->getMax() - histogram->getMin());
	float markC = (wc - histogram->getMin()) / (histogram->getMax() - histogram->getMin());
	float markR = (wc + ww / 2 - histogram->getMin()) / (histogram->getMax() - histogram->getMin());

    glUniform4f(colorShader.getUniform("color"), 0.5f, 0.5f, 1.0f, 1.0f);
    glUniform1f(colorShader.getUniform("offset"), markL * 2.0f);
    glDrawArrays(GL_LINES, 6, 2);
    glUniform1f(colorShader.getUniform("offset"), markR * 2.0f);
    glDrawArrays(GL_LINES, 6, 2);
    glUniform1f(colorShader.getUniform("offset"), markC * 2.0f);
    glDrawArrays(GL_LINES, 6, 2);
}

void Transfer1DRenderer::drawCursorValue()
{
    // cursor histogram value
    TextRenderer& text = MainController::getInstance().getText();
    text.setColor(0, 0, 0);
    text.begin(viewport.width, viewport.height);
    
	std::ostringstream os;
	os << std::setprecision(1) << std::fixed;
	os << cursorValue;
	if (volume->getModality() == VolumeData::CT)
		os << " " << hounsfield(cursorValue);
    
    if (cursorX > viewport.width / 2)
        text.add(os.str(), cursorX - 5, cursorY, TextRenderer::RIGHT);
    else
		text.add(os.str(), cursorX + 5, cursorY, TextRenderer::LEFT);
    text.end();
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

void Transfer1DRenderer::setDrawCursor(bool draw)
{
    drawCursor = draw;
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