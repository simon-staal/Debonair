#define RXD2 16
#define TXD2 17

const byte num_ints = 32;
int received_ints[num_ints];

boolean new_data = false;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);

}

void loop() {
  // put your main code here, to run repeatedly:
  rec_with_markers();
  show_new_data();
}

void rec_with_markers() {
  static boolean rec_in_progress = false;
  static byte bx = 0;
  int start_marker = 0;
  int end_marker = 99;
  int rc;

  while (Serial2.available() && new_data == false){
    rc = Serial2.read();

    if(rec_in_progress == true){
      if(rc != end_marker){
        received_ints[bx] = rc;
        bx++;
        if(bx >= num_ints){
          bx = num_ints -1;
        }
      }
      else{
        received_ints[bx] = '\0';
        rec_in_progress = false;
        bx = 0;
        new_data = true;
      }
    }
    else if(rc == start_marker){
      rec_in_progress = true;
    }
  }
}

void show_new_data(){
  if(new_data == true){
    Serial.print("Display all values: ");
    for(int i=0; i<=30;i++){
    Serial.println(received_ints[i], DEC);
    new_data = false;
    }
  }
}
