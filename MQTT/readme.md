MQTT
====
This is probably the prototcol I'll be using to send / receive data to the ESP32 device. The port typically used for MQTT is 1883, and for websockets port 9001.

Use [mosquitto](http://mosquitto.org/download/) on server side as MQTT broker.

Followed the following [guide](https://obrienlabs.net/how-to-setup-your-own-mqtt-broker/) to get a mosquitto set up on my AWS instance. Have authentication, clients must know password in order to publish to a topic, as well as websockets. Currently set up user `test` (password is same as usual). The following commands are useful:
- `sudo service mosquitto start` to start up the MQTT broker
- `sudo mosquitto -c /etc/mosquitto/mosquitto.conf` to ensure authentication permissions are being used (I think this needs to be running to use sub / pub)
- `mosquitto_sub -h localhost -t weather/#` to subscribe to the "weather" topic (example, change names and add topics as needed). The `#` means that it will listen to all subtopics under weather (only use for debugging)
- `mosquitto_pub -h localhost -t "weather/test" -m "<message>"` to publish to a topic (this wont work with authentication)
- `mosquitto_pub -h localhost -t "weather/test" -m "<message>" -u "<username>" -P "<password>"` to publish to a topic using authentication
- `sudo sudo cat /var/log/mosquitto/mosquitto.log` to view logs

Tested the MQTT server on AWS server using multiple SSH connections, seems to be working. Next step is to try accessing it using node.js server / ESP32 client.

**Stuff Petra is doing: (idk if it's relevant)**

```
npm install mqtt --save
npm install mqtt -g

sudo apt update
sudo apt-get install mosquitto mosquitto-clients

sudo service mosquitto start

npm init

sudo npm i -g npx
npx create-react-app client
```