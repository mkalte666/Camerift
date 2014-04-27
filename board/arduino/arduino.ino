//Code was created from the example "Communication/SercialCallRespnseASCII
//Created by Malte Kießling (mkalte666)

#include <Servo.h> 

//Cmds are the following:
//CMD_GET_SERIAL_DATA 0
//CMD_SET_SHUTDOWN 99

int inByte = 0;         // incoming serial byte
float *rotation;
Servo xrot;
Servo yrot;
Servo zrot;

void setup()
{
  // start serial port at 9600 bps and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  
  rotation = new float[3];
  xrot.attach(0);
  yrot.attach(1);
  zrot.attach(2);
  xrot.write(125);
  yrot.write(125);
  zrot.write(125);
}

void loop()
{
  // if we get a valid byte, read analog ins:
  if (Serial.available() > 0) {
    // get incoming byte:
    inByte = Serial.read();
    char recvdata[sizeof(float[3])];
    int i = 0;
    switch(inByte) {
    case 0:
        for(i = 0; i<sizeof(float[3]);i++)
          recvdata[i] = Serial.read();
        rotation = (float*)&recvdata[0];
        //Rotation comes in -180 - 180, we want (from the -90 - 90 range) it mapped to 0-255, so we add 90. 
        rotation[0]+=90.00;
        rotation[1]+=90.00;
        rotation[2]+=90.00;
        //now we map it to 0-255 and set the servos. 
        xrot.write(rotation[0]*255/180);
        yrot.write(rotation[1]*255/180);
        zrot.write(rotation[2]*255/180);
        //done, wait for new data!
        break;
        
   case 99:
        xrot.write(125);
        yrot.write(125);
        zrot.write(125);
  
     }      
  }
}


