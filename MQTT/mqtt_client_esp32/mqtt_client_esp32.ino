#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h> // Not 100% sure if this included is needed, test without

// Parameters for the wifi connection (will need to change depending on location)
const char* ssid = "iPhonedeYuna";
const char* password = "yuna1612";

// Parameters for the mqtt connection
const char* mqtt_server = "debonair.duckdns.org";
const int mqtt_port = 8883; // Switching to encrypted communication (Change if we ever use a different port)
const char* mqtt_user = "esp32";
const char* mqtt_pwd = "#8HAGxb3*V%+CD8^";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

// LED Pin
const int ledPin = 4;

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void setup() {
  Serial.begin(115200);
  // Will probably also need to setup wiring / pins to communicate? (not sure)
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback); // Sets callback function

  pinMode(ledPin, OUTPUT);
}

// This function is called whenever we receive a message to a topic we are subscribed to
void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  // Add more if statements to control more GPIOs with MQTT (figure out communication with pins)

  // If a message is received on the topic esp32/output, you check if the message is either "on" or "off". 
  // Changes the output state according to the message
  if (String(topic) == "toESP32/output") {
    Serial.print("Changing output to ");
    if(messageTemp == "on"){
      Serial.println("on");
      digitalWrite(ledPin, HIGH);
    }
    else if(messageTemp == "off"){
      Serial.println("off");
      digitalWrite(ledPin, LOW);
    }
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP32Client", mqtt_user, mqtt_pwd)) {
      Serial.println("connected");
      // Subscribe
      client.subscribe("toESP32/#");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Sends test message every 5 seconds
  long now = millis();
  if (now - lastMsg > 5000) {
    lastMsg = now;
    
    char *testString = "test";
    Serial.print("Test string: ");
    Serial.println(testString);
    client.publish("fromESP32/test", testString);
  }
}
