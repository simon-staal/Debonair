#include <Wire.h>
#include <INA219_WE.h>
#include <SPI.h>
#include <SD.h>

INA219_WE ina219; // this is the instantiation of the library for the current sensor

// set up variables using the SD utility library functions:
Sd2Card card;
SdVolume volume;
SdFile root;

const int chipSelect = 10;
unsigned int rest_timer;
unsigned int loop_trigger;
unsigned int int_count = 0; // a variables to count the interrupts. Used for program debugging.
float u0i, u1i, delta_ui, e0i, e1i, e2i; // Internal values for the current controller
float uv_max=3.7, uv_min=0; //anti-windup limitation
float ui_max = 1, ui_min = 0; //anti-windup limitation
float kpi = 0.02512, kii = 39.4, kdi = 0; // current pid.
float ev=0,cv=0,ei=0,oc=0; //internal signals
float Ts = 0.001; //1 kHz control frequency.
float current_measure, current_ref = 0, error_amps, current_lim; // Current Control
float pwm_out;
float V_Bat, vref;
float meas_1, temp_meas=0; 
float meas_2;
float diff;
float kpv=0.05024,kiv=15.78,kdv=0; // voltage pid.
float u0v,u1v,delta_uv,e0v,e1v,e2v; // u->output; e->error; 0->this time; 1->last time; 2->last last time
boolean input_switch;
int state_num=0,next_state;
String dataString;

void setup() {
  //Some General Setup Stuff

  Wire.begin(); // We need this for the i2c comms for the current sensor
  Wire.setClock(700000); // set the comms speed for i2c
  ina219.init(); // this initiates the current sensor
  Serial.begin(9600); // USB Communications


  //Check for the SD Card
  Serial.println("\nInitializing SD card...");
  if (!SD.begin(chipSelect)) {
    Serial.println("* is a card inserted?");
    while (true) {} //It will stick here FOREVER if no SD is in on boot
  } else {
    Serial.println("Wiring is correct and a card is present.");
  }

  if (SD.exists("BatCycle.csv")) { // Wipe the datalog when starting
    SD.remove("BatCycle.csv");
  }

  
  noInterrupts(); //disable all interrupts
  analogReference(EXTERNAL); // We are using an external analogue reference for the ADC

  //SMPS Pins
  pinMode(13, OUTPUT); // Using the LED on Pin D13 to indicate status
  pinMode(2, INPUT_PULLUP); // Pin 2 is the input from the CL/OL switch
  pinMode(6, OUTPUT); // This is the PWM Pin

  //LEDs on pin 7 and 8
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);

  //output CONTROL for relays or discharges
  pinMode (3, OUTPUT);// dis for cell 1 
  pinMode (4, OUTPUT); // dis for cell 2
  pinMode (5, OUTPUT);// relay for cell 1
  pinMode (9, OUTPUT); //relay for cell 2
  

  //Analogue input, the battery voltage (also port B voltage)
  pinMode(A0, INPUT);
  
  //BATTERY VOLTAGES
  pinMode(A1, INPUT); //cell 1
  pinMode(A2, INPUT);//cell 2
  
  // TimerA0 initialization for 1kHz control-loop interrupt.
  TCA0.SINGLE.PER = 999; //
  TCA0.SINGLE.CMP1 = 999; //
  TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV16_gc | TCA_SINGLE_ENABLE_bm; //16 prescaler, 1M.
  TCA0.SINGLE.INTCTRL = TCA_SINGLE_CMP1_bm;

  // TimerB0 initialization for PWM output
  TCB0.CTRLA = TCB_CLKSEL_CLKDIV1_gc | TCB_ENABLE_bm; //62.5kHz

  interrupts();  //enable interrupts.
  analogWrite(6, 120); //just a default state to start with

}

void loop() {
  if (loop_trigger == 1){ // FAST LOOP (1kHZ)
      state_num = next_state; //state transition
      V_Bat = analogRead(A0)*4.096/1.03; //check the battery voltage (1.03 is a correction for measurement error, you need to check this works for you)
      if ((V_Bat > 3700 || V_Bat < 2400)) { //Checking for Error states (just battery voltage for now)
          state_num = 10; //go directly to jail
          next_state = 10; // stay in jail
          digitalWrite(7,true); //turn on the red LED
          current_ref = 0; // no current
      }

      if (state_num==0 || state_num==1 ||state_num==2 ||state_num==3 ||state_num==4 ||state_num==6 ||state_num==7 ||state_num==8||state_num==9||state_num==10){
      current_measure = (ina219.getCurrent_mA()); // sample the inductor current (via the sensor chip)
      error_amps = (current_ref - current_measure) / 1000; //PID error calculation
      pwm_out = pidi(error_amps); //Perform the PID controller calculation
      pwm_out = saturation(pwm_out, 0.99, 0.01); //duty_cycle saturation
      analogWrite(6, (int)(255 - pwm_out * 255)); // write it out (inverting for the Buck here)
      }
      
      if(state_num==5){
        ev = vref - V_Bat;  //voltage error at this time
        cv=pidv(ev);  //voltage pid
        current_lim=0.6;
        cv=saturation(cv, current_lim, 0); //current demand saturation
        ei=cv-(current_measure/1000); //current error
        pwm_out=pidi(ei);  //current pid
        pwm_out=saturation(pwm_out,0.99,0.01);  //duty_cycle saturation
        analogWrite(6, (int)(255 - pwm_out * 255));
        }
      int_count++; //count how many interrupts since this was last reset to zero
      loop_trigger = 0; //reset the trigger and move on with life
  }
  
  if (int_count == 1000) { // SLOW LOOP (1Hz)
    input_switch = digitalRead(2); //get the OL/CL switch status
    
    switch (state_num) { // STATE MACHINE (see diagram)
      
      case 0:{ // Start state (no current, no LEDs)
        current_ref = 0;
        if (input_switch == 1) { // if switch, move to charge
          next_state = 1;
          digitalWrite(8,true);
        } else { // otherwise stay put
          next_state = 0;
          digitalWrite(8,false);
        }
        break;
      }
      case 1:{ // Checking volt in cell 1
        current_ref = 0;
        if (V_Bat > 3450) { // if 3.45V go to CV
          next_state = 5;
          digitalWrite(8,true);          
        } 
        if(rest_timer < 5) { // measure for 5s
          digitalWrite(5, true);
          temp_meas=temp_meas + (analogRead(A1)*4.096/1.03);
          next_state = 1;
          rest_timer++;
          digitalWrite(8,true);
        }
        if (rest_timer >=5 ){
          digitalWrite(5, false);
          meas_1= temp_meas/5;
          next_state = 2;
          rest_timer = 0;
          temp_meas=0;
          }
        if(input_switch == 0){ // UNLESS the switch = 0, then go back to start
          next_state = 0;
          digitalWrite(8,false);
        }
        break;
      }
      case 2:{ // check volts in cell 2
          current_ref = 0;
        if (V_Bat > 3450) { // if 3.45V go to CV
          next_state = 5;
          digitalWrite(8,true);          
        } 
        if(rest_timer < 5) { // in measure for 5s
          digitalWrite(9, true);
          temp_meas=temp_meas + (analogRead(A2)*4.096/1.03);
          next_state = 2;
          rest_timer++;
          digitalWrite(8,false);
        }
        if (rest_timer >=5 ){
          digitalWrite(9, false);
          meas_2=temp_meas/5;
          next_state = 3;
          rest_timer = 0;
          temp_meas=0;
          }
        if(input_switch == 0){ // UNLESS the switch = 0, then go back to start
          next_state = 0;
          digitalWrite(8,false);
        }
        break;
      }
      case 3:{ // comparison state
         diff = abs(meas_1-meas_2);
         current_ref = 0;
         if (diff <=200){ // if difference is small then charge equally
           next_state = 4; 
         } 
         else if (( diff > 200 )&&(meas_1 > meas_2)) { // if cell one is too large then discharge it
           next_state =4;
           digitalWrite(3, true);
           digitalWrite(4, false);
         }
         else if (( diff > 200 )&&(meas_1 < meas_2)) { // If cell 2 is too large then discharge it
           next_state =4;
           digitalWrite(4, true);
           digitalWrite(3,false);
         }
         if(input_switch == 0){ //UNLESS the switch = 0, then go back to start
          next_state = 0;
          digitalWrite(8,false);
        }
        break;
      }
      case 4:{ // constant current state
        current_ref = 500;
        
        if (rest_timer < 40) { // charge for 40s
          next_state = 4;
          rest_timer++;
        } else if(rest_timer == 40) { // When thats done, move back to checking states
          next_state = 1;
          digitalWrite(8,true);
          rest_timer = 0;
        }
        if (V_Bat >= 3450){// if vbat is large enough then go to cv charging
          next_state= 5; 
          }
        if(input_switch == 0){ //UNLESS the switch = 0, then go back to start
          next_state = 0;
          digitalWrite(8,false);
        }
        break;
      }
      
      case 5:{ // constant voltage charging
        vref=3550;
        
        if (rest_timer < 40) { // chare for 40s
          next_state = 5;
          digitalWrite(8,false);
          rest_timer++;
        } 
        
        else if(rest_timer == 40) { // When thats done, move back to checking states
          next_state = 6;
          digitalWrite(8,true);
          rest_timer = 0;
        }
        else if (V_Bat > 3550){
          next_state= 9; 
          }
        if(input_switch == 0){ //UNLESS the switch = 0, then go back to start
          next_state = 0;
          digitalWrite(8,false);
        }
        break;
      }
        case 6:{ // Checking volt in cell 1
        current_ref = 0;
        if (V_Bat > 3510) { // if 3.55V go to float
          next_state = 9;
             
        } if(rest_timer < 5) { // in measure for 5s
          digitalWrite(5, true);
          temp_meas=temp_meas + (analogRead(A1)*4.096/1.03);
          next_state = 6;
          rest_timer++;
        }
        if (rest_timer >=5 ){
          digitalWrite(5, false);
          meas_1=temp_meas/5;
          next_state = 7;
          rest_timer = 0;
          temp_meas=0;
          }
        if(input_switch == 0){ // UNLESS the switch = 0, then go back to start
          next_state = 0;
          digitalWrite(8,false);
        }
        break;
      }
      case 7:{ // check volts in cell 2
          current_ref = 0;
        if (V_Bat > 3550) { // if 3.55V go to CV
          next_state = 5;         
        } 
        if(rest_timer < 5) { // in measure for 5s
          digitalWrite(9, true);
          temp_meas= temp_meas + analogRead(A2)*4.096/1.03;
          next_state = 7;
          rest_timer++;
        }
        if (rest_timer >=5 ){
          digitalWrite(9, false);
          meas_2=temp_meas/5;
          next_state = 8;
          rest_timer = 0;
          temp_meas=0;
          }
        if(input_switch == 0){ // UNLESS the switch = 0, then go back to start
          next_state = 0;
          digitalWrite(8,false);
        }
        break;
      }
      case 8:{ // comparison state
         diff = abs(meas_1-meas_2);
         current_ref = 0;
         if (diff <=200){ // if difference is small then charge equally
           next_state = 5;
           digitalWrite(8,false);
         } else if (( diff > 200 )&&(meas_1 > meas_2)) { // if cell one is too large then discharge it
           next_state= 5;
           digitalWrite(3, true);
           digitalWrite(4,false);
         }
         else if (( diff > 200 )&&(meas_1 < meas_2)) { // If cell 2 is too large then discharge it
           next_state =5;
           digitalWrite(4, true);
           digitalWrite(3,false);
         }
         else if(meas_1>3550 && meas_2>3550){
          next_state=9;
          }
         if(input_switch == 0){ //UNLESS the switch = 0, then go back to start
          next_state = 0;
          digitalWrite(8,false);
        }
        break;
      }
      case 9:{ // rest stage
        current_ref = 0;
        
        digitalWrite(7,true);
        digitalWrite(8,true);
        
        if(input_switch == 0){ //UNLESS the switch = 0, then go back to start
          next_state = 0;
          digitalWrite(8,false);
        }
        break;
      }
      case 10: { // ERROR state RED led and no current
        current_ref = 0;
        next_state = 5; // Always stay here
        digitalWrite(7,true);
        digitalWrite(8,false);
        if(input_switch == 0){ //UNLESS the switch = 0, then go back to start
          next_state = 0;
          digitalWrite(7,false);
        }
        break;
      }
      
      default :{ // Should not end up here ....
        Serial.println("Boop");
        current_ref = 0;
        next_state = 5; // So if we are here, we go to error
        digitalWrite(7,true);
      }
      
    }
    
    dataString = String(state_num) + "," + String(V_Bat) + "," + String(current_ref) + "," + String(current_measure)+","+String(meas_1)+","+String(meas_2)+","+String(diff); //build a datastring for the CSV file
    Serial.println(dataString); // send it to serial as well in case a computer is connected
    File dataFile = SD.open("BatCycle.csv", FILE_WRITE); // open our CSV file
    if (dataFile){ //If we succeeded (usually this fails if the SD card is out)
      dataFile.println(dataString); // print the data
    } else {
      Serial.println("File not open"); //otherwise print an error
    }
    dataFile.close(); // close the file
    int_count = 0; // reset the interrupt count so we dont come back here for 1000ms
  }
}

// Timer A CMP1 interrupt. Every 1000us the program enters this interrupt. This is the fast 1kHz loop
ISR(TCA0_CMP1_vect) {
  loop_trigger = 1; //trigger the loop when we are back in normal flow
  TCA0.SINGLE.INTFLAGS |= TCA_SINGLE_CMP1_bm; //clear interrupt flag
}

float saturation( float sat_input, float uplim, float lowlim) { // Saturation function
  if (sat_input > uplim) sat_input = uplim;
  else if (sat_input < lowlim ) sat_input = lowlim;
  else;
  return sat_input;
}

float pidv( float pid_input){ // controller for voltage
  float e_integration;
  e0v = pid_input;
  e_integration = e0v;
 
  //anti-windup, if last-time pid output reaches the limitation, this time there won't be any intergrations.
  if(u1v >= uv_max) {
    e_integration = 0;
  } else if (u1v <= uv_min) {
    e_integration = 0;
  }

  delta_uv = kpv*(e0v-e1v) + kiv*Ts*e_integration + kdv/Ts*(e0v-2*e1v+e2v); //incremental PID programming avoids integrations.there is another PID program called positional PID.
  u0v = u1v + delta_uv;  //this time's control output

  //output limitation
  saturation(u0v,uv_max,uv_min);
  
  u1v = u0v; //update last time's control output
  e2v = e1v; //update last last time's error
  e1v = e0v; // update last time's error
  return u0v;
}

float pidi(float pid_input) { // discrete PID function
  float e_integration;
  e0i = pid_input;
  e_integration = e0i;

  //anti-windup
  if (u1i >= ui_max) {
    e_integration = 0;
  } else if (u1i <= ui_min) {
    e_integration = 0;
  }

  delta_ui = kpi * (e0i - e1i) + kii * Ts * e_integration + kdi / Ts * (e0i - 2 * e1i + e2i); //incremental PID programming avoids integrations.
  u0i = u1i + delta_ui;  //this time's control output

  //output limitation
  saturation(u0i, ui_max, ui_min);

  u1i = u0i; //update last time's control output
  e2i = e1i; //update last last time's error
  e1i = e0i; // update last time's error
  return u0i;
}
