//Code was created from the example "Communication/SercialCallRespnseASCII
//Created by Malte Kießling (mkalte666)

#include <Servo.h> 

//Cmds are the following:
//CMD_GET_SERIAL_DATA 0
//CMD_SET_SHUTDOWN 99

int inByte = 0;         // incoming serial byte
short *rotation;
short *lastrot;
Servo xrot;
Servo yrot;
Servo zrot;
unsigned long time;
short  recv_time;
unsigned long recv_timer;

void setup()
{
  // start serial port at 9600 bps and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  
  rotation = new short[3];
  lastrot  = new short[3];
  rotation[0] = lastrot[0] = rotation[1] = lastrot[1] = rotation[2] = lastrot[2] = 0;
  xrot.attach(3);
  yrot.attach(2);
  zrot.attach(4);
  xrot.write(0);
  yrot.write(0);
  zrot.write(0);
  recv_timer = time = millis();
  recv_time = 20;
}

void loop()
{
  time = millis();
  // if we get a valid byte, read analog ins:
  if (Serial.available() > 0) {
    // get incoming byte:
    inByte = Serial.read();
    if(millis()-recv_timer > recv_time) {
      
      char recvdata[sizeof(short[3])];
      int i = 0;
      switch(inByte) {
      case 0:
          for(i = 0; i<sizeof(short[3]);i++)
            recvdata[i] = Serial.read();
          rotation = (short*)&recvdata[0];
  
          //if(rotation[0]!= lastrot[0]) {
            xrot.write(rotation[0]);
            lastrot[0] = rotation[0];
          //}
          //if(rotation[1]!= lastrot[1]) {
            yrot.write(rotation[1]);
            lastrot[1] = rotation[1];
          //}
          //if(rotation[2]!= lastrot[2]) {
            zrot.write(rotation[2]);
            lastrot[2] = rotation[2];
         // }
          
          
          //done, wait for new data!
          break;
          
     case 99:
          xrot.write(90);
          yrot.write(90);
          zrot.write(90);
    
       }    
       recv_timer = millis();
    }  
  }
}


