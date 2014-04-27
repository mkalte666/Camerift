/*
File: networkcmd.h
Purpose: A shared header containing some information needed for the network communikation
Author(s): Malte Kieﬂling (mkalte666)
*/

#ifndef NETWORKCMD_HEADER
#define NETWORKCMD_HEADER

//Some information for memory alloctaion
#define NET_STANDART_PORT 13146
#define NET_CMD_BUFFER_LENGTH 1
#define NET_CMD_SIZE_LENGTH (sizeof(int))
#define NET_CMD_ROT_DATA (3*sizeof(float))

//Commands
#define NET_CMD_GET_IMAGE_LEFT 1
#define NET_CMD_GET_IMAGE_RIGHT 2
#define NET_CMD_GET_IMAGESIZE 3
#define NET_CMD_GET_IMAGESIZE_WIDTH 4
#define NET_CMD_GET_IMAGESIZE_HEIGHT 5
#define NET_CMD_GET_CAMERA_NUM 6
#define NET_CMD_SET_ROT 7

#define NET_CMD_SET_SERVERSTOP 90

#endif