MQTT
====
This is <del>probably</del> definitely the prototcol I'll be using to send / receive data to the ESP32 device. The port typically used for MQTT is 1883, and for websockets port 9001.

The broker has been tested locally, as well as its ability to communicate with the ESP32 via the [*ESP32 client*](../Control/mqtt_client_esp32/mqtt_client_esp32.ino). The [**pubsubclient**](https://github.com/knolleary/pubsubclient) library was used for this.

An example MQTT client for the webapp has been developed and tested, and can be found [*here*](../Server/mqtt_server.js). This was done using [**MQTT.js**](https://www.npmjs.com/package/mqtt). The client will be integrated as part of our backend REST API written in node.js.

Other than setting up encryption using SSL (I'll try this tomorrow), not too sure what I can add to this until we have a better idea of the things we are sending / receiving, need the other modules to progress more.

Bound the server IP to debonair.duckdns.org, using mqtts://debonair.duckdns.org allows us to connect to the broker from a node.js server.

Mosquitto broker
----------------
I've set up an MQTT broker on my AWS instance using [mosquitto](http://mosquitto.org/download/).

Followed the following [guide](https://obrienlabs.net/how-to-setup-your-own-mqtt-broker/) to get a mosquitto set up on my AWS instance. Have authentication (for more details, see [**users**](#users)), clients must know password in order to publish or read from particular topics, as well as websockets (not sure if this will be needed). The following commands are useful:
- `sudo service mosquitto start` to start up the MQTT broker
- `sudo mosquitto -c /etc/mosquitto/mosquitto.conf` to ensure authentication permissions are being used (I think this needs to be running to use sub / pub)
- `mosquitto_sub -h localhost -t test/#` to subscribe to the "weather" topic (example, change names and add topics as needed). The `#` means that it will listen to all subtopics under weather (only use for debugging)
- `mosquitto_pub -h localhost -t "test/test" -m "<message>"` to publish to a topic (this wont work with authentication)
- `mosquitto_pub -h localhost -t "test/test" -m "<message>" -u "<username>" -P "<password>"` to publish to a topic using authentication
- `sudo cat /var/log/mosquitto/mosquitto.log` to view logs
- `sudo nano /etc/mosquitto/acl` to edit permissions
- `sudo nano /etc/mosquitto/conf.d/myconfig.conf` to edit config

**SSL**
SSL encryption for the broker was set up as follows:
- I used a modified version of [**this**](https://github.com/owntracks/tools/blob/master/TLS/generate-CA.sh) script, where I specified the host to be the public IP address of the AWS instance. 
- I moved the ca.crt and IP.* files to /etc/mosquitto/certs and updated the mosquitto config file.
- Rebooted instance and tested as follows:
```
mosquitto_sub -t \# -v --cafile /etc/mosquitto/certs/ca.crt -p 8883
     
mosquitto_pub --cafile /etc/mosquitto/certs/ca.crt -h localhost -t "test/secure" -m "hello securely" -p 8883
```
- Then copied over the ca.crt file and used convert.cpp to put it into the mqtt_client.ino file for the esp32.
- MQTT client now using WiFiClientSecure instead of just WifiClient, and certificate is used to connect to the broker securely.
- On the REST API side, used updated settings to connect to the broker, but since the connection is local, unencrypted communication can also be used.

Updated broker to use encrypted communication on port 8883 with the outside world, only using port 1883 locally. MQTT client works with this.

Updated esp32 mqtt client to use encrypted port, untested (will test again at home)
Refer to this: http://www.iotsharing.com/2017/08/how-to-use-esp32-mqtts-with-mqtts-mosquitto-broker-tls-ssl.html

Control
-------
**TOPICS**
Initialisation:
- Web-app says to start mapping
- Control sends back when we're done mapping
- We will need to send location of ping pong balls
*Possible implementation*
- We have a specific order in which we loop through the ping pong balls to find
- The target ball is sent by our server to the esp, which is forwarded to vision
- Vision sends angle to drive to tell it where to turn / drive

General:
- Drive receives coordinates
- Drive receives actual directions
- Drive sends coordinates
- Energy sends battery level
- We can calculate range server side (hopefully)

Therefore suggests the following topics:
**toESP32**
Published to by web-app, subscribed to by ESP32
- `toESP32/x_coord`contains desired x-coordinate
- `toESP32/y_coord` contains desired y-coordinate
- `toESP32/direction` contains direction being steered
- `toESP32/target` contains the colour being targeted

**fromESP32**
Published to by ESP32, subscribed to by web-app
- `fromESP32/x_coord` contains current x_pos
- `fromESP32/y_coord` contains current y_pos
- `fromESP32/target` indicated if we have reached the target
- `fromESP32/power` contains power information (battery level)

We can therefore update the publishing rights to the toESP32 and fromESP32 topics to only allow the web-app / rover to respectively publish to those topics. Additionally these topics can only be read from our web-app / rover to prevent outside connections from accessing them

<a name="users"></a>Users
-------------------------
I am making the following 2 users to limit access to the topics above (storing the users and passwords here cause plain text password storage is the one). I might keep them here permanently as security is more for the purposes of the assessment, it's not like anyone is actually gonna try and hijack our stuff. Obviously if I needed to be serious about security measures I wouldn't leave these here.
- esp32:#8HAGxb3*V%+CD8^
i.e. mosquitto_pub -h localhost -t "fromESP32/test" -m "<message>" -u "esp32" -P "#8HAGxb3*V%+CD8^"
- webapp:=ZCJ=4uzfZZZ#36f

The rights of these users allows them to use **toESP32** and **fromESP32** as described above.