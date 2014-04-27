/*
File: shaders.h
Purpose: Contains the Shaders for Camerift
Author(s): Malte Kieﬂling (mkalte666)
*/
#ifndef SHADER_DATA_HEADER
#define SHADER_DATA_HEADER

const char* g_vertex_shader = {
	"#version 330 core\n"

	"// Input vertex data, different for all executions of this shader.\n"
	"layout(location = 0) in vec3 vertexPosition_modelspace;\n"
	"layout(location = 1) in vec2 texUV;\n"
	"// Output data ; will be interpolated for each fragment.\n"

	"uniform mat4 M;\n"
	"out vec2 UV;\n"

	"void main(){\n"
		"gl_Position = M*vec4(vertexPosition_modelspace,1);\n"
		"UV = texUV;\n"
	"}\n"
};

const char* g_vertex_shader_fbo = {
	"#version 330 core\n"

	"// Input vertex data, different for all executions of this shader.\n"
	"layout(location = 0) in vec3 vertexPosition_modelspace;\n"
	"layout(location = 1) in vec2 texUV;\n"
	"// Output data ; will be interpolated for each fragment.\n"
	"out vec2 UV;\n"

	"void main(){\n"
		"gl_Position = vec4(vertexPosition_modelspace,1);\n"
		"UV = texUV;\n"
	"}\n"
};

//The following shader is partly copied from https://developer.oculusvr.com/forums/viewtopic.php?t=4249. Thanks to the guys over there!
const char* g_fragment_shader_fbo = {
	"#version 330 core\n"
	
	"//from Vertex shader\n"
	"in vec2 UV;\n"
	"//Output data\n"
	"out vec3 color;\n"
	
	"//Constant mesh data\n"
	"uniform sampler2D texture;\n"
	"uniform vec2 LenseCenter;\n"
	"uniform vec2 ScreenCenter;\n"
	"uniform vec2 Scale;\n"
	"uniform vec2 ScaleIn;\n"
	"uniform vec4 HmdWarpParam;\n"

	"vec2 HmdWarp(vec2 in01) {\n"
	"	vec2 theta = (in01 - LenseCenter) * ScaleIn; //Scales to -1, 1\n"
	"	float rSq = theta.x*theta.x + theta.y*theta.y;\n"
	"	vec2 rvector = theta* (HmdWarpParam.x + HmdWarpParam.y*rSq + HmdWarpParam.z*rSq*rSq+HmdWarpParam.w*rSq*rSq*rSq);\n"
	"	return LenseCenter+Scale*rvector;\n"
	"}\n"

	"void main() {\n"
	"vec2 tc = HmdWarp(UV);\n"
	"if (!all(equal(clamp(tc, ScreenCenter-vec2(0.25,0.5), ScreenCenter+vec2(0.25,0.5)), tc)))\n"
	"	color = vec3(0);\n"
	"else\n"
	"	color = texture2D(texture, tc).rgb;\n"
	"}\n"
};

const char* g_fragment_shader = {
	"#version 330 core\n"
	
	"//from Vertex shader\n"
	"in vec2 UV;\n"
	"//Output data\n"
	"layout(location = 0) out vec3 color;\n"
	
	"//Constant mesh data\n"
	"uniform sampler2D texture;\n"

	"void main() {\n"
	"	color = texture2D(texture, UV).rgb;\n"
	"	//color = vec3(0.0,1.0,0.0);\n"
	"}\n"
};

#endif
