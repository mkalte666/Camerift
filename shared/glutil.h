/*
File: glutil.h
Purpose: Header for the OpenGL utility functions. Thanks to opengl-tutorial.org
Author(s): Malte Kieﬂling (mkalte666)
*/
#ifndef GLUTIL_HEADER
#define GLUTIL_HEADER

#include "base.h"
extern GLuint CreateTexture(char * data, int width, int height);
extern void SetNewTextureData(GLuint textureID, char * data, int width, int height);
extern GLuint LoadShadersFromData(const char * vertex,const char * fragment);
#endif