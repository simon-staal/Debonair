Debonair - Mars Rover Project
==================
This repository contains all the files developed as part of ELEC50008 - Engineering Design Project II (End of Year project for Y2 EEE/EIE).

For this project, a fully integrated rover system was designed with the following operational modes:
- **Exploration-** The rover autonomously explores the target environment, identifying and reporting obstacle positions.
- **Coordinate-** Given a set of destination coordinates, the rover will autonomously navigate through the environment to reach the destination while avoiding obstacles.
- **Remote control-** The rover drive is fully controlled by the user, allowing them to explore the environment as they see fit.

A web application available [**here**](https://debonair.duckdns.org) was designed as the control hub for the rover, where the different operational modes can be selected between and information from the rover is received.

Check [**here**](https://youtu.be/QJuqwNQwGvM) for a demo video of the system in action.

**IMPORTANT:**
Please note that there are still multiple branches that are up on this repository:
- Drive_* branches have varying implementations of Drive code due to issues with running original version in master
- Power_Integration contains dual-core ESP32 code meant to be run when the Power module was integrated into the overall system
- performance_testing is a branch that was used to run a variety of benchmarking tests, was left up for posterity / if needed to be used again.

Find the guide to the different folders below:

[Control](Control)
--------
This folder contains the code loaded on the ESP32 to manage communications between the rover and the web-server. Also contains the [**Comms**](Control.Comms) subfolder which was developed to test ESP32 communication capabilites with the Arduino Nano Every.

[Drive](Drive)
-------
This folder contains code developed as a part of the Drive subsystem. Please note that in the different Drive_* branches, this code is usually stored in Rover_combined/Rover_rearranged (no longer in master).

[Energy](Energy)
-------
This folder contains all the code and data used by the Energy subsystem.

[Front_End](Front_End/React)
---------
This folder contains the React app used for the front-end of our webapp. 

[MQTT](MQTT)
----
This folder contains details about how the MQTT protocol will be used to communicate between our server and ESP32 control chip on the actual rover, outlining the different users allowed by the MQTT broker, useful debugging tools, as well as the broker configuration and encryption certificate.

[Server](Server)
------
This folder conatins the REST API used for the server backend, as well as a documentation detailing the features of the AWS instance infrastructure and the scripts+permission files necessary to setup the instance for deployment. Also contains the [**pathfinding module**](Server/pathfinding) used by our REST API. 

Additionally contains a sub-folder [**examples**](Server/examples), which contains a variety of node.js files that were used as part of testing / development for the REST API pre-integration:
- [**index.js**](Server/examples/index.js) is an example HTTP server that I was able to successfully host on a remote AWS instance and connect to from my local PC.
- [**mqtt_server.js**](Server/examples/mqtt_server.js) is an example of an MQTT client for the server-side which will be used to communicate with our ESP32.
- [**express_example.js**](Server/examples/express_example.js) is an example of how express.js can be used to handle with resource requests from our React front-end to different end-points of our webapp.
- [**React+Express_link_example**](Server/examples/React+Express_link_example) contains 2 files, an react_app.js file representing the React front-end and a restAPI.js file which represents the server-backend written using node.js + express.js, and shows how you can link the 2 so that the server back-end can provide the resources requested by the React app.

[Vision](Vision)
--------------
Contains all the files needed to setup the Vision subsystem.

Contributors
------------
- Simon Staal (sts219) - Control, MQTT, Server
- Khayle Torres (kt1719) - Vision
- Yuna Valade (yv19) - Front_End
- Ruwan Silva (rs1018) - Control/Comms, Pathfinding algorithm
- Margherita Contri - Drive
- Yash Rajput (yr419) - Energy
