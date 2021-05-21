Management
==========
This section will track the goals / progress done by me (Simon) for the control module of the rover.

Goals
-----
- <del>Investigate how to use the ESP32-DevkitC as an HTTP client</del> (Reworked 20/05/21)
- Think about how we'll actually do the client/server (currently assuming it'll be an HTTP server)
- Bro idk i wanna kms
- Test MQTT broker using node.js server
- <del>Program an MQTT client onto ESP32 using arduino IDE and test connection with broker</del> (Marked complete 21/05/21)
- Try adding SSL encryption to our MQTT client (this bitch gonna be so secure)

Changelog
---------
**17/05/21**
I've found out how to connect the ESP32 board to my local macbook using Arduino IDE, and successfuly uploaded test code and ran it on the board to test wifi functionality.

**18/05/21**
Will most likely use an MQTT server / client to communicate with the ESP32 board, and an HTTP client for the web-app. We can run both on the same AWS server using `screen` (more usage details [here](https://www.howtogeek.com/662422/how-to-use-linuxs-screen-command/))

*Update_1:* Added a bunch of stuff to the repo, working mostly on server backend at the moment using node.js, and ensuring that it works on our AWS instance and that I can communicate with it locally. The `screen` command should let us run mulitple processes on our server at the same time, which will be good as we will probably have an HTTP server for the webpage which will be communicating with an MQTT server that will interface with the MQTT client on our ESP32 to send and receive instructions to our rover.

Started integrating `screen` functionality into server startup script, so everything should be able to run in parallel. Not added yet, but found command to stop a screen in a script as follows:`screen -X -S "sessionname" quit`

**20/05/21**
Been focusing on working on the the full-stack web dev course to learn more about node.js and React. Made some progress today with MQTT, was able to set up a broker on the AWS server, which seems to work, and found js libraries to use so that our web-app can interact with our broker. Added some new goals to work on for the ESP32 side of things, want to try get communication between the ESP32 and the server by this weekend.

**21/05/21**
Huge progress on the MQTT client. Have a working MQTT client on the ESP32, and buffed up the security of our MQTT broker on the aws side of things. Tested both reading data from a topic on the ESP32 and publishing to a topic, meaning our rover should now be able to communicate wirelessly via the ESP32 chip. Ruwan is working on the communication between the ESP32 and the arduino / FPGA.