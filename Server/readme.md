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
This is a broker that is custom set-up to fit our needs. For more information about the broker, check [here](../MQTT/readme.md). When setting up the server, the following command needs to be run to start up the broker:
        `sudo mosquitto -c /etc/mosquitto/mosquitto.conf`

REST API
--------
This is a node.js app which handles requests from the REACT front-end and communicates to our ESP32 via the MQTT broker, and is contained in this folder. It listens for requests on port 8080. To boot it up, simply run `npm start`.

REACT Web-App
-------------
This 

nginx Web Server
----------------

Added an SSL certificate to the AWS instance:
```
Your certificate and chain have been saved at:
   /etc/letsencrypt/live/debonair.duckdns.org/fullchain.pem
Your key file has been saved at:
   /etc/letsencrypt/live/debonair.duckdns.org/privkey.pem
```
Additionally, set up the certificate to auto-renew every week as certificates are only valid for 3 months.