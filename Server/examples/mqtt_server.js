// THIS IS AN EXAMPLE JS FILE SHOWING HOW TO USE MQTT
// For more details check http://www.steves-internet-guide.com/using-node-mqtt-client/

const mqtt = require('mqtt');
const clientOptions = {
	clientId:"mqttjs01",
	username:"webapp",
	password:"=ZCJ=4uzfZZZ#36f",
	rejectUnauthorized : false // I need to do this for it to work
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


//client.subscribe(topic/topic array/topic object, [options], [callback])
/*
client.subscribe(topic, (err, granted) => {
	if (err) {
	 console.log(err);
	 return;
	}
	console.log('Subscribed to topic: ' + topic);
   });
*/

// Triggered when receiving messages, need to be subscribed first
client.on('message', (topic, message, packet) => {
	console.log("message is "+ message);
	console.log("topic is "+ topic);
});

//client.publish(topic, message, [options], [callback]);

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
