/*
File: window.cpp
Purpose: cpp file for window-class. The Window-class handles the initialisation of OpenGL/Glfw and the window-management
Author(s): Malte Kieﬂling (mkalte666)
*/
#include "window.h"

/*
* @name: window
* @param: void
* @return: void
* @description: Constructor for Window. Opens Window with basic resolution and standart-title. Also Starts OpenGL and inits glu and sets context
*/
window::window(void) : m_width(640), m_height(480), m_title("Camerift")
{
	m_workingWindow = false;

	int worked = glfwInit();
	if(worked == GL_FALSE) {
		std::cout << "Error in Initialisation ( glfwInit() ) \n";
		return;
	}

	//OpenGL profile and some Basiacal settings
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//Creating window and context
	m_window = glfwCreateWindow(m_width, m_height, m_title, NULL, NULL); //Window
	glfwMakeContextCurrent(m_window); //Context
	//Init Glew
	glewExperimental = GL_TRUE;
	GLenum err = glewInit(); 
	if(err != GLEW_OK) {
		std::cout << "Error in Initialisation ( glewInit() ): " << glewGetErrorString(err) << "\n";
	}
	else {
		glfwSetInputMode(m_window, GLFW_STICKY_KEYS, GL_FALSE);
		glfwSetInputMode(m_window, GLFW_STICKY_MOUSE_BUTTONS, GL_FALSE);
		glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		m_workingWindow = true;
	}
}

/*
* @name: window
* @param:	int w: width of the window
			int h: height of the window
			const char* title: the title of the window
* @return: void
* @description: Constructor for Window. Opens Window with given resulution and title and some basic settings. Also Starts OpenGL and inits glu and sets context
*/
window::window(int w, int h, const char* title) : m_width(w), m_height(h), m_title(title)
{
	m_workingWindow = false;

	int worked = glfwInit();
	if(worked == GL_FALSE) {
		std::cout << "Error in Initialisation ( glfwInit() ) \n";
		return;
	}

	//OpenGL profile and some Basiacal settings
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//Creating window and context
	m_window = glfwCreateWindow(m_width, m_height, m_title, NULL /*glfwGetPrimaryMonitor()*/, NULL); //Window
	glfwMakeContextCurrent(m_window); //Context
	//Init Glew
	glewExperimental = GL_TRUE;
	GLenum err = glewInit(); 
	glFrontFace(GL_CCW);

	int errorcode = glGetError();
	if(errorcode!= GL_NO_ERROR) {
		std::cout << "Oh, something went wrong (In Window creation): ";
		std::cout << std::hex << errorcode;
		std::cout << "\n";
	}


	if(err != GLEW_OK) {
		std::cout << "Error in Initialisation ( glewInit() ): " << glewGetErrorString(err) << "\n";
	}
	
	else {
		glfwSetInputMode(m_window, GLFW_STICKY_KEYS, GL_FALSE);
		glfwSetInputMode(m_window, GLFW_STICKY_MOUSE_BUTTONS, GL_FALSE);
		glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		m_workingWindow = true;
	}
}

/*
* @name: ~window
* @param: void
* @return: void
* @description: Frees  allocated memory and also stops glf and glew
*/
window::~window(void)
{
	glfwDestroyWindow(m_window);
	glfwTerminate();
}

void window::Delete() 
{
	
}
/*
* @name: SetWindowTitle
* @param: const char* title: The new title for the window
* @return: void
* @description: Set a new title to the active window
*/
void window::SetWindowTitle(const char* title)
{
	m_title = title;
	glfwSetWindowTitle(m_window, m_title);
}

/*
* @name: GetWindowTitle
* @param: void
* @return: const char*: the title the Window has at the moment
* @description: Returns the Title set to the window via constructor or by the SetWindowTitle-function.
*/
const char* window::GetWindowTitle(void)
{
	return m_title;
}

/*
* @name: SwapBuffers
* @param: void
* @return: void
* @description: Helper-function for glfwSwapBuffers, so that no direct use of m_window is needet
*/
void window::SwapBuffers(void)
{
	glfwSwapBuffers(m_window);
}

/*
* @name: GetIsAlive
* @param: void
* @return: bool: is window alive?
* @description: Function takes infos set by construtor. True if window is alive and context is created etc, false if anything wierd happend
*/
bool window::GetIsAlive(void)
{
	 return m_workingWindow && !GetKeyState(GLFW_KEY_ESCAPE);
}

void window::SetCursor(double x, double y)
{
	x*=m_width;
	y*=m_height;
	glfwSetCursorPos(m_window, x,y);
}

void window::GetCursor(double &x, double &y)
{
	glfwGetCursorPos(m_window, &x, &y);
	x/=m_width;
	y/=m_height;
}

bool window::GetMouseButtonState(int button)
{
	if (glfwGetMouseButton(m_window, button)==GLFW_PRESS) 
		return true;
	return false;
}

void window::CreateTexture(GLuint &id, int w, int h) 
{
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);

	glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, w, h, 0, GL_BGR, GL_UNSIGNED_BYTE, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);
}

bool window::GetKeyState(int key)
{
	return (glfwGetKey(m_window, key)==GLFW_PRESS);
}