void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}

void go_forwards();
void go_backwards();
void turn_left();
void turn_right();

void turn_left_rapid();
void turn_right_rapid();

// approccio Closed Loop: Mi serve un input dal sensore ed un input dal command
// li confronto e muovo il Rover di conseguenza.

// approccio Open Loop con il tempo della rotazione è meglio per i turn altrimenti 
// in un approccio Closed Loop dovrei calcolare il cerchio immaginario per definire l'angolo
// l'angolo della rotazione
