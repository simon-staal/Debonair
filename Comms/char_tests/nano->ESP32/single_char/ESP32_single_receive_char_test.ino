#define RXD2 16
#define TXD2 17

char received_char;
boolean new_data = false;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);


}

void loop() {
  // put your main code here, to run repeatedly:
  rec_one_char();
  show_new_data();

}

void rec_one_char() {
  if(Serial2.available()){
    received_char = Serial2.read();
    new_data = true;
  }
}

void show_new_data() {
  if(new_data == true) {
    Serial.print("An ");
    Serial.print(received_char);
    Serial.println(" has arrived");
    new_data = false;
  }
  
}
