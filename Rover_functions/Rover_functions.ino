void setup() {
  // put your setup code here, to run once:
}

void loop() {
  // put your main code here, to run repeatedly:
}

void go_forwards(int command_forwards_des_dist, int sensor_forwards_distance);
void go_forwards_until(bool command_prompt);
void go_backwards(int command_backwards_des_dist, int sensor_backwards_distance);
void turn_90left();
void turn_90right();
void turn_180left();
void turn_180right();
void stop_Rover();

void turn_left_rapid();
void turn_right_rapid();

void go_to_destination(Rover_position, bool Reached_pos);

//note: the optical sensor measures properly distances

// pensa se implementare mezzi angoli per permettere diagonali

// approccio Closed Loop: Mi serve un input dal sensore ed un input dal command
// li confronto e muovo il Rover di conseguenza.

// approccio Open Loop con il tempo della rotazione è meglio per i turn altrimenti 
// in un approccio Closed Loop dovrei calcolare il cerchio immaginario per definire 
// l'angolo della rotazione

// implementa velocità variabile dei motori => rendi il potenziometro obsoleto fissando
// il duty direttamente nel codice
// questo permette di snellire il codice dove puoi rimuovere certe parti del SMPS code
// Si fa rimuovendo la dipendenza dal pin analogico di Vref

// include Vision related code: i dunno like position of ping pong ...

// the only information that i can send back is my current position

//**exploration mode**

// considering desired distance from command module and actual distance measured with the sensor
void go_forwards(float command_forwards_des_dist, float sensor_forwards_distance){
   float sensor_distance= sensor_forwards_distance;
   float distance_error = command_forwards_des_dist - sensor_distance;
    while(distance_error > 0){
     // go forwards
    DIRRstate = HIGH;
    DIRLstate = LOW;
      }
    while(distance_error < 0){
     // go backwards
    DIRRstate = LOW;
    DIRLstate = HIGH;
      }
    if(distance_error == 0){
     // stop rover
     pwm_modulate(0);
     }
    
  }

// while command button is pressed the action is continued
void go_forwards_until(bool command_prompt){
  while(command_prompt == true ){
    DIRRstate = HIGH;
    DIRLstate = LOW;}
  if(command_prompt == false){
    stop_Rover();}
    digitalWrite(DIRR, DIRRstate);
    digitalWrite(DIRL, DIRLstate);
   }

void go_backwards(int command_backwards_des_dist, int sensor_backwards_distance){
    digitalWrite(DIRR, DIRRstate);
    digitalWrite(DIRL, DIRLstate);
    }

void stop_Rover(){
  pwm_modulate(0)}
}




// DA RIELABORARE: 

// probabilmente l'input sono coordinate x e y della posizione desiderata
void go_to_destination(Rover_position, bool Reached_pos){
// Rover_position: input from get_position_Rover, should be a set of x,y coordinates
// Reached_pos: goes high when the desired position has been reached
rov_pos_x= Rover_position(1);
rov_pos_y= Rover_position(2);
desired_pos_y;
desired_pos_x;
diff_y = desired_pos_y - rov_pos_y;
diff_x = desired_pos_x - rov_pos_x;

//compare coordinates               -->> it must be all in a loop until it reaches the wanted position
if(obstacle_detected() == FALSE){
if (diff_y > 0){
  //Rover goes straight       -->> non prende in considerazioni multipli ostacoli sul percorso
  DIRRstate = HIGH;
  DIRLstate = LOW;
  }
if(diff_y < 0){
  //Rover goes backwards
  DIRRstate = LOW;
  DIRLstate = HIGH;
  }
if(diff_x > 0){
  //Rover goes right-clockwise
  DIRRstate = HIGH;
  DIRLstate = HIGH;  
  }
if(diff_x < 0){
  //Rover goes left-anticlockwise
  DIRRstate = LOW;
  DIRLstate = LOW;
  }
}
}
