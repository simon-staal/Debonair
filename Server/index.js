const express = require('express');
const cors = require('cors');
const mqtt = require('mqtt');
// Encryption
const https = require('https');
const fs = require('fs')

// ---------------- Admin shit -------------------
const app = new express();
const HTTP_port = 8080;
const HTTPS_port = 8443;

app.use(express.json());
app.use(express.urlencoded({ extended: true }));

const cert = fs.readFileSync('/etc/letsencrypt/live/debonair.duckdns.org/fullchain.pem', 'utf8');
const key = fs.readFileSync('/etc/letsencrypt/live/debonair.duckdns.org/privkey.pem', 'utf8');
const SSL_options = {
  key: key,
  cert: cert
};

// Play around with this shit later if I can be fucked
const corsOptions = {
    origin: 'http://3.8.182.14:3000',
    optionsSuccessStatus: 200
}

app.use(cors()); // Enables CORS for just out REACT APP (both must be running on same server)

// ------------------ MQTT client ---------------
const clientOptions = {
	clientId:"mqttjs01",
	username:"webapp",
	password:"=ZCJ=4uzfZZZ#36f",
	rejectUnauthorized : false // I need to do this for it to work (bad but idgaf)
}
const client  = mqtt.connect("mqtts://debonair.duckdns.org", clientOptions);

// Runs on connection to the broker
client.on("connect", () => {
	console.log("connected " + client.connected);
	// Subscribes to topics on startup
	let topic = 'fromESP32/#';
	client.subscribe(topic, (err, granted) => {
		if (err) {
		 console.log(err);
		 return;
		}
		console.log('Subscribed to topic: ' + topic);
	});
	// Testing publishing ability
	publish('toESP32/test','hello');
})

// Runs if unable to connect to broker
client.on("error", error => {
	console.log("cannot connect " + error);
	process.exit(1)
});

const pubOptions={
	retain:true,
	qos:1
};

// You can call this function to publish to things
function publish(topic,msg,options=pubOptions){
	console.log("publishing",msg);
	if (client.connected == true){
		client.publish(topic,msg,options);
		}
}


// ----------------------- REST API ----------------------
app.get("/",(req,res)=>{
    res.send('Hello from server!');
  });

app.post("/coords", (req,res) => {
    console.log("Request received: " + JSON.stringify(req.body));
    console.log(req.body);
    
    let receivedCoord = {
        'coordinateX': req.body.coordinateX,
        'coordinateY': req.body.coordinateY
    }
    // res.set('Content-Type', 'text/plain');
    console.log(JSON.stringify(receivedCoord));
    publish('toESP32/x_coord',receivedCoord.coordinateX);
    publish('toESP32/y_coord',receivedCoord.coordinateY);
    res.send(receivedCoord);
});

app.post("/move", (req,res) => {
    console.log("Request received: " + JSON.stringify(req.body));
    publish('toESP32/dir', req.body.direction)
    res.send("Received direction " + req.body.direction);
});

// Incorrect route
app.use((req, res, next) => {
    res.status(404).send("404: This route doesn't exist");
});

// Server
const httpServer = http.createServer(app);
const httpsServer = https.createServer(SSL_options, app);

httpServer.listen(HTTP_port, () => {
	console.log(`Listening at URL http://debonair.duckdns.org:${HTTP_port}`);
})

httpsServer.listen(HTTPS_port, () => {
    console.log(`Listening at URL https://debonair.duckdns.org:${HTTPS_port}`);
})