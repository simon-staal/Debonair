const express = require('express');
const cors = require('cors');
const mqtt = require('mqtt');


const app = new express();
const port = 8080;

// ---------------- Admin shit -------------------
app.use(express.json());
app.use(express.urlencoded({ extended: true }));

const corsOptions = {
    origin: 'http://localhost:3000',
    optionsSuccessStatus: 200
}

app.use(cors(corsOptions)); // Enables CORS for just out REACT APP (both must be running on same server)

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
	publish('toESP32/test','hello',options);
})

// Runs if unable to connect to broker
client.on("error", error => {
	console.log("cannot connect " + error);
	process.exit(1)
});

// You can call this function to publish to things
function publish(topic,msg,options){
	console.log("publishing",msg);
	if (client.connected == true){
		client.publish(topic,msg,options);
		}
}

const options={
	retain:true,
	qos:1
};

// ----------------------- REST API ----------------------
app.get("/",(req,res)=>{
    res.render('index.html');
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
    publish('toESP32/x_coord',receivedCoord.coordinateX,options);
    publish('toESP32/y_coord',receivedCoord.coordinateY,options);
    res.send(receivedCoord);
});

app.post("/move", (req,res) => {
    console.log("Request received: " + JSON.stringify(req.body));
    res.send("Received direction " + req.body.direction);
});

// Incorrect route
app.use((req, res, next) => {
    res.status(404).send("404: This route doesn't exist");
});

// Server
app.listen(port, function(){
    console.log(`Listening on URL http://localhost:${port}`);
})