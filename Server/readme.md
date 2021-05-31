Server Breakdown
================
This folder contains the files for the backend REST API to be run on our AWS instance, as well as information about the contents of the instance itself. We use an EC2 AWS instance to simultaniously run the following:
- Mosquitto MQTT broker
- REST API
- Front-end REACT web-app
- nginx web server
Please find a more detailed explaination of each below:

Mosquitto MQTT broker
---------------------
This is a broker that is custom set-up to fit our needs. For more information about the broker, check [**here**](../MQTT/readme.md). When setting up the server, the following command needs to be run to start up the broker:
        `sudo mosquitto -c /etc/mosquitto/mosquitto.conf`

REST API
--------
This is a node.js app which handles requests from the REACT front-end and communicates to our ESP32 via the MQTT broker, and is contained in this folder. It listens for requests on port 8080 (potentially update the instance details to ban outside connection to this port). To boot it up, simply run `npm start` in this directory.

REACT Web-App
-------------
This is our front-end application contained [**here**](../Front_End/React). It is hosted on port 3000, and communicates with the rover via requests to the REST API.

nginx Web Server
----------------
I set up an nginx web server on our AWS instance to allow us to use HTTPS on our website and forward connection requests to http://debonair.duckdns.org to our REACT web app, giving us a proper domain name for browser clients to interact with our rover.

To use HTTPS, I used LetsEncrypt to add an SSL certificate to the AWS instance:
```
Your certificate and chain have been saved at:
   /etc/letsencrypt/live/debonair.duckdns.org/fullchain.pem
Your key file has been saved at:
   /etc/letsencrypt/live/debonair.duckdns.org/privkey.pem
```
Additionally, set up the certificate to auto-renew every week as certificates are only valid for 3 months.