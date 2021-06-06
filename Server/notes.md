Important (DONE)
=========
Will need to wipe all these example garbage files when making the actual REST API to interface with Yuna's front-end React app. When making a fresh project, makes sure to run the following:
```
npm init
npm install --save-dev nodemon **This is to relaunch the server whenever changes are made**
npm install --save express **We are using express for routing**
```
<del>`npm install --save body-parser`</del> (Don't think I need this)
```
npm install --save mqtt **To hand mqtt client stuff**
npm install --save cors **This might be needed to allow use of browsers**
```

At the top of the node.js file, include the following:
```
const express = require('express');
const cors = require('cors');
const app = express();
app.use(express.json());
app.use(express.urlencoded({ extended: true }));
app.use(cors());
```
This should hopefully get everything working.
The app.use stuff with express is for handling parsing of JSON objects that could be received in POST requests from React.

IF PULLING FROM GITHUB FOR THE FIRST TIME, RUN THE FOLLOWING TO LAUNCH THE SERVER:
```
npm install
npm start
```

Notes
=====
**18/05/21**
Starting to work on building the backend server in node.js.
To run the server, simply run `node index.js` (make sure node is up to date).
To then test the server functionality, in another terminal you can use `curl localhost:8080` (in this particular case we are using port 8080)

Going to try to run server in AWS instance and see if I can connect from local PC.

*Update* THIS WORKS!!!!!!
Downloaded repo onto AWS instance, used screen to run `node index.js` in a seperate whatever the fuck, did `curl <server_ip>:8080` on local PC and received a response

**23/05/21**
Trying to test the mqtt_server.js file, I am able to successfully connect to the broker and subscribe to the topic, but the client seems to be unable to detect messages. Further testing is requires, will work on that tomorrow.

*Update:*
Tested mqtt_server.js again, is successfully authenticating and can receive messages, but doesn't subscribe to all subtopics. Fixed this issue in mqtt_server code and now can receive messages from all subtopics. Also tested publishing capabilities, which work.

**24/05/21**
Trying to set up SSL certificate for the MQTT broker. To do this I'm setting up a dynamic DNS hostname for the AWS instance which is hosting the broker. Currently hostname is **http://debonair.duckdns.org** and is manually linked to our instance IP.

**27/05/2021**
Trying to get SSL encryption working, got error for current certificate being used for mqtt.js:
```
cannot connect Error [ERR_TLS_CERT_ALTNAME_INVALID]: Hostname/IP does not match certificate's altnames: Host: debonair.duckdns.org. is not in the cert's altnames: IP Address:172.31.46.12, IP Address:FE80:0:0:0:8E1:59FF:FE12:3116, IP Address:127.0.0.1, IP Address:0:0:0:0:0:0:0:1, DNS:ip-172-31-46-12.eu-west-2.compute.internal, DNS:localhost
```
Basically suggests to me that the certificate I generated used the internal private IP address instead of the external one. Need to have a play around and try to figure out how to solve this. For now, cavemanned changes in such a way that everything still works like before (hopefully).

**31/05/2021**
Got SSL encryption working for MQTT stuff, all MQTT communication is now encrypted. Additionally, have HTTPS setup on nginx, but I cannot make http requests to our backend (must make HTTP requests), meaning that https://debonair.duckdns.org doesn't have full functionality. Might try working on this on a branch, potentially useful info here: https://stackoverflow.com/questions/11744975/enabling-https-on-express-js. Otherwise, using http://debonair.duckdns.org works fine.

General
=======
- For MQTT stuff, will probs use [MQTT.js](https://www.npmjs.com/package/mqtt#api)
Will probs need to install MQTT globally: `npm install mqtt -g`

- For routing, refer to the following: https://expressjs.com/en/guide/routing.html

- Handling post requests (from frontend)
https://codeforgeek.com/handle-get-post-request-express-4/

- How to send post requests (in frontend) - **USE AXIOS**
https://jasonwatmore.com/post/2020/07/17/react-axios-http-post-request-examples

- Managing dynamic DNS hostname for server using [Duck DNS](https://www.duckdns.org/)

- SSL stuff: https://jamielinux.com/docs/openssl-certificate-authority/index.html

MongoDB
-------
Using MongoDB for database, following user details to access cluster:
s_staal:LNFfaKDXPekXvb76
