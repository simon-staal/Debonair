#define RXD2 16
#define TXD2 17

// Define good start and end markers
int endmarker = 99;
void setup() {
  // put your setup code here, to run once:
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
}

void loop() {
  // put your main code here, to run repeatedly:
  for(int i=0;i<=31;i++){
  Serial2.write(i);
  if(i == 31){
    Serial2.write(endmarker);
  }
  }
}
