#ifndef GLUTIL_HEADER
#define GLUTIL_HEADER

#include "base.h"
extern GLuint CreateTexture(char * data, int width, int height);
extern GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path);
extern void SetNewTextureData(GLuint textureID, char * data, int width, int height);
extern GLuint LoadShadersFromData(const char * vertex,const char * fragment);
#endif