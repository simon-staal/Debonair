#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <Wire.h> // Not 100% sure if this included is needed, test without

// Parameters for the wifi connection (will need to change depending on location)
const char* ssid = "The Circus";
const char* password = "Hail_Pietr0";

// Parameters for the mqtt connection
const char* mqtt_server = "3.8.182.14";
const int mqtt_port = 1883; // Currently using unencrypted version
const char* mqtt_user = "esp32";
const char* mqtt_pwd = "#8HAGxb3*V%+CD8^";

const char* ca_cert = \
"-----BEGIN CERTIFICATE-----\n" \
"MIIFtTCCA52gAwIBAgIUKXp2SE5JtfQ1LlJf8CI/kPXoqJUwDQYJKoZIhvcNAQEN\n" \
"BQAwajEXMBUGA1UEAwwOQW4gTVFUVCBicm9rZXIxFjAUBgNVBAoMDU93blRyYWNr\n" \
"cy5vcmcxFDASBgNVBAsMC2dlbmVyYXRlLUNBMSEwHwYJKoZIhvcNAQkBFhJub2Jv\n" \
"ZHlAZXhhbXBsZS5uZXQwHhcNMjEwNTI2MjE1MzAyWhcNMzIwNTIzMjE1MzAyWjBq\n" \
"MRcwFQYDVQQDDA5BbiBNUVRUIGJyb2tlcjEWMBQGA1UECgwNT3duVHJhY2tzLm9y\n" \
"ZzEUMBIGA1UECwwLZ2VuZXJhdGUtQ0ExITAfBgkqhkiG9w0BCQEWEm5vYm9keUBl\n" \
"eGFtcGxlLm5ldDCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIBAPCvBJ7K\n" \
"+ldC8Z9+XZB4YIxJWISxYYhtQCa6LjwCbX6LB4sWS6TcXPTdHDSusl6tjs2omBgd\n" \
"P4D/S8Nw+12yB9v6wm5wnuWCYgxZuJVkEmjrnWcwpwNWwvNeCL3hFxlXF/C6LIDK\n" \
"neVAc7n2ug8V3wfgDdyzeIOdxVA/SD8UJK8uMur+g0V2X7Ub9IjWri0Rn9IXEKj+\n" \
"G4gannJjUFAmNcF4BKKvr4E0TfUwbKxKA6MWXuIQbjGTF+HlCGHrtmVD7ol7n2iz\n" \
"2AfcnjPto98bw8Qe/vyJnstSTqr1W0aQ5bdqmKK9B+h3Tnx+xqVptI+FSH/Po6yE\n" \
"WC5jwgNRVcr5PaV9rw75j3xYateFJ4z6oe2lfeg88ZRxTksFv9iqxfWlS7dPys9y\n" \
"/sQtlOJ7vcAxMGzS4ObHplNowkGkDarLqjR15B6Su8/9hv3HKgzvCEXWX4fHKnbr\n" \
"ORArPFNQERq2dRJZeYaGDiew+6+gfgnc8zIf5JiPHjDkR8kfVR8hOsgssZFgnLHk\n" \
"SIldBhSb/gJA2zMHTxgSHgxeXKc/tiF7UAp/PxJAisG65nO3TiAoNohBhulMCsV9\n" \
"hpSE/9hz5SOJfgndE03LzQfwk+Ej1CkbIr8FhW9LdvIqOZM0cYQ+OYuQWbl0P9dw\n" \
"lSiMOzbubpuiHFI8htcPJGKr+WR6azAkTPCPAgMBAAGjUzBRMB0GA1UdDgQWBBS0\n" \
"PjFYC9zywnNtsVbHzVHS4fZD+jAfBgNVHSMEGDAWgBS0PjFYC9zywnNtsVbHzVHS\n" \
"4fZD+jAPBgNVHRMBAf8EBTADAQH/MA0GCSqGSIb3DQEBDQUAA4ICAQC6ONZlzDCZ\n" \
"U2NF2noAWJ5AMWAJfaWN92aHVl4aEGQwyAmaox3V0IZcstUt2qMmbKEoXn9uzn8Q\n" \
"w5q8SpVbg5Z9YL39Xu8BrWz0BpD6qLC4WK4WV2r9it5Um6xamMBmA5pRYZhzqpXn\n" \
"klW7b/3yUST7Bgoj1oPEhlEd+SIEG5Ghc+paKQn50+IskV3ZuP5+nTbgX5A1lEp1\n" \
"YscfVsHvUAa326P4tAFMeU4PgB5OwG9OMiJSXG6ojF5x3goB7CQVXm+JFNDUeajt\n" \
"4JrIbanVF9+LRrjVCfbKCbKTXYdjf7nIeObohkejcprCVbN7cYfGXZ6SvqDS0f7P\n" \
"/z85pJX2Oxi9YzfTZtbw/i/IaWV1bGqcp4JMPQSxRZFHhYCBiF8v/aAJ1sgCCeII\n" \
"sHwjT1GuXqbOpGV9MUpDYegCiy+y6+SMdM0RWwdVMSBhh2+CHWV5W9UK7N4a+8Fd\n" \
"DtkRW3/DGZDny2bbM/m94Tn99Q0T7DpRyBzYEa6ja7txuouaFjM3oeE6OOf1gKT0\n" \
"pBQaZr9c2VznnRB3fvZjXC4ebIqrUcJ0vQlkTaJ+zlbU8Mqwx8foaGl15BKLz97Q\n" \
"sIQsAyPUYHfauREH+RqgPuYdNaVCoPQ8oNtWraifolk7MNU0aXsQEeESV5osUkYx\n" \
"48pa58/edqexF7wUSpjIB45TCvwrbQv+OQ==\n" \
"-----END CERTIFICATE-----\n";

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
  //espClient.setCACert(ca_cert); // Set SSL/TLS certificate
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
