void setup() {
  // put your setup code here, to run once:
  Serial1.begin(9600);



}

void loop() {
  // put your main code here, to run repeatedly:
  for(int i = 0; i<=255; i++){
    Serial1.write(i);
    delay(1000);
   }
}
