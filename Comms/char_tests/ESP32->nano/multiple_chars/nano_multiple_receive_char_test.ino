const byte num_chars = 32;
char received_chars[num_chars];

boolean new_data = false;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial1.begin(9600);
  
}

void loop() {
  // put your main code here, to run repeatedly:
  rec_with_markers();
  show_new_data();

}

void rec_with_markers() {
  static boolean rec_in_progress = false;
  static byte bx = 0;
  char start_marker = '<';
  char end_marker = '>';
  char rc;

  while (Serial1.available() && new_data == false){
    rc = Serial1.read();

    if(rec_in_progress == true){
      if(rc != end_marker){
        received_chars[bx] = rc;
        bx++;
        if(bx >= num_chars){
          bx = num_chars -1;
        }
      }
      else{
        received_chars[bx] = '\0';
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
    Serial.print("Where are you...?    ");
    Serial.println(received_chars);
    new_data = false;
  }
}
