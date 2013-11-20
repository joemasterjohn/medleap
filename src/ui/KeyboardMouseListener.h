#ifndef medleap_KeyboardMouseListener_h
#define medleap_KeyboardMouseListener_h

#include <GLFW/glfw3.h>

class KeyboardMouseListener
{
public:
    virtual void keyboardInput(GLFWwindow* window, int key, int action, int mods) = 0;
    virtual void mouseButton(GLFWwindow* window, int button, int action, int mods) = 0;
    virtual void mouseMotion(GLFWwindow* window, double x, double y) = 0;
    
    virtual void scroll(GLFWwindow* window, double dx, double dy) = 0;
};

#endif
