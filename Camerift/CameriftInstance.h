/*
File: CameriftInstance.h
Purpose: Header for CameriftInstance. Class manages the flow of the Program
Author(s): Malte Kieﬂling (mkalte666)
*/
#pragma once

#include "../shared/networkcmd.h"
#include "window.h"
#include "../shared/glutil.h"

#include "HMD.h"


class CameriftInstance
{
public:
	CameriftInstance(const char* ip="127.0.0.1", int port=NET_STANDART_PORT);
	~CameriftInstance(void);//Frees memory

	
	//Main functions
	void loop();
	void Render();
	bool Update();

	//We need to init the OpenGL-stuff in a seperate function. 
	void InitGL();

protected:	
	void SendCmd(char cmd);
	void SendData(char* data, int len);
	void RecvData(char* dst, int len);
	void RecvData(int &dst);
	

private:
	//First, basic stuff. Displaying, HMD etc. 
	bool						m_runable;
	bool						m_base_init;
	std::auto_ptr<window>		m_window;
	int							m_width;
	int							m_height;
	const char*					m_ip;
	int							m_port;
	hmdcontroler				m_hmd;

	//Rendering stuff
	float						m_vScale;
	float						m_hScale;
	//Network stuff
	SOCKET						m_clientsocket;

	//Information we need for the transmission, the images etc.
	int							m_numCameras;
	int							m_sizeImage;
	int							m_wImage;
	int							m_hImage;
	float						m_camera_frametime;
	float						m_camera_timer;
	float						m_servo_interval;
	float						m_servo_timer;
	//And the images in raw-format
	char*						m_rawLeft;
	char*						m_rawRight;

	//Textures for the eyes and the Framebuffer-textures. Also the VBOs etc. 
	GLuint						m_vertexArray;
	GLuint						m_fbuffer;
	GLuint 						m_depthbuffer;

	GLuint						m_fbo_texture;
	GLuint						m_left_texture;
	GLuint						m_right_texture;

	GLuint						m_vertexbuffer_left;
	GLuint						m_vertexbuffer_right; 
	GLuint						m_vertexbuffer_scaled_left;
	GLuint						m_vertexbuffer_scaled_right;
	GLuint						m_uvbuffer_left;
	GLuint						m_uvbuffer_right;
	GLuint						m_uvbuffer_full;

	//Shaders and uniformIDs
	GLuint						m_postProcessShader;
	GLuint						m_renderShader;

	GLuint						m_renderShaderTextureId;
	GLuint						m_renderShaderMMatrixId;


	GLuint						m_postProcessShaderTextureId;
	GLuint						m_distortionLenseCenterId;
	GLuint						m_distortionScreenCenterId;
	GLuint						m_distortionScaleId;
	GLuint						m_distortionScaleInId;
	GLuint						m_distortionWarpParamId;

	//Vars for the eye-offsets
	glm::mat4					m_leftModelMatrix;
	glm::mat4					m_rightModelMatrix;
};

