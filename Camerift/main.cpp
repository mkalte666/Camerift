/*
File: main.cpp
Purpose: Main file of the client. Connects to the server, displays the webcams and sends rotation data.
Author(s): Malte Kieﬂling (mkalte666)
*/

#include "CameriftInstance.h"
#include "HMD.h"

using namespace std;

bool InitNetwork();

int main(int argc, char**argv)
{
	InitNetwork();
	InitHMD();
	int port = NET_STANDART_PORT;
	char* ip = "127.0.0.1";

	switch(argc) {
	case 3:
		port = atoi(argv[2]);
	case 2:
		ip =  argv[1];
		break;

	}

	CameriftInstance Instance(ip, port);
	Instance.InitGL();
	Instance.loop();

	//Deinit systems
	DestroyHMD();
	return 0;

}
bool InitNetwork()
{

	//When we use windows, we should init Winsock. else, ehh, not
#ifdef _WIN32
	cout << "Windows: WSAStartup etc. \n";
	int error;
	WSAData wsadata;
	error = WSAStartup(MAKEWORD(2,0), &wsadata);
	if(error != 0) {
		cerr << "Error: WSAStartup failed with code " << error << "!\n";
		return false;
	}
#endif

	return true;
}
