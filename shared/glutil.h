/*
File: glutil.h
Purpose: Header for the OpenGL utility functions. Thanks to opengl-tutorial.org
Author(s): Malte Kie�ling (mkalte666)
*/
#ifndef GLUTIL_HEADER
#define GLUTIL_HEADER

#include "base.h"
extern GLuint CreateTexture(char * data, int width, int height);
extern void SetNewTextureData(GLuint textureID, char * data, int width, int height);
extern GLuint LoadShadersFromData(const char * vertex,const char * fragment);
extern void GenerateFramebuffer(int w, int h, GLuint &dstFboId, GLuint &dstTex, GLuint &dstDepth);
extern GLuint LoadShaders(const char * vertex_file_path, const char * fragment_file_path);
#endif