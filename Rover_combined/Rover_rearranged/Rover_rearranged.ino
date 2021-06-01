
#include <Wire.h>
#include <INA219_WE.h>
#include "SPI.h"


INA219_WE ina219; // this is the instantiation of the library for the current sensor

// these pins may be different on different boards
// this is for the uno
#define PIN_SS        10
#define PIN_MISO      12
#define PIN_MOSI      11
#define PIN_SCK       13

#define PIN_MOUSECAM_RESET     8
#define PIN_MOUSECAM_CS        7

#define ADNS3080_PIXELS_X                 30
#define ADNS3080_PIXELS_Y                 30

#define ADNS3080_PRODUCT_ID            0x00
#define ADNS3080_REVISION_ID           0x01
#define ADNS3080_MOTION                0x02
#define ADNS3080_DELTA_X               0x03
#define ADNS3080_DELTA_Y               0x04
#define ADNS3080_SQUAL                 0x05
#define ADNS3080_PIXEL_SUM             0x06
#define ADNS3080_MAXIMUM_PIXEL         0x07
#define ADNS3080_CONFIGURATION_BITS    0x0a
#define ADNS3080_EXTENDED_CONFIG       0x0b
#define ADNS3080_DATA_OUT_LOWER        0x0c
#define ADNS3080_DATA_OUT_UPPER        0x0d
#define ADNS3080_SHUTTER_LOWER         0x0e
#define ADNS3080_SHUTTER_UPPER         0x0f
#define ADNS3080_FRAME_PERIOD_LOWER    0x10
#define ADNS3080_FRAME_PERIOD_UPPER    0x11
#define ADNS3080_MOTION_CLEAR          0x12
#define ADNS3080_FRAME_CAPTURE         0x13
#define ADNS3080_SROM_ENABLE           0x14
#define ADNS3080_FRAME_PERIOD_MAX_BOUND_LOWER      0x19
#define ADNS3080_FRAME_PERIOD_MAX_BOUND_UPPER      0x1a
#define ADNS3080_FRAME_PERIOD_MIN_BOUND_LOWER      0x1b
#define ADNS3080_FRAME_PERIOD_MIN_BOUND_UPPER      0x1c
#define ADNS3080_SHUTTER_MAX_BOUND_LOWER           0x1e
#define ADNS3080_SHUTTER_MAX_BOUND_UPPER           0x1e
#define ADNS3080_SROM_ID               0x1f
#define ADNS3080_OBSERVATION           0x3d
#define ADNS3080_INVERSE_PRODUCT_ID    0x3f
#define ADNS3080_PIXEL_BURST           0x40
#define ADNS3080_MOTION_BURST          0x50
#define ADNS3080_SROM_LOAD             0x60
#define ADNS3080_PRODUCT_ID_VAL        0x17

float vref = 3; // value chosen by me

float open_loop, closed_loop; // Duty Cycles
float vpd,vb,iL,dutyref,current_mA; // Measurement Variables // removed vref from this list
unsigned int sensorValue0,sensorValue1,sensorValue2,sensorValue3;  // ADC sample values declaration
float ev=0,cv=0,ei=0,oc=0; //internal signals
float Ts=0.0008; //1.25 kHz control frequency. It's better to design the control period as integral multiple of switching period.

float kpv=0.05024,kiv=15.78,kdv=0; // voltage pid.
float kpi=0.02512,kii=39.4,kdi=0; // current pid.

float u0v,u1v,delta_uv,e0v,e1v,e2v; // u->output; e->error; 0->this time; 1->last time; 2->last last time
float u0i,u1i,delta_ui,e0i,e1i,e2i; // Internal values for the current controller
float uv_max=4, uv_min=0; //anti-windup limitation
float ui_max=1, ui_min=0; //anti-windup limitation
float current_limit = 1.0;
boolean Boost_mode = 0;
boolean CL_mode = 0;

unsigned int loopTrigger;
unsigned int com_count=0;   // a variables to count the interrupts. Used for program debugging.

//**************************Commhnication variables ****************//
char received_char = 'S';
boolean new_data = false;
//************************** Motor Constants **************************//
   
int DIRRstate = LOW;              //initializing direction states
int DIRLstate = HIGH;

int DIRL = 20;                    //defining left direction pin
int DIRR = 21;                    //defining right direction pin

int pwmr = 5;                     //pin to control right wheel speed using pwm
int pwml = 9;                     //pin to control left wheel speed using pwm

//********************** Camera sensor variables ********************//
// NOTE: int is 2 bytes on this thing
int total_x = 0;
int total_y = 0;
int total_x1 = 0;
int total_y1 = 0;
int x=0;
int y=0;
int a=0;
int b=0;
int distance_x=0;
int distance_y=0;
volatile byte movementflag=0;
volatile int xydat[2];
int tdistance = 0;

//************************ Function declarations *********************//
int convTwosComp(int b);
void mousecam_reset();
int mousecam_init();
void mousecam_write_reg(int reg, int val);
int mousecam_read_reg(int reg);

struct MD
{
 byte motion;
 char dx, dy;
 byte squal;
 word shutter;
 byte max_pix;
};

void mousecam_read_motion(struct MD *p);
int mousecam_frame_capture(byte *pdata);
void sampling(float vref);
float saturation( float sat_input, float uplim, float lowlim);
void pwm_modulate(float pwm_input);
float pidv( float pid_input);
float pidi(float pid_input);

void stop_Rover();
void go_forwards(int command_forwards_des_dist, int sensor_forwards_distance); // -ve values to go back
//void go_backwards(int command_backwards_des_dist, int sensor_backwards_distance);
void turn_90left(unsigned long haltTime);
void turn_90right(); // Update if needed later

//***********************Receiving data part ****************//
  void rec_one_char();
  void show_new_data();

//***************************Globals*******************************
bool halted = 0;
bool done = 0;
bool finished_turning=false;
unsigned long haltTime;

bool ydone1=0;
bool ydone2=0;
//*************************** Setup ****************************//

void setup() {
  
  // Camera Pins Defining: 
  pinMode(PIN_SS,OUTPUT);
  pinMode(PIN_MISO,INPUT);
  pinMode(PIN_MOSI,OUTPUT);
  pinMode(PIN_SCK,OUTPUT);
  
  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV32);
  SPI.setDataMode(SPI_MODE3);
  SPI.setBitOrder(MSBFIRST);
  
  Serial.begin(38400);
  // Communication with ESP32
  Serial1.begin(9600);

  if(mousecam_init()==-1)
  {
    Serial.println("Mouse cam failed to init");
    while(1);
  }

  // Motor Pins Defining: 
  pinMode(DIRR, OUTPUT);
  pinMode(DIRL, OUTPUT);
  pinMode(pwmr, OUTPUT);          //pin to control right wheel speed using pwm - pwmr is pin 5
  pinMode(pwml, OUTPUT);          //pin to control left wheel speed using pwm - pwml is pin 9
  
  digitalWrite(pwmr, HIGH);       //setting right motor speed at maximum
  digitalWrite(pwml, HIGH);       //setting left motor speed at maximum

  // Basic pin setups:
  noInterrupts(); //disable all interrupts
  pinMode(13, OUTPUT);  //Pin13 is used to time the loops of the controller
  pinMode(3, INPUT_PULLUP); //Pin3 is the input from the Buck/Boost switch
  pinMode(2, INPUT_PULLUP); // Pin 2 is the input from the CL/OL switch
  analogReference(EXTERNAL); // We are using an external analogue reference for the ADC

  //*************** TimerA0 initialization for control-loop interrupt.***********//
  
  TCA0.SINGLE.PER = 999; //
  TCA0.SINGLE.CMP1 = 999; //
  TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV16_gc | TCA_SINGLE_ENABLE_bm; //16 prescaler, 1M.
  TCA0.SINGLE.INTCTRL = TCA_SINGLE_CMP1_bm; 
  
  //********************** TimerB0 initialization for PWM output ****************//
  
  pinMode(6, OUTPUT);
  TCB0.CTRLA =TCB_CLKSEL_CLKDIV1_gc | TCB_ENABLE_bm; //62.5kHz
  analogWrite(6,120); 

  interrupts();  //enable interrupts.
  Wire.begin(); // We need this for the i2c comms for the current sensor
  ina219.init(); // this initiates the current sensor
  Wire.setClock(700000); // set the comms speed for i2c
}

//***************************** Loop **************************//

void loop() {
  
  // main code here runs repeatedly:

rec_one_char();
show_new_data();
//******************* Camera loop part: ********************//

#if 0
/*
    if(movementflag){
    tdistance = tdistance + convTwosComp(xydat[0]);
    Serial.println("Distance = " + String(tdistance));
    movementflag=0;
    delay(3);
    }
  */
// if enabled this section grabs frames and outputs them as ascii art  
  if(mousecam_frame_capture(frame)==0)
  {
    int i,j,k;
    for(i=0, k=0; i<ADNS3080_PIXELS_Y; i++) 
    {
      for(j=0; j<ADNS3080_PIXELS_X; j++, k++) 
      {
        Serial.print(asciiart(frame[k]));
        Serial.print(' ');
      }
      Serial.println();
    }
  }
  Serial.println();
  delay(250);
  
  #else
  
  // if enabled this section produces a bar graph of the surface quality that can be used to focus the camera
  // also drawn is the average pixel value 0-63 and the shutter speed and the motion dx,dy.
 
  int val = mousecam_read_reg(ADNS3080_PIXEL_SUM);
  MD md;
  mousecam_read_motion(&md);
  for(int i=0; i<md.squal/4; i++)
    Serial.print('*');
  Serial.print(' ');
  Serial.print((val*100)/351);
  Serial.print(' ');
  Serial.print(md.shutter); Serial.print(" (");
  Serial.print((int)md.dx); Serial.print(',');
  Serial.print((int)md.dy); Serial.println(')');

  // Serial.println(md.max_pix);
  delay(100);
  
    distance_x = md.dx; //convTwosComp(md.dx);
    distance_y = md.dy; //convTwosComp(md.dy);

total_x1 = (total_x1 + distance_x);
total_y1 = (total_y1 + distance_y);

total_x = (float)(total_x1/157.0) * 10; //Conversion from counts per inch to mm (400 counts per inch)
total_y = (float)(total_y1/157.0) * 10; //Conversion from counts per inch to mm (400 counts per inch)
    

Serial.print('\n');
Serial.println("dx (mm) = "+String(distance_x));
Serial.println("dy (mm) = "+String(distance_y));

Serial.println("Distance_x = " + String(total_x));
Serial.println("Distance_y = " + String(total_y));
Serial.print('\n');

  delay(100);

  #endif

//************************** Motor Loop part: ****************************//

  if(loopTrigger) { // This loop is triggered, it wont run unless there is an interrupt
    
    digitalWrite(13, HIGH);   // set pin 13. Pin13 shows the time consumed by each control cycle. It's used for debugging.
    
    // Sample all of the measurements and check which control mode we are in
    sampling(vref);
    CL_mode = digitalRead(3); // input from the OL_CL switch
    Boost_mode = digitalRead(2); // input from the Buck_Boost switch
     
      if (!Boost_mode && CL_mode) { // Closed Loop Buck
          // The closed loop path has a voltage controller cascaded with a current controller. The voltage controller
          // creates a current demand based upon the voltage error. This demand is saturated to give current limiting.
          // The current loop then gives a duty cycle demand based upon the error between demanded current and measured
          // current
          current_limit = 3; // Buck has a higher current limit
          ev = vref - vb;  //voltage error at this time
          cv=pidv(ev);  //voltage pid
          cv=saturation(cv, current_limit, 0); //current demand saturation
          ei=cv-iL; //current error
          closed_loop=pidi(ei);  //current pid
          closed_loop=saturation(closed_loop,0.99,0.01);  //duty_cycle saturation
          pwm_modulate(closed_loop); //pwm modulation - closed_loop is a duty      
      }
    digitalWrite(13, LOW);   // reset pin13.
    loopTrigger = 0;
  }
  
  //************************** Rover Actions **************************//
  
  unsigned long now = millis();
  
  //setting motor movements:

// MODE: DIRECT INPUT FROM USER
//  if(received_char == 'F'){
//    DIRRstate = LOW;   //goes forwards
//    DIRLstate = HIGH;
//    Serial.println(received_char);}
//  else if(received_char == 'B'){
//    DIRRstate = HIGH;   //goes backwards
//    DIRLstate = LOW;
//    Serial.println(received_char);} 
//  else if(received_char == 'L'){
//    DIRRstate = LOW;   // turns left - rotates anticlockwise
//    DIRLstate = LOW;
//    Serial.println(received_char);} 
//  else if(received_char == 'R'){
//    DIRRstate = HIGH;   // turns right - rotates clockwise
//    DIRLstate = HIGH;
//    Serial.println(received_char);}
//  else if(received_char == 'S'){
//    pwm_modulate(0);
//    Serial.println(received_char);} // stops
//  else{
//    pwm_modulate(0);
//    Serial.println("default stop");
//    Serial.println(received_char);} // by default stop 
//
// digitalWrite(DIRR, DIRRstate);
// digitalWrite(DIRL, DIRLstate);  


    // MODE: 
  //Serial.print("Start of movement section, time diff ");
  //Serial.println(now-lastMsg);
  
  //coordinates are provided by the ESP32 from Command
  int target_y = 100;
  int target_x = 300;

// NON è CHE PER CASO LA Y E LA X SI RISETTANO IN BASE ALLA POSIZIONE DELLA TELECAMERA?
   if((ydone1==1) && (ydone2==1)){
      done = 1;
      stop_Rover();
      rover_stopped=1;
      Serial.println("ROVER STOPPED");
    }
  if(!halted){
    if(finished_turning==false){
      Serial.println("going to target y");
      go_forwards(target_y, total_y);
      if(abs(-target_y + total_y) <= 5){
      ydone1=1;
      Serial.println("ydone="); Serial.print(ydone);
      Serial.print("\n");}
    } 
    if(finished_turning==true && ydone1==1 && reached_y2_position==0 && rover_stopped!=1){
      //Serial.println(abs(target_x + total_y));
      Serial.println("going to target x");
      
      // the sensor perceives as the y-direction wherever the front of the rover points.
      // After rotating, to reach target_x, the y position must be accounted since for the rover 
      // the movement happens in the y direction
      
      go_forwards((y_after_rotation + target_x), total_y);
      Serial.println("vuoi uscireeeeee");  
     if(abs(-(y_after_rotation + target_x) + total_y) <= 5){
      Serial.println("è dentroooooo");
      ydone2=1;
      reached_y2_position=1;
      Serial.println("xdone="); Serial.print(xdone);
      }
    }
  }
  
  if(halted && !done)
  {
    if((abs(-(y_after_rotation + target_x) + total_y) <= 5) && ydone==1){
      ydone2=1;
      ydone1=1;
      stop_Rover();
      Serial.print("\n");
      Serial.println("Reached the coordinates while rotating");
    }else{
      Serial.println("rotating");
      // turn 90 to work on other direction
      turn_90left(haltTime);}
  }

// EXPLORE FUNCTIONALITY 
  // receives range of ints. Rotate Rover slowly until the int=0 (probs will need a range for this)
  // if int<0 rotate right
  // if int>0 rotate left
  // when the int=0 , Rover moves forwards (for how much? bool? or fixed distance?)
  // 
   
  /*
  if(now > 20000 && now < 40000){
    go_forwards(-300,total_y);
      }
  if(now > 40000 && now < 45000){
    turn_90left();}
  if(now > 45000 && now < 50000){
    turn_90right();}
  if(now > 50000){
    stop_Rover();}
  //Serial.print("End of movement section, time taken = ");
  //Serial.println(millis()-lastMsg);
  */
  /*
   if (total_y >= 0 && total_y <100) {
      DIRRstate = HIGH;   //goes forwards
      DIRLstate = LOW;}
    if(total_y >= 100){
      pwm_modulate(0);}
   */
   digitalWrite(DIRR, DIRRstate);
   digitalWrite(DIRL, DIRLstate);
 }

//*************************** Loop end ***********************

// Timer A CMP1 interrupt. Every 800us the program enters this interrupt. 
// This, clears the incoming interrupt flag and triggers the main loop.

ISR(TCA0_CMP1_vect){
  TCA0.SINGLE.INTFLAGS |= TCA_SINGLE_CMP1_bm; //clear interrupt flag
  loopTrigger = 1;
}

//************************** Functions ***************************************//

int convTwosComp(int b){
  //Convert from 2's complement
  if(b & 0x80){
    b = -1 * ((b ^ 0xff) + 1);
    }
  return b;
  }

void mousecam_reset(){
  digitalWrite(PIN_MOUSECAM_RESET,HIGH);
  delay(1); // reset pulse >10us
  digitalWrite(PIN_MOUSECAM_RESET,LOW);
  delay(35); // 35ms from reset to functional
}

int mousecam_init(){
  pinMode(PIN_MOUSECAM_RESET,OUTPUT);
  pinMode(PIN_MOUSECAM_CS,OUTPUT);
  
  digitalWrite(PIN_MOUSECAM_CS,HIGH);
  
  mousecam_reset();
  
  int pid = mousecam_read_reg(ADNS3080_PRODUCT_ID);
  if(pid != ADNS3080_PRODUCT_ID_VAL)
    return -1;

  // turn on sensitive mode
  mousecam_write_reg(ADNS3080_CONFIGURATION_BITS, 0x19);

  return 0;
 }

void mousecam_write_reg(int reg, int val){
  digitalWrite(PIN_MOUSECAM_CS, LOW);
  SPI.transfer(reg | 0x80);
  SPI.transfer(val);
  digitalWrite(PIN_MOUSECAM_CS,HIGH);
  delayMicroseconds(50);
}

int mousecam_read_reg(int reg){
  digitalWrite(PIN_MOUSECAM_CS, LOW);
  SPI.transfer(reg);
  delayMicroseconds(75);
  int ret = SPI.transfer(0xff);
  digitalWrite(PIN_MOUSECAM_CS,HIGH); 
  delayMicroseconds(1);
  return ret;
}

void mousecam_read_motion(struct MD *p){
  digitalWrite(PIN_MOUSECAM_CS, LOW);
  SPI.transfer(ADNS3080_MOTION_BURST);
  delayMicroseconds(75);
  p->motion =  SPI.transfer(0xff);
  p->dx =  SPI.transfer(0xff);
  p->dy =  SPI.transfer(0xff);
  p->squal =  SPI.transfer(0xff);
  p->shutter =  SPI.transfer(0xff)<<8;
  p->shutter |=  SPI.transfer(0xff);
  p->max_pix =  SPI.transfer(0xff);
  digitalWrite(PIN_MOUSECAM_CS,HIGH); 
  delayMicroseconds(5);
}

int mousecam_frame_capture(byte *pdata){            // pdata must point to an array of size ADNS3080_PIXELS_X x ADNS3080_PIXELS_Y
  mousecam_write_reg(ADNS3080_FRAME_CAPTURE,0x83);  // you must call mousecam_reset() after this if you want to go back to normal operation
  
  digitalWrite(PIN_MOUSECAM_CS, LOW);
  SPI.transfer(ADNS3080_PIXEL_BURST);
  delayMicroseconds(50);
  
  int pix;
  byte started = 0;
  int count;
  int timeout = 0;
  int ret = 0;
  for(count = 0; count < ADNS3080_PIXELS_X * ADNS3080_PIXELS_Y; ){
    pix = SPI.transfer(0xff);
    delayMicroseconds(10);
    if(started==0){
      if(pix&0x40)
        started = 1;
      else{
        timeout++;
        if(timeout==100){
          ret = -1;
          break;
        }
      }
    }
    if(started==1){
     pdata[count++] = (pix & 0x3f)<<2; // scale to normal grayscale byte range
     }
  }
  digitalWrite(PIN_MOUSECAM_CS,HIGH); 
  delayMicroseconds(14);
  return ret;
}

void sampling(float vref){

  // Make the initial sampling operations for the circuit measurements
  
  sensorValue0 = analogRead(A0); //sample Vb
  sensorValue2 = vref *(1023.0/ 4.096); 
  sensorValue3 = analogRead(A3); //sample Vpd
  current_mA = ina219.getCurrent_mA(); // sample the inductor current (via the sensor chip)
  
  /* Process the values so they are a bit more usable/readable
     The analogRead process gives a value between 0 and 1023 
     representing a voltage between 0 and the analogue reference which is 4.096V
  */
  vb = sensorValue0 * (4.096 / 1023.0); // Convert the Vb sensor reading to volts
  //vref = sensorValue2 * (4.096 / 1023.0); // Convert the Vref sensor reading to volts
  // now vref is set at the top of the code
  vpd = sensorValue3 * (4.096 / 1023.0); // Convert the Vpd sensor reading to volts

 /* The inductor current is in mA from the sensor so we need to convert to amps.
    We want to treat it as an input current in the Boost, so its also inverted
    For open loop control the duty cycle reference is calculated from the sensor
    differently from the Vref, this time scaled between zero and 1.
    The boost duty cycle needs to be saturated with a 0.33 minimum to prevent high output voltages
 */ 
  if (Boost_mode == 1){
    iL = -current_mA/1000.0;
    dutyref = saturation(sensorValue2 * (1.0 / 1023.0),0.99,0.33);
  }else{
    iL = current_mA/1000.0;
    dutyref = sensorValue2 * (1.0 / 1023.0);
  } 
}

float saturation( float sat_input, float uplim, float lowlim){ // Saturation function
  if (sat_input > uplim) sat_input=uplim;
  else if (sat_input < lowlim ) sat_input=lowlim;
  else;
  return sat_input;
}

void pwm_modulate(float pwm_input){ // PWM function
  analogWrite(6,(int)(255-pwm_input*255)); 
}

// This is a PID controller for the voltage
float pidv( float pid_input){
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

// This is a PID controller for the current
float pidi(float pid_input){
  float e_integration;
  e0i = pid_input;
  e_integration=e0i;
  
  //anti-windup
  if(u1i >= ui_max){
    e_integration = 0;
  } else if (u1i <= ui_min) {
    e_integration = 0;
  }
  
  delta_ui = kpi*(e0i-e1i) + kii*Ts*e_integration + kdi/Ts*(e0i-2*e1i+e2i); //incremental PID programming avoids integrations.
  u0i = u1i + delta_ui;  //this time's control output

  //output limitation
  saturation(u0i,ui_max,ui_min);
  
  u1i = u0i; //update last time's control output
  e2i = e1i; //update last last time's error
  e1i = e0i; // update last time's error
  return u0i;
}


void go_forwards(int command_forwards_des_dist, int sensor_forwards_distance){
  int distance_error = sensor_forwards_distance - command_forwards_des_dist;
  halted = 0;
  if(abs(distance_error) < 5 ){
     // stop rover
     pwm_modulate(0);
     halted = 1;
     haltTime = millis();
     Serial.print("\n");
     Serial.print("\n");
     Serial.print("Halting at ");
     Serial.println(haltTime);
     return;
     }
  if(distance_error <= -15){
    // goes forwards
    DIRRstate = LOW;
    DIRLstate = HIGH;
    return;
  }else if(distance_error >-15 && distance_error <0){
    pwm_modulate(0.25);
    //goes forwards
    DIRRstate = LOW;
    DIRLstate = HIGH;
    return;
  }else if(distance_error >= 15){
    // goes backwards
    DIRRstate = HIGH;
    DIRLstate = LOW;
    return;
  }else if(distance_error >0 && distance_error <15){
    pwm_modulate(0.25);
    //goes backwards
    DIRRstate = HIGH;
    DIRLstate = LOW;
    return;
   }  
 }

void turn_90left(unsigned long haltTime){
    unsigned long now = millis();
    if(now-haltTime < 4500){
      DIRRstate = LOW;
      DIRLstate = LOW;
    }
    else{
      Serial.print("We have stopped rotating ");
      Serial.println(now);
      Serial.println(haltTime);
      stop_Rover();
      halted = 0;
      finished_turning=true;
    }
    return; 
}
void turn_90right(){
    DIRRstate = HIGH;
    DIRLstate = HIGH;
    return;
}

void stop_Rover(){
  pwm_modulate(0);
  return;
}

void rec_one_char() {
  if(Serial1.available()){
    received_char = Serial1.read();
    new_data = true;
  }
}

void show_new_data() {
  if(new_data == true) {
    Serial.print("An ");
    Serial.print((byte)received_char);
    Serial.println(" has arrived");
    new_data = false;
  }
}

/*end of the program.*/
