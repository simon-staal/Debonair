#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <Wire.h> // Not 100% sure if this included is needed, test without
#include <Math.h> // Needed for distance calculation stuff
#include <SPI.h> // Needed for comms with vision

// **************DEFINITIONS*************

#define VSPI_MISO   MISO
#define VSPI_MOSI   MOSI
#define VSPI_SCLK   SCK
#define VSPI_SS     SS

#define RXD2 16
#define TXD2 17

//***************COMMUNICATION SETUP*************

// Parameters for SPI
SPISettings settings(100000, MSBFIRST, SPI_MODE0);
uint8_t spi_counter[6]; // [0] = c20, [1] = c21, [2] = c22, [3] = c23, [4] = c24, [5] = c25
uint16_t spi_val;
uint8_t spi_reg;
uint16_t spi_returnval;
uint16_t change_char = 0;

void calcDistance(int col);
void resetCounter();

// Parameters for the wifi connection (will need to change depending on location)

const char* ssid = "Khayle"; //"The Circus";
const char* password = "pass1234"; //"Hail_Pietr0";

//const char* ssid = "AndroidAP8029"; //"The Circus";
//const char* password = "hirk8481"; //"Hail_Pietr0";

//const char* ssid = "iPhonedeYuna";
//const char* password = "yuna1612"; 

//const char* ssid = "VM6446132";
//const char* password = "ty5VxqxnW3yr";

// Parameters for the mqtt connection
const char* mqtt_server = "3.8.182.14";
const int mqtt_port = 8883; // Currently using encrypted version
const char* mqtt_user = "esp32";
const char* mqtt_pwd = "#8HAGxb3*V%+CD8^";

const char* ca_cert = \
"-----BEGIN CERTIFICATE-----\n" \
"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\n" \
"KKaH0SCVC/XiMCCdR/dFBKwUKOoEfibqXQ==\n" \
"-----END CERTIFICATE-----\n";

WiFiClientSecure espClient;
PubSubClient client(espClient);
long lastMsg = 0; // Timer for sending coords
char buffer[30]; // buffer for messages sent by ESP32

// LED Pin
const int ledPin = 4;

// Rover Parameters
struct Rover{
  int angle = 0;
  std::pair<int,int> coords = {0,0}; // coords.first = x, coords.second = y
  char mode = 'M';
  char dir = 'S';
};
Rover rover;
std::pair<int,int> dest = {0,0};

// Obstacle Parameters
struct Obstacle{
  int colour; // pink = 1, green = 2, blue = 3, orange = 4
  std::pair<int,int> coords = {0,0};
};
Obstacle obstacle;
bool newObstacle = 0;

//****************Function declarations******************
void setup_wifi(); 
void callback(char* topic, byte* message, unsigned int length); // Called when receiving message from MQTT broker
void reconnect();
void genCoordMsg(char *buf); // Generates message containing rover information as JSON string
void genObsMsg(char *buf); // Generates message containing obstacle information as JSON string

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
  Serial.begin(115200); // Debugging
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2); // Communicate with drive

  // Set up wireless comms
  setup_wifi();
  espClient.setCACert(ca_cert); // Set SSL/TLS certificate
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback); // Sets callback function

  pinMode(ledPin, OUTPUT);

  // Setup SPI stuff
  pinMode(VSPI_SS, OUTPUT);
  SPI.begin();
  resetCounter();
  spi_returnval = 0;
}

// This function is called whenever we receive a message to a topic we are subscribed to
void callback(char* topic, byte* message, unsigned int length) {
  // For debugging, comment this out in production
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
  if (String(topic) == "toESP32/dir" && rover.mode == 'M') { // Single char
    rover.dir = (char)message[0];
    Serial.print("Sending direction ");
    Serial.println(rover.dir);
    Serial2.print(rover.dir);
  }
  else if (String(topic) == "toESP32/mode") { // Single char
    rover.mode = (char)message[0];
    Serial.print("Sending mode ");
    Serial.println(rover.mode);
    if (rover.mode == 'E') {
      Serial2.print('M');
    }
    else {
      Serial2.print(rover.mode);
    }
  }
  // Receives messages of the form "<x_coord,y_coord>"
  // Also not sure if storing the destination coords is necessary, doing it for now
  else if (String(topic) == "toESP32/coord" && rover.mode == 'C') {
    String toDrive;
    toDrive += (char)message[0]; // Adds '<'
    char bufX[6];
    int i = 1;
    while((char)message[i] != ',') {
      bufX[i-1] = message[i];
      toDrive += (char)message[i++];
    }
    toDrive += (char)message[i]; // Adds ','
    bufX[(i++)-1] = '\0';
    String x(bufX);
    dest.first = x.toInt();
    char bufY[6];
    int j = 0;
    while((char)message[i] != '>') {
      bufY[j++] = message[i];
      toDrive += (char)message[i++];
    }
    toDrive += (char)message[i]; // Adds '>'
    Serial2.print(toDrive); // Sends "<x_coord,y_coord>" to drive
    bufY[j++] = '\0';
    String y(bufY);
    dest.second = y.toInt();
    // Debugging
    Serial.print("Sending: ");
    Serial.println(toDrive);
    Serial.print("x coord: ");
    Serial.print(dest.first);
    Serial.print(", y coord: ");
    Serial.println(dest.second);
  }
  else if (String(topic) == "toESP32/output") {
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
  //else if (String(topic) == "toESP32/x_coord")
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTTS connection...");
    // Attempt to connect
    if (client.connect("ESP32Client", mqtt_user, mqtt_pwd)) {
      Serial.println("connected securely");
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

  // ************** UART STUFF *****************
  // Receiving stuff from drive
  int extract = 0;
  if (Serial2.available()) {
    char received_char = Serial2.read();
    if (received_char == '<') {
      String x = Serial2.readStringUntil(',');
      rover.coords.first = x.toInt();
      String y = Serial2.readStringUntil(',');
      rover.coords.second = y.toInt();
      String a = Serial2.readStringUntil('>');
      rover.angle = a.toInt();
      // Serial.println("Received: x = "+x+", y = "+y+", a = "+a);
    }
  }
  // ************** SPI STUFF ******************
  // Only care about vision if we are in exploration mode
  if (rover.mode == 'E') { 
    // Transfer stuff
    SPI.beginTransaction(settings);
    digitalWrite(VSPI_SS, LOW);
    spi_val = SPI.transfer16(spi_returnval);
    spi_returnval = 0;
    digitalWrite(VSPI_SS, HIGH);
    SPI.endTransaction();
    bool charhaschanged = false;
    if (change_char != spi_val){
      change_char = spi_val;
      charhaschanged = true;
    }
    // Processing data received
    if (spi_val == 2048 && charhaschanged == true){
      Serial2.print('R'); // Tells drive to turn right
      //Serial.print("We're rotating right \n");
      resetCounter();
    }
    if (spi_val == 4096 && charhaschanged == true){
      Serial2.print('L'); // Tells drive to turn left
      resetCounter();
    }
    if (spi_val == 8192 && charhaschanged == true){
      Serial2.print('B'); // Tells drive to go backwards
      resetCounter();
    }
    if (spi_val == 16384 && charhaschanged == true){
      Serial2.print('F'); // Tells drive to go forward
      resetCounter();
    }
    if (spi_val > 32768 && charhaschanged == true){
      Serial2.print('S'); // Tells drive to stop
    }
    if (spi_val > 32768){
      spi_val -= 32768;
      spi_val >>= 7;
      spi_reg = spi_val & 7;
      spi_val >>= 3;
      switch(spi_reg)
      {
        case 0:
          //Serial.print("We have a pink ball \n");
          calcDistance(1);
          break;
        case 1:
          //Serial.print("We have a yellow ball \n");
          calcDistance(4);
          break;
        case 2:
          Serial.print("We have a green ball \n");
          calcDistance(2);
          Serial.print("We sent the green ball \n");
          break;
        case 3:
          //Serial.print("We have a blue ball \n");
          calcDistance(3);
          break;
        default:
          Serial.print("Invalid ball detected \n");
      }
    }
  }

  // ************** MQTT STUFF *******************
  if (!client.connected()) {
    reconnect();
  }
  // Allows client to process incoming messages and maintain connection to MQTT broker
  client.loop(); 

  // Handles publishing data about new obstacle to server
  if (newObstacle) {
    genObsMsg(buffer);
    Serial.print("Sending message: ");
    Serial.println(buffer);
    client.publish("fromESP32/obstacle", buffer, false); // Ensure messages arent retained
    newObstacle = 0;
  }

  // Updates server with rover coords
  long now = millis();
  if (now - lastMsg > 200) {
    lastMsg = now;
    genCoordMsg(buffer);
    client.publish("fromESP32/rover_coords", buffer, false);
    Serial.println("Sending " + String(buffer));
  }

  // Sends test message every 2 seconds
 /*
  long now = millis();
  if (now - lastMsg > 10000) {
    lastMsg = now;
    /*
    genCoordMsg(buffer);
    Serial.print("Sending message: ");
    Serial.println(buffer);
    client.publish("fromESP32/rover_coords", buffer, false);
    rover.coords.first = (rover.coords.first + 1)%1000;
    rover.coords.second = (rover.coords.second + 1)%1000;
    rover.angle = (rover.angle + 1)%360;
    obstacle.colour = (obstacle.colour)%4 + 1;
    obstacle.coords.first = (obstacle.coords.first +100)%1000;
    obstacle.coords.second  = (obstacle.coords.second + 100)%1000;
    newObstacle = 1;
    
  }
  */
}

void genCoordMsg(char *buf) 
{
    int cur = 0;
    char x[6] = "{\"x\":";
    for(int i = 0; i < 5; i++){
        buf[cur++] = x[i];
    }
    char num[6];
    sprintf(num, "%d", rover.coords.first);
    int cnt = 0;
    while(num[cnt]){
      buf[cur++] = num[cnt++];
    }
    char y[6] = ",\"y\":";
    for(int i = 0; i < 5; i++){
        buf[cur++] = y[i];
    }
    sprintf(num, "%d", rover.coords.second);
    cnt = 0;
    while(num[cnt]){
      buf[cur++] = num[cnt++];
    }
    char a[6] = ",\"a\":";
    for(int i = 0; i < 5; i++){
        buf[cur++] = a[i];
    }
    sprintf(num, "%d", rover.angle);
    cnt = 0;
    while(num[cnt]){
      buf[cur++] = num[cnt++];
    }
    buf[cur++] = '}';
    while(cur < 30){
      buf[cur++] = '\0';
    }
}

void genObsMsg(char *buf)
{
    int cur = 0;
    char c[6] = "{\"c\":";
    for(int i = 0; i < 5; i++){
        buf[cur++] = c[i];
    }
    buf[cur++] = (char)(obstacle.colour+48);
    char x[6] = ",\"x\":";
    for(int i = 0; i < 5; i++){
        buf[cur++] = x[i];
    }
    char num[6];
    sprintf(num, "%d", obstacle.coords.first);
    int cnt = 0;
    while(num[cnt]){
      buf[cur++] = num[cnt++];
    }
    char y[6] = ",\"y\":";
    for(int i = 0; i < 5; i++){
        buf[cur++] = y[i];
    }
    sprintf(num, "%d", obstacle.coords.second);
    cnt = 0;
    while(num[cnt]){
      buf[cur++] = num[cnt++];
    }
    buf[cur++] = '}';
    while(cur < 30){
      buf[cur++] = '\0';
    }
}

void resetCounter(){
  for(int i = 0; i < 4; i++){
    spi_counter[i] = 0;
  }
}

void calcDistance(int col) 
{
  for(int i = 0; i < 5; i++){
    Serial.println("calculating distance...");
    Serial.println(spi_val);
    if(spi_val == i+26){
      spi_counter[i]++;
      if(spi_counter[i] == 100){
        obstacle.colour = col;
        int x_diff = (i+26.0)*10.0*sin(rover.angle);
        int y_diff = (i+26.0)*10.0*cos(rover.angle);
        obstacle.coords.first = rover.coords.first + x_diff;
        obstacle.coords.second = rover.coords.second + y_diff;
        newObstacle = 1;
        spi_returnval = 32768+col;
        resetCounter();
      }
    }
  }
}
