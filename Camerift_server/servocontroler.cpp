/*
File: servocontroler.cpp
Purpose: Contains the Functions needed to cummuticate with servos connected to a board at a com-port
Author(s): Malte Kieﬂling (mkalte666)
*/
#include "servocontroler.h"


servocontroler::servocontroler(const char* port)
{
	m_valid = false;
	std::cout << "Opening Serial port...\n";
#ifdef WIN32
	m_hComm = CreateFile( port,  
                    GENERIC_READ | GENERIC_WRITE, 
                    0, 
                    0, 
                    OPEN_EXISTING,
                    FILE_FLAG_OVERLAPPED,
                    0);
	if (m_hComm == INVALID_HANDLE_VALUE) {
		std::cout << "Cant open Com-port. Running without servos!\n";
		return;
	}

	DCB comm_settings = {0};
	if(!GetCommState(m_hComm, &comm_settings)) {
		std::cout << "Cant read Com-port-settings. Running without servos!\n";
		return;
	}
	
	comm_settings.DCBlength = 8;
	comm_settings.Parity = FALSE;
	comm_settings.StopBits= ONESTOPBIT;
	comm_settings.BaudRate=CBR_9600;

	if(!SetCommState(m_hComm, &comm_settings)) {
		std::cout << "Cant set Com-port-settings. Running without servos!\n";
		return;
	}


#endif
	std::cout << "Serial port: done!\n";
	m_valid = true;
}


servocontroler::~servocontroler(void)
{
	CloseHandle(m_hComm);
}

bool servocontroler::WriteData(char* src, int len)
{
	if(!m_valid) return false;
#ifdef _WIN32
	bool ret;
	DWORD written;
	int writeResult;

	OVERLAPPED osWrite = {0};
	osWrite.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if(osWrite.hEvent == NULL) {
		std::cout << "WARNING: COULD NOT SEND DATA: Overlap event creation failed!\n";
		return false;
	}

	if(!WriteFile(m_hComm, src, len, &written, &osWrite)) {
		if(GetLastError() != ERROR_IO_PENDING) {
			std::cout << "ERROR: Serial write failed!\n";
			ret = false;
		}
		else {
			writeResult = WaitForSingleObject(osWrite.hEvent, 200);
			switch(writeResult) {
			case WAIT_OBJECT_0:
				if(!GetOverlappedResult(m_hComm, &osWrite, &written, FALSE)) {
					std::cout << "ERROR: Serial write faild after a delay!\n";
					ret = false;
				}
				else
					ret = true;
				break;

			default:
				std::cout << "ERROR: Serial write faild. Possible problems with Overlapped-handle.\n";
				ret = false;
				break;
			}
		}
	} 
	else
		ret = true;

	CloseHandle(osWrite.hEvent);
	return ret;
#endif
}

bool servocontroler::WriteCmd(char cmd)
{
	if(!m_valid) return false;

	return WriteData(&cmd, 1);
}

bool servocontroler::ReadData(char *dst, int len)
{
	if(!m_valid) return false;

#ifdef _WIN32

#endif
}