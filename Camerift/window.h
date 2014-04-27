/*
File: window.h	
Purpose: Header for the Window-class. 
Author(s): Malte Kieﬂling (mkalte666)
*/
#pragma once

#include "../shared/base.h"

class window
{
public:
	window(void);
	window(int w, int h, const char* title = "OwlEngine");
	~window(void);

	void					Delete();

	void					SetWindowTitle(const char* title);
	const char*				GetWindowTitle(void);

	void					SwapBuffers(void);
	float					GetAspectRatio(void);
	bool					GetIsAlive(void);
	void					GetCursor(double &x, double &y);
	void					SetCursor(double x, double y);
	bool					GetMouseButtonState(int button);
	bool					GetKeyState(int key);

	void					CreateTexture(GLuint &id, int w, int h);
private:
	GLFWwindow*				m_window;
	int						m_width;
	int						m_height;
	const char*				m_title;
	bool					m_workingWindow;
};