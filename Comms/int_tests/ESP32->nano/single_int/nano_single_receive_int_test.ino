void setup() {
  // put your setup code here, to run once:
   Serial1.begin(9600);
   Serial.begin(115200);

}

void loop() {
  // put your main code here, to run repeatedly:

  if (Serial1.available()){
    Serial.print("Data received: ");
    int incomingByte = Serial1.read();
    Serial.println(incomingByte, DEC);
  }
}
