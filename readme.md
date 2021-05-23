MARS ROVER PROJECT
==================
This repository will contain the files developed as part of the End of Year project for Y2 (EEE/EIE).

Find the guide to the different folders below:

express_test
------------
This folder was made to test a simple server using express framework on top of node.js

HTML
----
A folder containing all the HTML stuff for the front-end of our webapp. Also contains the [**React**](HTML/React) subdirectory which contains a simple basic React project, which we will most likely use for the server front-end.

MQTT
----
A folder containing details about how the MQTT protocol will be used to communicate between our server and ESP32 control chip on the actual rover, as well as the arduino files for the ESP32 chip.

Server
------
Contains a variety of node.js files that are being used as testing / examples for our REST API backend:
- [**index.js**](Server/index.js) is an example HTTP server that I was able to successfully host on a remote AWS instance and connect to from my local PC. This file is pretty irrelevant at this point.
- [**mqtt_server.js**](Server/mqtt_server.js) is an example of an MQTT client for the server-side which will be used to communicate with our ESP32.
- [**express_example.js**](Server/express_example.js) is an example of how express.js can be used to handle with resource requests from our React front-end to different end-points of our webapp.
- [**React+Express_link_example**](Server/React+Express_link_example) contains 2 files, an react_app.js file representing the React front-end and a restAPI.js file which represents the server-backend written using node.js + express.js, and shows how you can link the 2 so that the server back-end can provide the resources requested by the React app.

It also contains the .pem file required to SSH into my AWS instance, as well as [**run_server.sh**](Server/run_server.sh), a script I will be using to run everything on our AWS instance required for the project (will be updating this as we go, currently untested)

Rover_combined and Rover_functions
------
Both contain Arduino code related to the Drive subsystem. 
Rover_combined is the fusion of: SMPS code, Optical sensor code and Motor control code, while Rover_functions is a work in progress with all the functions that map the inputs from the Command subsystem to actual Rover movements.
