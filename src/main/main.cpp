#include "gl/glew.h"
#include "MainController.h"
#include "main/MainConfig.h"
#include <iostream>

GLFWwindow* initGL(int width, int height, const char* title)
{
	if (!glfwInit())
		return NULL;

	MainConfig cfg;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwWindowHint(GLFW_RED_BITS, 8);
	glfwWindowHint(GLFW_GREEN_BITS, 8);
	glfwWindowHint(GLFW_BLUE_BITS, 8);
	glfwWindowHint(GLFW_ALPHA_BITS, 8);
	glfwWindowHint(GLFW_DEPTH_BITS, 24);

	if (cfg.getValue<bool>(MainConfig::MULTISAMPLING))
		glfwWindowHint(GLFW_SAMPLES, cfg.getValue<unsigned>(MainConfig::SAMPLES));
	if (cfg.getValue<bool>(MainConfig::USE_SRGB))
		glfwWindowHint(GLFW_SRGB_CAPABLE, GL_TRUE);

	GLFWwindow* window = glfwCreateWindow(width, height, title, NULL, NULL);
	if (!window) {
		glfwTerminate();
		return NULL;
	}
	glfwMakeContextCurrent(window);

	if (cfg.getValue<bool>(MainConfig::USE_SRGB))
		glEnable(GL_FRAMEBUFFER_SRGB);

	// Set vertical retrace rate (0 == run as fast as possible)
	glfwSwapInterval(1);

	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (err != GLEW_OK) {
		glfwTerminate();
		return NULL;
	}

	return window;
}

int main(int argc, char** argv)
{
	GLFWwindow* window = initGL(800, 600, "MedLeap");
	if (!window) {
		std::cout << "Couldn't initialize OpenGL" << std::endl;
		return 1;
	}

    MainController& controller = MainController::getInstance();
    controller.init(window);
    
    if (argc > 1) {
		VolumeLoader::Source src = { argv[1], VolumeLoader::Source::DICOM_DIR };
        controller.setVolumeToLoad(src);
    }
    
    controller.startLoop();
    
    return 0;
}