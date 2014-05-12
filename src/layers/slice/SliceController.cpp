#include "SliceController.h"
#include "main/MainController.h"

using namespace gl;
using namespace Leap;
using namespace std::chrono;

SliceController::SliceController() :
	leap_scroll_dst_(5.0f)
{
    mouseLeftDrag = false;

	sliceShader = Program::create("shaders/slice_clut.vert", "shaders/slice_clut.frag");
	sliceShader.enable();
	glUniform1i(sliceShader.getUniform("tex_slice"), 0);
	glUniform1i(sliceShader.getUniform("tex_clut"), 1);

	sliceTexture.generate(GL_TEXTURE_2D);

	// geometry is simply a textured quad
	// uniform matrix will scale to correct aspect ratio
	GLfloat vertexData[] = {
		-1, -1, 0, 0,
		1, -1, 1, 0,
		1, 1, 1, 1,
		-1, -1, 0, 0,
		1, 1, 1, 1,
		-1, 1, 0, 1
	};
	sliceVBO.generateVBO(GL_STATIC_DRAW);
	sliceVBO.bind();
	sliceVBO.data(vertexData, sizeof(vertexData));

	point_pose_.engageSpeedThreshold(100);
	point_pose_.engageDelay(milliseconds(0));
	point_pose_.disengageDelay(milliseconds(0));
	point_pose_.trackFunction(std::bind(&SliceController::leapScroll, this, std::placeholders::_1));
	point_pose_.engageFunction([&](const Leap::Controller& c){
		saved_slice_ = currentSlice_;
		MainController::getInstance().leapStateController().active(LeapStateController::icon_h2f1_point);
	});
	point_pose_.disengageFunction([](const Leap::Controller&){
		MainController::getInstance().leapStateController().active(LeapStateController::icon_none);
	});
}

void SliceController::gainFocus()
{
	MainController::getInstance().setMode(MainController::MODE_2D);
	auto& lsc = MainController::getInstance().leapStateController();
	lsc.clear();
	lsc.add(LeapStateController::icon_h1f1_circle, "Main Menu");
	lsc.add(LeapStateController::icon_h2f1_point, "Scroll Slices");
}

void SliceController::slice(int index)
{
	if (index < 0) {
		index = volume->getDepth() - 1 - (-index - 1) % volume->getDepth();
	} else {
		index = index % volume->getDepth();
	}

	currentSlice_ = index;
	updateTexture();
}

bool SliceController::keyboardInput(GLFWwindow* window, int key, int action, int mods)
{
    if (key == GLFW_KEY_UP && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
		if (++currentSlice_ == volume->getDepth())
			currentSlice_ = 0;
		updateTexture();
    } else if (key == GLFW_KEY_DOWN && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
		if (--currentSlice_ < 0)
			currentSlice_ = volume->getDepth() - 1;
		updateTexture();
    }
    
    return true;
}

bool SliceController::mouseButton(GLFWwindow* window, int button, int action, int mods, double x, double y)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            mouseLeftDrag = true;
            glfwGetCursorPos(window, &mouseAnchorX, &mouseAnchorY);
			anchorSliceIndex = currentSlice_;
        } else if (action == GLFW_RELEASE) {
            mouseLeftDrag = false;
        }
    }
    
    return true;
}

bool SliceController::mouseMotion(GLFWwindow* window, double x, double y)
{
    if (!viewport_.contains(x,y))
        return true;
    
    // move through slices by dragging left and right
    if (mouseLeftDrag) {
        double dx = x - mouseAnchorX;
        int slice = static_cast<int>(anchorSliceIndex + dx * 0.1);
        if (slice < 0)
            slice = volume->getDepth() - ((-1*slice) % volume->getDepth());
        else
            slice = slice % volume->getDepth();
        
		currentSlice_ = slice;
		updateTexture();
    }
    
    return true;
}


void SliceController::setCLUTTexture(gl::Texture& texture)
{
	this->clutTexture = texture;
}

void SliceController::updateTexture()
{
	sliceTexture.bind();
	sliceTexture.setParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	sliceTexture.setParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	GLenum internalFormat;
	switch (volume->getType()) {
	case GL_UNSIGNED_BYTE: internalFormat = GL_R8; break;
	case GL_UNSIGNED_SHORT: internalFormat = GL_R16; break;
	case GL_BYTE: internalFormat = GL_R8_SNORM; break;
	case GL_SHORT: internalFormat = GL_R16_SNORM; break;
	default: internalFormat = GL_RED; break;
	}

	sliceTexture.setData2D(0,
		internalFormat,
		volume->getWidth(),
		volume->getHeight(),
		volume->getFormat(),
		volume->getType(),
		volume->getData() + currentSlice_ * volume->getSliceSizeBytes());
}

void SliceController::setVolume(VolumeData* volume)
{
	this->volume = volume;
	currentSlice_ = 0;
	updateTexture();
	resize();
}

void SliceController::resize()
{
	// model matrix will scale to keep the displayed image in proportion to its
	// intended dimensions without changing the input vertices in NDC
	float windowAspect = viewport_.aspect();
	float sliceAspect = (float)volume->getWidth() / volume->getHeight();

	modelMatrix = (sliceAspect <= 1.0f) ?
		scale(sliceAspect / windowAspect, 1.0f, 1.0f) :
		scale(1.0f, windowAspect / sliceAspect, 1.0f);
}

void SliceController::draw()
{
	sliceShader.enable();

	glActiveTexture(GL_TEXTURE1);
	clutTexture.bind();
	glActiveTexture(GL_TEXTURE0);
	sliceTexture.bind();

	// set the uniforms
	glUniform1i(sliceShader.getUniform("signed_normalized"), volume->isSigned());
	glUniform1f(sliceShader.getUniform("window_min"), volume->visible().left());
	glUniform1f(sliceShader.getUniform("window_multiplier"), 1.0f / volume->visible().width());
	glUniformMatrix4fv(sliceShader.getUniform("model"), 1, false, modelMatrix);

	// set state and shader for drawing medical stuff
	GLsizei stride = 4 * sizeof(GLfloat);
	sliceVBO.bind();

	int loc = sliceShader.getAttribute("vs_position");
	glEnableVertexAttribArray(loc);
	glVertexAttribPointer(loc, 2, GL_FLOAT, false, stride, 0);

	loc = sliceShader.getAttribute("vs_texcoord");
	glEnableVertexAttribArray(loc);
	glVertexAttribPointer(loc, 2, GL_FLOAT, false, stride, (GLvoid*)(2 * sizeof(GLfloat)));

	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void SliceController::update(std::chrono::milliseconds elapsed)
{
	elapsed_ = elapsed;
}

bool SliceController::leapInput(const Leap::Controller& leapController, const Leap::Frame& currentFrame)
{
	point_pose_.update(leapController);

	return false;
}

void SliceController::leapScroll(const Leap::Controller& controller)
{
	Vector delta = point_pose_.posDelta();
	slice(saved_slice_ + delta.x / leap_scroll_dst_);
}

void SliceController::loseFocus()
{
	point_pose_.tracking(false);
}