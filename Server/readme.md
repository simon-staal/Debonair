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
This is a node.js app which handles requests from the REACT front-end and communicates to our ESP32 via the MQTT broker, and is contained in this folder. <del>It listens for requests on port 8080 (potentially update the instance details to ban outside connection to this port). To boot it up, simply run `npm start` in this directory.</del> Updated the REST API to use HTTPS to ensure that front-to-back communication is fully encrypted. Can also listen using HTTP on port 8080, but this option is currently disabled, only allowing https connection on port 8443 @ https://debonair.duckdns.org:8443. The backend uses the same certificates that were set up using LetsEncrypt for our nginx web server.

REACT Web-App
-------------
This is our front-end application contained [**here**](../Front_End/React). It is hosted on port 3000, and communicates with the rover via requests to the REST API. For our app to run on our AWS instance, I needed to downgrade npm to npm@7.5.4 (as per [**this issue**](https://github.com/facebook/create-react-app/issues/10811)).

nginx Web Server
----------------
I set up an nginx web server on our AWS instance to allow us to use HTTPS on our website and forward connection requests to debonair.duckdns.org to our REACT web app, giving us a proper domain name for browser clients to interact with our rover.

To use HTTPS, I used LetsEncrypt to add an SSL certificate to the AWS instance:
```
Your certificate and chain have been saved at:
   /etc/letsencrypt/live/debonair.duckdns.org/fullchain.pem
Your key file has been saved at:
   /etc/letsencrypt/live/debonair.duckdns.org/privkey.pem
```
Additionally, set up the certificate to auto-renew every week as certificates are only valid for 3 months.

Fixed HTTPS issues, so now have full front-to-back encryption!
