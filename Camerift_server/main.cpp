/*
File: main.cpp
Purpose: The Server Sends the webcam-frames to the clients and sends the rotation-data to the atmel
Author(s): Malte Kieﬂling (mkalte666)
*/
#include "../shared/base.h"
#include "../shared/networkcmd.h"
#include "servocontroler.h"

using namespace std;

int main(int argc, char**argv)
{
	//Print some text. So the user knows what to do etc.
	cout << "Welcome to the Camerift-server. If you have questions, show in the README.txt-file" << endl;
	//Fist we need to set up our server. -> bind sokets, listen, ...
	//WSAStartup for windows systems
	cout << "Init Sockets...\n";
#ifdef _WIN32
	cout << "Windows: WSAStartup etc. \n";
	int error;
	WSAData wsadata;
	error = WSAStartup(MAKEWORD(2,0), &wsadata);
	if(error != 0) {
		cerr << "Error: WSAStartup failed with code " << error << "!\n";
		return 1;
	}
#endif
	cout << "Creating Sockent and binding Server...\n";
	//Create the soket. Udp is most usefull because it's easyer to 1. drop frames and 2. we are sending Images -> whole data banches
	SOCKET serversock = socket(AF_INET, SOCK_STREAM, 0);
	//Address-structures for the client and the server
	SOCKADDR_IN server_addr; 
	SOCKADDR_IN remote_addr;
	int remote_addr_len = sizeof(SOCKADDR_IN);
	//Filling server_addr with data for bind()
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(NET_STANDART_PORT);
	server_addr.sin_addr.S_un.S_addr = ADDR_ANY;
	//And bind the server
	int binderr = bind(serversock, (SOCKADDR*)&server_addr, sizeof(SOCKADDR_IN));
	if(binderr == SOCKET_ERROR) {
		cerr << "Error while binding Socket at port "<< server_addr.sin_port << "! Do you have permissions to bind a socket?\n";
		return 1;
	}
	cout << "Done. Trying to create capture...\n";
	//Lets create a Mat and a Capture for the webcams
	cv::VideoCapture left_capture(1);
	cv::VideoCapture right_capture(10);
	int num_cams = 2;
	//Look if we have webcams connected
	if(!left_capture.isOpened()) {
		num_cams = 0;
		cerr << "No webcams connected to the system. Maybe you don't have the permission to access them?!\n";
		return 1;
	}
	//If we cannot capture the 2. Webcam we want to use the same image as webcam1
	bool onlyLeftCapture = false;
	
	if(!right_capture.isOpened()) {
		onlyLeftCapture = true;
		num_cams = 1;
		cout << "Warning: Running in Left-view-only-mode!!! Adding a second webcam might be a good idea!\n";
	} 
	
	bool running = true;
	cout << "Succesfull! Now I wait for connections...\n";
	//Ok, now it's time to wait until we get commands (See shared/networkcommands.h) from the client. 
	cv::Mat frame_left;
	cv::Mat frame_right;
	
	left_capture >> frame_left;
	if(!onlyLeftCapture)
		right_capture >> frame_right;
	else {
		frame_left.copyTo(frame_right);
	}


	//Listen for a connection
	listen(serversock, 1);
	SOCKET connectedSocket;
	connectedSocket = accept(serversock, (SOCKADDR*)&remote_addr, &remote_addr_len);
	onlyLeftCapture = true;
	servocontroler servos("COM9");
	float *recvrotation = new float[3];
	short *lastrotation = new short[3];
	short *rotation = new short[3];
	lastrotation[0] = rotation[0] = lastrotation[1] = rotation[1] = lastrotation[2] = rotation[2] = 0;
	char cmd[NET_CMD_BUFFER_LENGTH];

	while(running) {
		//Capture magic
		cv::waitKey(16);
		cmd[0] = 0;
		//Read the cmd from the client
		recv(connectedSocket,cmd,NET_CMD_BUFFER_LENGTH,0);
		int size_image = 0;
		switch(cmd[0]) {
		case NET_CMD_GET_IMAGESIZE:
			size_image = frame_left.cols*frame_left.rows*3;
			send(connectedSocket, (char*)&size_image, NET_CMD_SIZE_LENGTH, 0);
			cout << "Size-request from" << remote_addr.sin_addr.S_un.S_addr << "!\n";
			break;

		case NET_CMD_GET_CAMERA_NUM:
			cout << "Camera-num reqest from " << remote_addr.sin_addr.S_un.S_addr << "("<< num_cams << ") !\n";
			send(connectedSocket, (char*)&num_cams, NET_CMD_SIZE_LENGTH, 0);
			break;

		case NET_CMD_GET_IMAGESIZE_WIDTH:
			size_image = frame_left.cols;
			send(connectedSocket, (char*)&size_image, NET_CMD_SIZE_LENGTH, 0);
			cout << "Width-request from " << remote_addr.sin_addr.S_un.S_addr << "("<< size_image << ") !\n";
			break;
		case NET_CMD_GET_IMAGESIZE_HEIGHT:
			size_image = frame_left.rows;
			send(connectedSocket, (char*)&size_image, NET_CMD_SIZE_LENGTH, 0);
			cout << "Height-request from " << remote_addr.sin_addr.S_un.S_addr << "("<< size_image << ") !\n";
			break;

		//The clienet will (hopefully) ask for the size of the image before requesting one, but he DON'T have to. The commands below are just sending the images in raw data. 
		//Maybe encoding is a usefull thing to implement here...
		case NET_CMD_GET_IMAGE_LEFT:
			left_capture >> frame_left;
			size_image = frame_left.cols*frame_left.rows*3; //RGB...
			send(connectedSocket, (char*)frame_left.data, size_image, 0);
			break;

		case NET_CMD_GET_IMAGE_RIGHT:
			if(!onlyLeftCapture)
				right_capture >> frame_right;
			else {
				frame_left.copyTo(frame_right);
			}
			size_image = frame_left.cols*frame_left.rows*3;
			send(connectedSocket, (char*)frame_right.data, size_image, 0);
			break;

		//The client will send us the rotation so we can send it to the servos/steppers/...
		case NET_CMD_SET_ROT:
			recv(connectedSocket, (char*)recvrotation, NET_CMD_ROT_DATA, 0);
			//The Rotation is float, the arduino wants intetger. So lets do it :) Also map -90 - 90 to 0-180. Also the rotation is MAX -180 - 180 -> short
			rotation[0] = (short)floor(recvrotation[0]) + 90;
			rotation[1] = (short)floor(recvrotation[1]) + 90;
			rotation[2] = (short)floor(recvrotation[2]) + 90;
			for (int i = 0; i < 3; i++) {
				if (rotation[i] < 0)
					rotation[i] = 0;
				else if (rotation[i] > 180) 
					rotation[i] = 180;
			}
			
			//debug
			if(rotation[0] != lastrotation[0] || rotation[1] != lastrotation[1] || rotation[2] != lastrotation[2]) {
				std::cout << "Rotation: X[" << rotation[0] << "] Y][" << rotation[1] << "] Z[ " << rotation[2] << std::endl;
				servos.WriteCmd(servocontroler::CMD_SET_SERVO);
				servos.WriteData((char*)rotation, sizeof(short[3]));
				lastrotation[0] = rotation[0];
				lastrotation[1] = rotation[1];
				lastrotation[2] = rotation[2];
			}
			break;
		
		case NET_CMD_SET_SERVERSTOP:
			running = false;
			break;
		}

		
	}


}

