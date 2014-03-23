====Camerift====
Camera control with the Oculus Rift

==Information==
Camerift was created because of a school project, so don't expect too much.
The plan is to have an easy-useable, cheap and and OpenSource solution for telepresence.

==The Project now==
Camerift is still in development. What you can see here is a complete recode of the first version (as it can be seen on youtube - see below).
At the moment it doesnt support Servos/Steppers - but this fuction will follow. So be patient :)

==How to Compile==
Ok, that is easy (or not?). At first you need a version of the OculusSDK. I myself copyed LibOVR to Camerift/3rdPary, and changed my linker settings. 
You should see how you want to do that. Same goes for OpenCV, but you need it for the Server: I copyed it to Camerift_server/3rdParty.
Also, you will need 3 Other libarys you have to add to your system: GLFW3, GLM and GLEW. 

To Compile the Project on Windows, Simply openen The Solutuion in VS2010 (or VSC++Express) and click Complile. 
On Linux you should instal the deps and run make - only that there isnt a makefile, yet. Sorry for that, it will follow soon.

Problems? Contact me at mkalte666 [at(@)goes here] googlemail.com

