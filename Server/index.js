// Communication
const http = require('http');
const express = require('express');
const cors = require('cors');
const mqtt = require('mqtt');
// Encryption
const https = require('https');
const fs = require('fs')
// Database
const MongoClient = require('mongodb').MongoClient

// ---------------- Admin shit -------------------
// Setting up communication
const app = new express();
const HTTP_port = 8080;
const HTTPS_port = 8443;

// To read JSON packages from frontend
app.use(express.json());
app.use(express.urlencoded({ extended: true }));

// Setup certificates for encrypted communication with front-end (HTTPS)
const cert = fs.readFileSync('/etc/letsencrypt/live/debonair.duckdns.org/fullchain.pem', 'utf8');
const key = fs.readFileSync('/etc/letsencrypt/live/debonair.duckdns.org/privkey.pem', 'utf8');
const SSL_options = {
  key: key,
  cert: cert
};

app.use(cors()); // Enables CORS (required to work with browsers)

// Data storage stuff (database + rover info)
const db_uri = "mongodb+srv://s_staal:LNFfaKDXPekXvb76@cluster0.wmggl.mongodb.net/Debonair?retryWrites=true&w=majority";
const db_client = new MongoClient(db_uri, { useNewUrlParser: true, useUnifiedTopology: true });

var dbo; // Use MongoClient connection pooling to re-use connection to the database
const time = new Date();
// Holds information about our rover
const rover = {
	"x":null,
	"y":null,
	"angle":null,
	"lastUpdate": time.getTime(),
	"status":"offline",
	"battery":null
};

// Database to track the obstacles
db_client.connect((err) => {
	if (err) {
		console.log(err);
		// process.kill(process.pid, 'SIGTERM');
		throw err;
	}
	console.log("MongoDB connected");
	dbo = db_client.db("Debonair");
	const obstacles = dbo.collection("obstacles");
	let initObs = [
		{ colour: 'pink', x: null, y: null },
		{ colour: 'green', x: null, y: null },
		{ colour: 'blue', x: null, y: null },
		{ colour: 'orange', x: null, y: null}
	];
	obstacles.insertMany(initObs, (err, res) => {
		if (err) {
			console.log(err);
			// process.kill(process.pid, 'SIGTERM');
			throw err;
		}
		console.log("Initialised obstacle database");
	});
});

// Accesses the database entry associated with an obstacle
function getObstacle(colour) {
	dbo.collection("obstacles").findOne({colour: `${colour}`}, (err, res) => {
		if (err) {
			console.log(err);
			console.log("Obstacle attempted: " + colour);
			throw err;
			// return {};
		}
		console.log(JSON.stringify(res));
		return res;
	});
}

let newObstacle = 0; // Flag indicating if we have detected a new obstacle

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
		 process.kill(process.pid, 'SIGTERM');
		}
		console.log('Subscribed to topic: ' + topic);
	});
	// Testing publishing ability
	publish('toESP32/test','hello');
})

// Runs if unable to connect to broker
client.on("error", error => {
	console.log("cannot connect " + error);
	process.kill(process.pid, 'SIGTERM');
});

const pubOptions={
	retain:true,
	qos:1
};

// Callback function for when messages are received
client.on('message', (topic, message, packet) => {
	if (topic === "fromeESP32/status") {
		rover.status = message.toString();
	}
	if (topic === "fromESP32/obstacle") {
		// figure out what message will be / how to turn it into something useable
		// Probably a JSON onject as a string
		// i.e. "{ \"c\":\"pink\",\"x\":1450,\"y\":-420 }"
		let msg = JSON.parse(message.toString());
		let query = { colour: msg.c };
		let newCoords = { $set: {x: msg.x, y: msg.y } };
		dbo.collection("obstacles").updateOne(query, newCoords, (err, res) => {
			if (err) {
				console.log(err);
				console.log("Obstacle attempted: " + msg.colour);
				throw err;
				// return;
			}
			newObstacle = 1; // Tells front-end we have new obstacle data
			console.log("Updated " + msg.colour + " to x: " + msg.x + " y: " + msg.y);
		})
	}
	if (topic === "fromESP32/rover_coords") {
		// again probs a JSON object
		// i.e. "{\"x\":0,\"y\":0,\"a\":0}" would be sent from esp32
		let msg = JSON.parse(message.toString());
		rover.x = msg.x;
		rover.y = msg.y;
		rover.angle = msg.a;
		rover.lastUpdate = time.getTime();
		//console.log(JSON.stringify(msg));
		console.log("x: "+rover.x+" y: "+rover.y+" angle: "+rover.angle);
	}
	//console.log("message is "+ message);
	//console.log("topic is "+ topic);
});

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

// Requests rover coordinates
app.get("/coords", (req,res) => {
	let response = {
		'coordinateX': rover.x, //Rover coordinate x
		'coordinateY': rover.y, //Rover coordinate y
		'angle': rover.angle, //Rover angle
		'newObstacle': newObstacle //Any updates to ball positions (1 => new position)
	};
	res.send(response);
});

// Requests obstacle coordinates
app.get("/obstacles", (req,res) => {
	let pink = getObstacle("pink");
	let green = getObstacle("green");
	let blue = getObstacle("blue");
	let orange = getObstacle("orange");
	let response = {
		'pink': [pink.x, pink.y], //pink XY coords
		'green': [green.x, green.y], //green XY coords
		'blue': [blue.x, blue.y], //blue XY coords
		'orange': [orange.x, orange.y] //orange XY coords
	};
	newObstacle = 0; // Resets newObstacle flag
	res.send(response);
});

// Tells backend to reset obstacle coordinates
app.get("/reset", (req,res) => {
	let emptyCoords = {$set: {x: null, y: null} };
	dbo.collection("obstacles").updateMany({}, emptyCoords, (err, response) => {
		if (err) {
			console.log(err);
			res.send("Failure");
		}
		// Potentially check # of rows updated using response.result.nModified
		res.send("Success");
	})
})

// Sends desired coordinates to rover 
// WILL NEED TO UPDATE WITH PATHFINDING
app.post("/coords", (req,res) => {
    console.log("Request received: " + JSON.stringify(req.body));
    
    let receivedCoord = {
        'coordinateX': req.body.coordinateX,
        'coordinateY': req.body.coordinateY
    }
    // res.set('Content-Type', 'text/plain');
    console.log(JSON.stringify(receivedCoord));
	// Not sure about this yet
    publish('toESP32/x_coord',receivedCoord.coordinateX);
    publish('toESP32/y_coord',receivedCoord.coordinateY);
    res.send(receivedCoord);
});

// Sends directions to rover
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
// const httpServer = http.createServer(app);
const httpsServer = https.createServer(SSL_options, app);

// Disabling HTTP, only allowing HTTPS connections
/*
httpServer.listen(HTTP_port, () => {
	console.log(`Listening at URL http://debonair.duckdns.org:${HTTP_port}`);
})
*/
httpsServer.listen(HTTPS_port, () => {
    console.log(`Listening at URL https://debonair.duckdns.org:${HTTPS_port}`);
})

// Handles shutting down application on critical errors
process.on('SIGTERM', () => {
	httpsServer.close(() => {
		console.log('HTTPS server terminated');
	});
	client.end(() => {
		console.log('MQTT client disconnected');
	});
	db_client.close();
	console.log("Disconnected from MongoDB");
})
