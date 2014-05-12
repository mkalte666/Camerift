/*
File: base.h
Purpose: Contains basical includes often needed by different systems
Author(s): Malte Kieﬂling (mkalte666)
*/

#ifndef BASE_HEADER
#define BASE_HEADER

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <limits>
#include <assert.h>
#include <memory>
#include <cmath>

#ifdef _WIN32
#include <winsock2.h>
#include <windows.h>
#endif
#ifdef _LINUX
typedef SOCKET int;
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#endif

#ifdef CLIENT_APP

#ifdef _WIN32

#define GLFW_DLL //Needed for linking to Dll!
#endif //WIN32

#include <GL\glew.h>
#include <GL\glfw3.h>
#include <GL\glm\glm.hpp>
#include <GL\glm/gtc/matrix_transform.hpp>
#include <GL\glm/gtx/transform.hpp>



#endif //CLIENT_APP

#ifdef SERVER_APP
#include <opencv2\opencv.hpp>

#endif
#endif
