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
// Pathfinding
// TO USE: Pathfinder.genPath("rover_posX,rover_posY","dest_posX,dest_posY","{obstacle1_posx,obstacle1_posY}{obstacle2_posX,obstacle2_posY}{etc}")
const Pathfinder = require('./pathfinding/pathfinding'); 

// Testing 
/*
let i = 0;
let total = 0;
for (i; i < 10000; i++) {
	let start = Date.now();
	let path = JSON.parse(Pathfinder.genPath("0,0","3500,5000","{1000,1450}{2230,3100}{2700,3600}{3000,4450}{3350,4550}"));
	let end = Date.now();
	total += (end-start);
}
console.log("Average time for 10000 iterations: "+(total/i)+"ms");
*/
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

let path; // Holds path for the rover

// ------------------------ Database stuff ----------------------

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

// Holds information about obstacles
const obstacles = {
	"pink":{
		"x":null,
		"y":null
	},
	"green":{
		"x":null,
		"y":null
	},
	"blue":{
		"x":null,
		"y":null
	},
	"orange":{
		"x":null,
		"y":null
	}
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
	obstacles.deleteMany({}, (err, obj) => {
		if (err) throw err;
		console.log("Cleared obstacle collection, removed " + obj.result.n + " entries");
	})
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
function getObstacles() {
	dbo.collection("obstacles").find({}).toArray((err, res) => {
		if (err) {
			console.log(err);
			throw err;
		}
		//console.log(JSON.stringify(res));
		let i = 0;
		for(let col in obstacles){
			obstacles[col].x = res[i].x;
			obstacles[col].y = res[i].y;
			i++;
		}
		//console.log(JSON.stringify(obstacles));
		newObstacle = 1; // Tells front-end we have new obstacle data
	});
}

// Converts code into colour string
function getObsColour(col) {
	switch(col) {
		case 1:
			return "pink";
		case 2:
			return "green";
		case 3:
			return "blue";
		case 4:
			return "orange";
		default:
			console.log("Invalid obstacle colour received from ESP32");
			return "INVALID";
	}
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
	if (topic === "fromESP32/status") {
		rover.status = message.toString();
	}
	if (topic === "fromESP32/obstacle") {
		// JSON object with following fields {c:1, x:0, y:0} corresponding to colour_code, x and y values
		// Colour_code: pink=1, green=2, blue=3, orange=4
		let msg = JSON.parse(message.toString());
		let obsColour = getObsColour(msg.c);
		let query = { colour: obsColour };
		let newCoords = { $set: {x: msg.x, y: msg.y } };
		dbo.collection("obstacles").updateOne(query, newCoords, (err, res) => {
			if (err) {
				console.log(err);
				console.log("Obstacle attempted: " + obsColour);
				throw err;
				// return;
			}
			console.log("Updated " + obsColour + " to x: " + msg.x + " y: " + msg.y);
			getObstacles(); // Updates local obstacles
		})
		
	}
	if (topic === "fromESP32/rover_coords") {
		// JSON object with following fields {x:0, y:0, a:0} corresponding to x, y and angle values
		let msg = JSON.parse(message.toString());
		rover.x = msg.x;
		rover.y = msg.y;
		rover.angle = msg.a;
		rover.lastUpdate = time.getTime();
		//console.log("x: "+rover.x+" y: "+rover.y+" angle: "+rover.angle);
	}
	if (topic === "fromESP32/nextpoint") {
		if (path.points) {
			let point = path.points.shift(); // Extracts first element (next point)
			console.log("Sending point: "+JSON.stringify(point));
			publish('toESP32/coord',`<${point.x},${point.y}>`);
		}
	}
});

// You can call this function to publish to things
function publish(topic,msg,options=pubOptions){
	console.log("publishing",msg);
	if (client.connected == true){
		client.publish(topic,msg,options);
		}
}


// ----------------------- HTTP Request handler ----------------------
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
	newObstacle = newObstacle ? 0 : newObstacle; // Resets newObstacle flag
	res.send(response);
});

// Requests obstacle coordinates
app.get("/obstacles", (req,res) => {
	//console.log(JSON.stringify(obstacles));
	let response = {
		'pink': [obstacles.pink.x, obstacles.pink.y], //pink XY coords
		'green': [obstacles.green.x, obstacles.green.y], //green XY coords
		'blue': [obstacles.blue.x, obstacles.blue.y], //blue XY coords
		'orange': [obstacles.orange.x, obstacles.orange.y] //orange XY coords
	};
	//console.log(JSON.stringify(response));
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
		console.log("Reset "+response.result.nModified+" elements");
		res.send("Success");
	})
})

// Sends desired coordinates to rover 
app.post("/coords", (req,res) => {
    console.log("Request received: " + JSON.stringify(req.body));
    
    let receivedCoord = {
        'coordinateX': req.body.coordinateX,
        'coordinateY': req.body.coordinateY
    }
	let obstacles_string = ""
	for ( let i in obstacles ) {
		if (obstacles[i].x) obstacles_string += `{${obstacles[i].x},${obstacles[i].y}}`;
	}
	path = JSON.parse(Pathfinder.genPath(`${rover.x},${rover.y}`,`${req.body.coordinateX},${req.body.coordinateY}`,obstacles_string));
    console.log(JSON.stringify(path));
	res.send(path);
	let point = path.points.shift();
	console.log("Sending point: "+JSON.stringify(point));
    publish('toESP32/coord',`<${point.x},${point.y}>`);
});

// Sends directions to rover
app.post("/move", (req,res) => {
    console.log("Request received: " + JSON.stringify(req.body));
    publish('toESP32/dir', req.body.direction)
    res.send("Received direction " + req.body.direction);
});

app.post("/mode", (req, res) => {
	publish('toESP32/mode', req.body.mode);
	res.send("Updated mode " + req.body.mode);
})

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
	db_client.close(() => {
		console.log("Disconnected from MongoDB");
	});
})
