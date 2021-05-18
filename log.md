Management
==========
This section will track the goals / progress done by me (Simon) for the control module of the rover.

Goals
-----
- Investigate how to use the ESP32-DevkitC as an HTTP client
- Think about how we'll actually do the client/server (currently assuming it'll be an HTTP server)
- Bro idk i wanna kms

Changelog
---------
**17/05/21**
I've found out how to connect the ESP32 board to my local macbook using Arduino IDE, and successfuly uploaded test code and ran it on the board to test wifi functionality.

**18/05/21**
Will most likely use an MQTT server / client to communicate with the ESP32 board, and an HTTP client for the web-app. We can run both on the same AWS server using `screen` (more usage details [here](https://www.howtogeek.com/662422/how-to-use-linuxs-screen-command/))
