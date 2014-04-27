/*
File: CameriftInstance.h
Purpose: Contains the main functions of the Camerift Instance class, wich manages the flow of the Program
Author(s): Malte Kieﬂling (mkalte666)
*/
#include "CameriftInstance.h"
#include "shaders.h"
#include "../shared/base.h"

CameriftInstance::CameriftInstance(const char* ip, int port) : m_hmd()
{
	m_runable = false;
	m_base_init = false;
	std::cout <<	"Camerift. (c) Malte Kieﬂling 2013-2014\n" <<
					"View and see your Webcams with the Oculus Rift.\n" <<
					"Additional parameters are <camerift> [ip] [port]!\n" << 
					"Starting Initialisation!\n";

	std::cout << "Init HMD...\n";
	
	if(!m_hmd.GetValid()) {
		std::cerr << "Could not Create HMD! Will launch with 1280*800 and no distortion!\n";
		m_width = 1920;
		m_height = 1080;
	}
	else {
		std::cout << "Done!\n Creating Window with the dimensions og the HMD...\n";
		m_width = m_hmd.GetW();
		m_height = m_hmd.GetH();
	}
	{
		//Needs to be done from the Main thread. so...
		/*m_window = window(m_width, m_height);
		if(!m_window.GetIsAlive()) {
			std::cerr <<  "Could not create Window!!\n";
		}
		else*/ {
			std::cout << "Done!\n Creating Socket for communication...\n";
			m_clientsocket = socket(AF_INET, SOCK_STREAM, 0); //TCP, or we couldn send the amount of data we need
			if(	m_clientsocket == INVALID_SOCKET ) {
				std::cerr << "Could not create socket!\n";
			} else {
				std::cout << "Done!\n Connecting to server...\n";
				//We need a structure to connect to the server that contains the address.
				SOCKADDR_IN server_addr;
				server_addr.sin_addr.S_un.S_addr = inet_addr(ip);
				server_addr.sin_port = htons(port);
				server_addr.sin_family = AF_INET;
				//And we connect. If possible. Else... not
				int ret_connect = connect(m_clientsocket, (SOCKADDR*)&server_addr, sizeof(SOCKADDR));
				if(ret_connect == -1) {
					std::cerr << "Error conecting to server. Is the server active and your address right?\n";
				} else {
					std::cout << "Done\n Now reciving basic information from server...\n";

					//We need the size (in bytes) every image has
					SendCmd(NET_CMD_GET_IMAGESIZE);
					RecvData(m_sizeImage);
					
					//And, of course, the dimensions of the Images
					SendCmd(NET_CMD_GET_IMAGESIZE_WIDTH);
					RecvData(m_wImage);
					SendCmd(NET_CMD_GET_IMAGESIZE_HEIGHT);
					RecvData(m_hImage);
					//And we need to know how many cameras are connected to render the rigth image (3d etc)
					SendCmd(NET_CMD_GET_CAMERA_NUM);
					RecvData(m_numCameras);

					//And reserv some memory for our images.
					m_rawLeft = new char[m_sizeImage];
					m_rawRight = new char[m_sizeImage];
					std::cout << "Done!\n";

					

					
					//The rest of the init is In InitGL(), what MUST be called from the main thread!
					//But also the main-part of the Initialisatioin is compleate, so
					m_base_init = true;
				}
			}
		}
	}

	//Check if we can start etc. For debugging, haveing a nice UI etc.
	if(!m_base_init) {
		std::cerr << "Error in Initialisation!\n";	
	}
	else {
		std::cout << "Init succsessfull!\n";
	}
	//End of Constructor.
}



CameriftInstance::~CameriftInstance(void)
{
	SendCmd(NET_CMD_SET_SERVERSTOP);
	closesocket(m_clientsocket);
	//We need to free a lot of stuff, beginnig with the Textures, ending with other buffers...
	delete m_rawRight;
	delete m_rawLeft;
	glDeleteBuffers(1, &m_vertexbuffer_left);
	glDeleteBuffers(1, &m_vertexbuffer_right);
	glDeleteBuffers(1, &m_uvbuffer_left);
	glDeleteBuffers(1, &m_uvbuffer_right);
	glDeleteBuffers(1, &m_uvbuffer_full);
	glDeleteProgram(m_postProcessShader);
	glDeleteProgram(m_renderShader);
	glDeleteFramebuffers(1, &m_fbuffer);
	glDeleteRenderbuffers(1, &m_depthbuffer);
	glDeleteTextures(1, &m_fbo_texture);
	glDeleteTextures(1, &m_left_texture);
	glDeleteTextures(1, &m_right_texture);
	glDeleteVertexArrays(1, &m_vertexArray);
	m_window.release();
}

void CameriftInstance::SendCmd(char cmd) 
{
	//if(m_runable==false) return;
	char data[NET_CMD_BUFFER_LENGTH];
	data[0] = cmd;
	send(m_clientsocket, data, NET_CMD_BUFFER_LENGTH, 0);
}

void CameriftInstance::SendData(char* data, int len)
{
	send(m_clientsocket, data, len, 0);
}

void CameriftInstance::RecvData(char* dst, int len) 
{
	//if(m_runable==false) return;
	int data_recv = recv(m_clientsocket, dst, len, 0);
	if(data_recv=!len) {
		std::cout << "WARNING: NO MATCH IN RECV (network)!!! This may cause problems...\n";
	}
}

void CameriftInstance::RecvData(int &dst) 
{
	//if(m_runable==false) return;
	
	char data[NET_CMD_SIZE_LENGTH];
	recv(m_clientsocket, data, NET_CMD_SIZE_LENGTH, 0);
	dst = *(int*)(&data[0]); //Magic: The char has the size of an int, so we take its adress and dereference it. That is or value!
}

void CameriftInstance::loop() 
{
	if(m_runable==false) return;

	do {
		//This is the Main Loop. The order is easy: Update HMD, Send Data Rotation, Get new images, render, update local stuff
		//We start with the HMD
		m_hmd.Update();
	
		//Next is to send the Data to the server
		//TODO

		if((glfwGetTime() - m_camera_timer) >= m_camera_frametime) {
			//Now get the new Images
			
			SendCmd(NET_CMD_GET_IMAGE_LEFT);
			RecvData(m_rawLeft, m_sizeImage);
			SendCmd(NET_CMD_GET_IMAGE_RIGHT);
			RecvData(m_rawRight, m_sizeImage);
			//And set them as the new Textures
			SetNewTextureData(m_left_texture, m_rawLeft, m_wImage, m_hImage);
			SetNewTextureData(m_right_texture, m_rawRight, m_wImage, m_hImage);
			m_camera_timer = glfwGetTime();
		}
		//And now comes the Render! (see the function)
		Render();

		//Time for local Updates
	} while( Update());
}

bool CameriftInstance::Update()
{
	if(m_runable==false) return false;
	bool ret = true;
	glfwPollEvents();
	
	ret = (ret && m_window->GetIsAlive());
	//Todo: More Updates here
	//When we press LEFT, we move the images more to the middle, when we press RIGHT, farer away
	if(m_window->GetKeyState(GLFW_KEY_A)) {
		//we move the left image to the left etc
		m_leftModelMatrix = m_leftModelMatrix * glm::translate(glm::vec3(-0.005f, 0.0f, 0.0f));
		m_rightModelMatrix = m_rightModelMatrix * glm::translate(glm::vec3(0.005f, 0.0f, 0.0f));
	}
	if(m_window->GetKeyState(GLFW_KEY_D)) {
		//we move the left image to the left etc
		m_leftModelMatrix = m_leftModelMatrix * glm::translate(glm::vec3(0.005f, 0.0f, 0.0f));
		m_rightModelMatrix = m_rightModelMatrix * glm::translate(glm::vec3(-0.005f, 0.0f, 0.0f));
	}

	//Also we need to send the rotation to the server. So lets take a float-array
	float *rotdata = new float[3];
	//And fill it with the rotation
	rotdata[0] = m_hmd.GetXAngle();
	rotdata[1] = m_hmd.GetYAngle();
	rotdata[2] = m_hmd.GetZAngle();
	SendCmd(NET_CMD_SET_ROT);
	SendData((char*)rotdata, NET_CMD_ROT_DATA);

	return ret;
}

void CameriftInstance::Render()
{
	
	if(m_runable==false) return ;
	//At first we Render the Camera-images on our Framebuffer.
	//And there are things we need to do. First we set our viewport to Full
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbuffer);
	glViewport(0,0,m_width, m_height);
	//and we clear it
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//We render the normal way, means that we need our render shader
	glUseProgram(m_renderShader);

	//LEFT IMAGE
	//We use tex0, with the data of our left image
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_left_texture);
	glUniform1i(m_renderShaderTextureId, 0);
	//The Model-matrix
	glUniformMatrix4fv(m_renderShaderMMatrixId, 1, GL_FALSE, &m_leftModelMatrix[0][0]);
	//Activate our Vertex- and UV buffer
	//Vertex
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexbuffer_scaled_left);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	//UV
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, m_uvbuffer_full);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0); //See the difference to above. i use layout-pos 1 insted of 0, and i have a vec2
	//Aand draw Left
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

	//RIGHT IMAGE 
	//We use tex0, with the data of our right image
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_right_texture);
	glUniform1i(m_renderShaderTextureId, 0);
	//The Model-matrix
	glUniformMatrix4fv(m_renderShaderMMatrixId, 1, GL_FALSE, &m_rightModelMatrix[0][0]);
	//Activate our Vertex- and UV buffer
	//Vertex
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexbuffer_scaled_right);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	//UV
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, m_uvbuffer_full);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0); //See the difference to above. i use layout-pos 1 insted of 0, and i have a vec2
	//Aand draw right
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);



	//Ok, that is done. Our renderd image is now in m_fbo_texture. Lets render it again
	//To remove the lense distortion :)
	//Fist we need our screen as Viewport
	glBindFramebuffer(GL_FRAMEBUFFER, 0); //the 0 references to our screen
	glViewport(0,0,m_width,m_height);
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//And our dist. shader
	glUseProgram(m_postProcessShader);

	//And bind the FBO texture
	glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_fbo_texture);
	glUniform1i(m_postProcessShaderTextureId, 0); 

	//LEFT SIDE
	//Set data for left side
	glUniform2f(m_distortionLenseCenterId, m_hmd.GetLenseCenter(EYE_LEFT).x, m_hmd.GetLenseCenter(EYE_LEFT).y);
	glUniform2f(m_distortionScreenCenterId, m_hmd.GetScreenCenter(EYE_LEFT).x, m_hmd.GetScreenCenter(EYE_LEFT).y);
	glUniform2f(m_distortionScaleId, m_hmd.GetScale(EYE_LEFT).x, m_hmd.GetScale(EYE_LEFT).y);
	glUniform2f(m_distortionScaleInId, m_hmd.GetScaleIn(EYE_LEFT).x, m_hmd.GetScaleIn(EYE_LEFT).y);
	glUniform4f(m_distortionWarpParamId, m_hmd.GetDistortion(EYE_LEFT)[0], m_hmd.GetDistortion(EYE_LEFT)[1], m_hmd.GetDistortion(EYE_LEFT)[2], m_hmd.GetDistortion(EYE_LEFT)[3]);

	//Render 
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexbuffer_left);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, m_uvbuffer_left);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

	//RIGHT SIDE
	//Set data for Right side
	glUniform2f(m_distortionLenseCenterId, m_hmd.GetLenseCenter(EYE_RIGHT).x, m_hmd.GetLenseCenter(EYE_RIGHT).y);
	glUniform2f(m_distortionScreenCenterId, m_hmd.GetScreenCenter(EYE_RIGHT).x, m_hmd.GetScreenCenter(EYE_RIGHT).y);
	glUniform2f(m_distortionScaleId, m_hmd.GetScale(EYE_RIGHT).x, m_hmd.GetScale(EYE_RIGHT).y);
	glUniform2f(m_distortionScaleInId, m_hmd.GetScaleIn(EYE_RIGHT).x, m_hmd.GetScaleIn(EYE_RIGHT).y);
	glUniform4f(m_distortionWarpParamId, m_hmd.GetDistortion(EYE_RIGHT)[0], m_hmd.GetDistortion(EYE_RIGHT)[1], m_hmd.GetDistortion(EYE_RIGHT)[2], m_hmd.GetDistortion(EYE_RIGHT)[3]);

	//Aand render
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexbuffer_right);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, m_uvbuffer_right);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

	m_window->SwapBuffers();

	int errorcode = glGetError();
	if(errorcode!= GL_NO_ERROR) {
		std::cout << "Oh, something went wrong (In render loop): ";
		std::cout << std::hex << errorcode;
		std::cout << "\n";
	}
	//That was the Render :)
}

void CameriftInstance::InitGL()
{
	if(!m_base_init) return;
	m_window.reset(new window(m_width, m_height, "Camerift"));
	
	//Now some more OpenGL stuff: Wee need a VerexArray-id, the framebuffer, the textures, ...
	//Vertex Array
	std::cout << "Init some more OpenGL stuff...  \n";
	glGenVertexArrays(1, &m_vertexArray);
	glBindVertexArray(m_vertexArray);
	//And the  textures for the eyes and the FBO
	m_window->CreateTexture(m_left_texture, m_wImage, m_hImage);
	m_window->CreateTexture(m_right_texture, m_wImage, m_hImage);
	m_window->CreateTexture(m_fbo_texture, m_width, m_height);
	//Next is the FrameBuffer and it's texture
	glGenFramebuffers(1, &m_fbuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbuffer);
	//We need a depthbuffer for our framebuffer.
	glGenRenderbuffers(1, &m_depthbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, m_depthbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_width, m_height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthbuffer);
					
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_fbo_texture, 0);
					
	//Some more Framebuffer stuff
	GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
	glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers
					
	m_vScale = 40/m_hmd.GetVerticalFOV();
	m_hScale = 60/m_hmd.GetHorizontalFOV();
	//So we have textures, we need something to render on. Oh, and buffers for The Texture-coords, too.
	//And because we render with GL_QUADS, we can simply use quads :)
	//Left Eye/side/what ever
	static const GLfloat left_quad[] = { 
		-1.0f, -1.0f, 0.0f,
         0.0f, -1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,
		 0.0f, -1.0f, 0.0f,
         0.0f,  1.0f, 0.0f,

	};

	static const GLfloat left_quad_scaled[] = { 
		 0-m_hScale, 0-m_vScale, 0.0f,
         m_hScale-1, 0-m_vScale, 0.0f,
        0-m_hScale,  m_vScale, 0.0f,
        0-m_hScale,  m_vScale, 0.0f,
		m_hScale-1, 0-m_vScale, 0.0f,
        m_hScale-1,  m_vScale, 0.0f,
	};
	glGenBuffers(1, &m_vertexbuffer_left);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexbuffer_left);
	glBufferData(GL_ARRAY_BUFFER, sizeof(left_quad), left_quad, GL_STATIC_DRAW);
	glGenBuffers(1, &m_vertexbuffer_scaled_left);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexbuffer_scaled_left);
	glBufferData(GL_ARRAY_BUFFER, sizeof(left_quad_scaled), left_quad_scaled, GL_STATIC_DRAW);

	//Right side
	static const GLfloat right_quad[] = { 
		0.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        0.0f,  1.0f, 0.0f,
        0.0f,  1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
        1.0f,  1.0f, 0.0f,
	};
	static const GLfloat right_quad_scaled[] = { 
		1-m_hScale, -0-m_vScale, 0.0f,
        m_hScale, 0-m_vScale, 0.0f,
        1-m_hScale,  m_vScale, 0.0f,
        1-m_hScale,  m_vScale, 0.0f,
		0+m_hScale, 0-m_vScale, 0.0f,
        0+m_hScale,  m_vScale, 0.0f,
	};

	glGenBuffers(1, &m_vertexbuffer_right);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexbuffer_right);
	glBufferData(GL_ARRAY_BUFFER, sizeof(right_quad), right_quad, GL_STATIC_DRAW);
	glGenBuffers(1, &m_vertexbuffer_scaled_right);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexbuffer_scaled_right);
	glBufferData(GL_ARRAY_BUFFER, sizeof(right_quad_scaled), right_quad_scaled, GL_STATIC_DRAW);
	//Uv map for left
	static const GLfloat left_uv[] = {
		0.0f, 0.0f,
		0.5f, 0.0f,
		0.0f, 1.0f,
		0.0f, 1.0f,
		0.5f, 0.0f, 
		0.5f, 1.0f,

	};
	glGenBuffers(1, &m_uvbuffer_left);
	glBindBuffer(GL_ARRAY_BUFFER, m_uvbuffer_left);
	glBufferData(GL_ARRAY_BUFFER, sizeof(left_uv), left_uv, GL_STATIC_DRAW);
	//And left
	static const GLfloat right_uv[] = {
		0.5f, 0.0f,
		1.0f, 0.0f,
		0.5f, 1.0f,
		0.5f, 1.0f,
		1.0f, 0.0f, 
		1.0f, 1.0f,

	};
	glGenBuffers(1, &m_uvbuffer_right);
	glBindBuffer(GL_ARRAY_BUFFER, m_uvbuffer_right);
	glBufferData(GL_ARRAY_BUFFER, sizeof(right_uv), right_uv, GL_STATIC_DRAW);

	static const GLfloat full_uv[] = {
		0.0f, 1.0f,
        1.0f, 1.0f, 
        0.0f,  0.0f, 
        0.0f,  0.0f, 
        1.0f, 1.0f, 
        1.0f,  0.0f, 

	};
	glGenBuffers(1, &m_uvbuffer_full);
	glBindBuffer(GL_ARRAY_BUFFER, m_uvbuffer_full);
	glBufferData(GL_ARRAY_BUFFER, sizeof(full_uv), full_uv, GL_STATIC_DRAW);

	//Last thing is that we have to load the Shaders and its Identifiers. 
	//Fist we create the Programs
	m_renderShader = LoadShadersFromData(g_vertex_shader, g_fragment_shader);
	m_postProcessShader = LoadShadersFromData(g_vertex_shader_fbo, g_fragment_shader_fbo);
	//Next is that we need identifiers to our members in the shader. Means to the Texture-samplers, Colors, ...
	//Fist for the RenderShader - only the texture
	m_renderShaderTextureId = glGetUniformLocation(m_renderShader, "texture");
	m_renderShaderMMatrixId = glGetUniformLocation(m_renderShader, "M");

	//And some more stuff for the Post processing (texture and Parameters for the PostProcessing)
	m_postProcessShaderTextureId	= glGetUniformLocation(m_postProcessShader, "texture");
	m_distortionLenseCenterId		= glGetUniformLocation(m_postProcessShader, "LenseCenter");
	m_distortionScreenCenterId		= glGetUniformLocation(m_postProcessShader, "ScreenCenter");
	m_distortionScaleId				= glGetUniformLocation(m_postProcessShader, "Scale");
	m_distortionScaleInId			= glGetUniformLocation(m_postProcessShader, "ScaleIn");
	m_distortionWarpParamId			= glGetUniformLocation(m_postProcessShader, "HmdWarpParam");

	int errorcode = glGetError();
	if(errorcode!= GL_NO_ERROR) {
		std::cout << "Oh, something went wrong (In GL init): ";
		std::cout << std::hex << errorcode;
		std::cout << "\n";
	}

	//We have to move the images together so they fit the IPD of the user. The ipd is given in meters, the hscreensize too. 
	//We can calculate ipd/vscreensize  to get the 0-1 offset we need
	float eyeOffset = (m_hmd.GetIpd()/m_hmd.GetHScreenSize());
	m_leftModelMatrix = m_leftModelMatrix * glm::translate(glm::vec3(-(eyeOffset-0.5), 0.0f, 0.0f));
	m_rightModelMatrix = m_rightModelMatrix * glm::translate(glm::vec3((eyeOffset-0.5), 0.0f, 0.0f));





	//We need to know the rate the Camras Update. To achive this, we will get 5 frames for each side and measure the time it needs. 
					//The Frames will Only be updated when needed (So every N milliseconds. with that we secure a stable and high framerate.
					std::cout << "Testing Framerate. Please Wait...\n";
					double time_raw = (float)glfwGetTime();
					double dt = 0;
					for ( int i = 0; i < 5; i++) {
						SendCmd(NET_CMD_GET_IMAGE_LEFT);
						RecvData(m_rawLeft, m_sizeImage);
						SendCmd(NET_CMD_GET_IMAGE_RIGHT);
						RecvData(m_rawRight, m_sizeImage);
						if(dt <  (glfwGetTime() - time_raw)) dt= glfwGetTime() - time_raw;
						time_raw = glfwGetTime();
					}
					m_camera_frametime = dt;




	m_camera_timer = glfwGetTime();
	//Test if the Framebuffer etc. is ok, and if yes, we are ready to start!
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << "Error in OpenGL creation. Make shoure your GPU supports 3.3!\n Framebuffer(!)\n";
		std::cin;
	} else {
		// we can start now!
		std::cout << "Done!\n";
		m_runable=true;
	}
}