MQTT
====
This is <del>probably</del> almost definitely the prototcol I'll be using to send / receive data to the ESP32 device. The port typically used for MQTT is 1883, and for websockets port 9001.

The broker has been tested locally, as well as its ability to communicate with the ESP32 via the [*ESP32 client*](mqtt_client_esp32/mqtt_client_esp32.ino). All that is currently untested is the ability to setup an MQTT client on our webapp, (this will probably be done using [**MQTT.js**](https://www.npmjs.com/package/mqtt) - will help yuna if needed).

Other than setting up encryption using SSL (I'll try this tomorrow), not too sure what I can add to this until we have a better idea of the things we are sending / receiving, need the other modules to progress more.

Mosquitto broker
----------------
I've set up an MQTT broker on my AWS instance using [mosquitto](http://mosquitto.org/download/).

Followed the following [guide](https://obrienlabs.net/how-to-setup-your-own-mqtt-broker/) to get a mosquitto set up on my AWS instance. Have authentication (for more details, see [**users**](#users)), clients must know password in order to publish or read from particular topics, as well as websockets (not sure if this will be needed). The following commands are useful:
- `sudo service mosquitto start` to start up the MQTT broker
- `sudo mosquitto -c /etc/mosquitto/mosquitto.conf` to ensure authentication permissions are being used (I think this needs to be running to use sub / pub)
- `mosquitto_sub -h localhost -t weather/#` to subscribe to the "weather" topic (example, change names and add topics as needed). The `#` means that it will listen to all subtopics under weather (only use for debugging)
- `mosquitto_pub -h localhost -t "weather/test" -m "<message>"` to publish to a topic (this wont work with authentication)
- `mosquitto_pub -h localhost -t "weather/test" -m "<message>" -u "<username>" -P "<password>"` to publish to a topic using authentication
- `sudo sudo cat /var/log/mosquitto/mosquitto.log` to view logs

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
- `toESP32/x_pos`contains desired x-coordinate
- `toESP32/y_pos` contains desired y-coordinate
- `toESP32/target` contains the colour being targeted

**fromESP32**
Published to by ESP32, subscribed to by web-app
- `fromESP32/x_pos` contains current x_pos
- `fromESP32/y_pos` contains current y_pos
- `fromESP32/target` indicated if we have reached the target
- `fromESP32/power` contains power information (battery level)

We can therefore update the publishing rights to the toESP32 and fromESP32 topics to only allow the web-app / rover to respectively publish to those topics. Additionally these topics can only be read from our web-app / rover to prevent outside connections from accessing them

<a name="users"></a>Users
-------------------------
I am making the following 2 users to limit access to the topics above (storing the users and passwords here cause plain text password storage is the one). I might keep them here permanently as security is more for the purposes of the assessment, it's not like anyone is actually gonna try and hijack our stuff. Obviously if I needed to be serious about security measures I wouldn't leave these here.
- esp32:#8HAGxb3*V%+CD8^
- webapp:=ZCJ=4uzfZZZ#36f

The rights of these users allows them to use **toESP32** and **fromESP32** as described above.