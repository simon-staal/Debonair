Debonair - Mars Rover Project
==================
This repository will contains all the files developed as part of ELEC50008 - Engineering Design Project II (End of Year project for Y2 EEE/EIE).

**IMPORTANT:**
Please note that there are still multiple branches that are up on this repository:
- Drive_* branches have varying implementations of Drive code due to issues with running original version in master
- Power_Integration contains dual-core ESP32 code meant to be run when the Power module was integrated into the overall system
- performance_testing is a branch that was used to run a variety of benchmarking tests, was left up for posterity / if needed to be used again.

Find the guide to the different folders below:

Control
--------

Front_End
---------
A folder containing the React app used for the front-end of our webapp. 

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
