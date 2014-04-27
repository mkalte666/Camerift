/*
File: servocontroler.h
Purpose: Class that manages the Communication with the Arduino
Author(s): Malte Kieﬂling (mkalte666)
*/
#pragma once
#include "../shared/base.h"

class servocontroler
{
public:

	enum COMMANDS {
		CMD_SET_SERVO = 0,
		CMD_RESET= 99,
	};
	servocontroler(const char* port);
	~servocontroler(void);

	bool WriteCmd(char cmd);
	bool WriteData(char* src, int len);
	bool ReadData(char* dst, int len);

private:
	bool		m_valid;

#ifdef WIN32
	//Here is our windows code for the communication port. 
	HANDLE		m_hComm;

#endif
};

