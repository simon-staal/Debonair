
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

float vref; //= 1.5; // value chosen by me

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

//************************* PI variables **************************//

float kpDistance = 0.25, kiDistance = 0.58; // coefficient of PI controller
float u0Distance, u1Distance, delta_uDistance, e0Distance, e1Distance, e2Distance;
float cDistance;
float error_y;
float error_angle;

//**************************Communication variables ****************//
char received_char = 'S';
boolean new_data = false;
char change_char = 'S';
char mode = 'M';
char dir = 'S';
int destinationX = 0;
int destinationY = 0;
bool haschanged = false;
char buf[18]; //message to be sent to ESP32
long lastMsg = 0;

//************************** Rover Constants / Variables ************************//
  //Measured diameter of Rover complete rotation wrt pivot point positioned on wheel axis: 260 mm
const float pi = 3.14159;
float r = 140.0;
float C = 2*pi*r;
float arc_length;
float increment;
float x1 = 0;
float y1 = 0;

//*********************** Angle variable *****************************//
bool angle_flag = false;
bool target_flag = false;
float O_to_coord_measured;
float O_to_coord;
float angle = 0;
float beta = 0;
float gamma = 0;
int coord_after_rotation;
float dummy_angle=0;


float alpha = 0;
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

float dx_mm = 0;
float dy_mm = 0;
int counter = 0;
//***************************Globals*******************************
bool halted = 0;                  // in simple coordinate mode with timed 90°
bool done = 0;
bool finished_turning=false;
unsigned long haltTime;

bool rover_stopped;
bool reached_x_position=0;
bool ydone1=0;
bool xdone=0;
int y_after_rotation;

bool stopped_rover = false;
bool destination_reached; // in COORDINATE mode

long anglechanged = 0;
long sumchanged = 0;
float target;
float sensor;
float sum_dist = 0;
float current_angle = 0;
int current_x = 0;
int current_y = 0;

float coord_anglechanged = 0;
float coord_sumchanged = 0;
float alphaSummed = 0;
float angle_now = 0;
int x_now = 0;
int y_now = 0;
long sumdist = 0;

float measuredDistance = 0;
float Distance = 0;
float alpha2 = 0;
float previous_O_to_coord = 0;
bool reached_angle = 0;
bool new_coordinates = 1;

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
void sampling();
float saturation( float sat_input, float uplim, float lowlim);
void pwm_modulate(float pwm_input);
float pidv( float pid_input);
float pidi(float pid_input);

void stop_Rover();
void go_forwards(int command_forwards_des_dist, int sensor_forwards_distance); // -ve values to go back
//void go_backwards(int command_backwards_des_dist, int sensor_backwards_distance);
void turn_90left(unsigned long haltTime);
void turn_90right(); // Update if needed later
void angle_90_timedLeft();
void angle_90_timedRight(unsigned long startTime);
void compensate_x(float);
float toDegrees(float angleRadians);
void goLeft();
void goRight();
void goBackwards();
void goForwards();
float pidDistance(float pidDistance_input);
void F_B_PIcontroller(int sensor_total_y, int target);
void Turn_PIcontroller(float desired_angle, float measuring_angle);
void sendcoords(int x_coord_send, int y_coord_send, float angle_send);
void sendflag();

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
//******************** Communication part: ****************
if (Serial1.available()) {
  received_char = Serial1.read();
  // Updates mode (C = coordinate, M = controlled by ESP)
  if (received_char == 'C' || received_char == 'M') {
    mode = received_char;
  }
  // If in controlled mode, updates direction
  if (mode == 'M') {
    dir = received_char;
  }
  // If in coordinate mode, updates destination
  if (mode == 'C' && received_char == '<') {
    while (Serial1.available() && received_char != '>') {
      received_char = Serial1.read();
      char bufX[6];
      char bufY[6];
      int i = 0;
      while (Serial1.available() && received_char != ',') {
        bufX[i++] = received_char;
        received_char = Serial1.read();
      }
      bufX[i] = '\0';
      String x(bufX);
      destinationX = x.toInt();
      
      if (Serial1.available()) received_char = Serial1.read();
      i = 0;
      while (Serial1.available() && received_char != '>') {
        bufY[i++] = received_char;
        received_char = Serial1.read();
      }
      bufY[i] = '\0';
      String y(bufY);
      destinationY = y.toInt();
    }
  }
}

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

//defined by me
 dx_mm = (float)(distance_x/157.0) * 10;
 dy_mm = (float)(distance_y/157.0) * 10;

Serial.print('\n');
Serial.println("dx = "+String(distance_x));
Serial.println("dy = "+String(distance_y));

Serial.println("Distance_x = " + String(total_x));
Serial.println("Distance_y = " + String(total_y));
Serial.print('\n');
Serial.println("dx (mm) = "+String(dx_mm));
Serial.println("dy (mm) = "+String(dy_mm));


  delay(100);

  #endif

//************************** Motor Loop part: ****************************//
if (loopTrigger) { // This loop is triggered, it wont run unless there is an interrupt

    digitalWrite(13, HIGH);   // set pin 13. Pin13 shows the time consumed by each control cycle. It's used for debugging.

    // Sample all of the measurements and check which control mode we are in
    sampling();
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

 
  //************************** Rover Modes of Operation **************************//
  
  //unsigned long now = millis();

// REMOTE CONTROLLER MODE: DIRECT INPUT FROM USER 
// AND
// EXPLORE MODE - like REMOTE CONTROLLER BUT THE COMMANDS COME FROM VISION
//make a register that only changes if the received character becomes different (call it change_char)

Serial.println("mode = "+ String(mode));
 if(mode == 'M'){
   digitalWrite(pwmr, HIGH);       //setting right motor speed at maximum
   digitalWrite(pwml, HIGH);       //setting left motor speed at maximum
 
  if(dir == 'F'){
    DIRRstate = LOW;   //goes forwards
    DIRLstate = HIGH;
    
    Serial.println(dir);
    sumchanged += dy_mm;
    //anglechanged = 0;
    //accumulate the distance
    Serial.println("sumchanged Forwards"+ String(sumchanged));
    }
  else if(dir == 'B'){
    DIRRstate = HIGH;   //goes backwards
    DIRLstate = LOW;
    
    Serial.println(dir);
    sumchanged -= dy_mm;      // changed to subtraction cause Backwards
    //anglechanged = 0;
    Serial.println("sumchanged Backwards"+ String(sumchanged));
    }
  else if(dir == 'L'){
    DIRRstate = LOW;   // turns left - rotates anticlockwise
    DIRLstate = LOW;
    Serial.println(dir);
    
    O_to_coord_measured = sqrt(pow(dy_mm,2) + pow(dx_mm,2));
    alpha = toDegrees(asin(O_to_coord_measured/(2*r))) * 4 ; 
    anglechanged = (anglechanged + alpha);
    //sumchanged = 0;

    // send angle to Control
    
    Serial.println("alpha in Left rotation"+ String(alpha));
    Serial.println("anglechanged in Left rotation"+ String(anglechanged));
    }
  else if(received_char == 'R'){
    DIRRstate = HIGH;   // turns right - rotates clockwise
    DIRLstate = HIGH;
    Serial.println(dir);
    
    O_to_coord_measured = sqrt(pow(dy_mm,2) + pow(dx_mm,2));
    alpha = toDegrees(asin(O_to_coord_measured/(2*r))) * 4 ; 
    anglechanged = (anglechanged + alpha);
    //sumchanged = 0;

    // send angle to Control
    
    Serial.println("alpha in Right rotation"+ String(alpha));
    Serial.println("anglechanged in Right rotation"+ String(anglechanged));
    }
  else if(dir == 'S'){
    pwm_modulate(0);
    //stop_Rover();
    //anglechanged = 0;
    //sumchanged = 0;
    Serial.println(dir);
    Serial.println("current_x in S = "+ String(current_x));
    Serial.println("current_y in S = "+ String(current_y));
    Serial.println("current_angle in S = "+ String(current_angle));
    } // stops
  else{
    pwm_modulate(0);
    //stop_Rover();
    Serial.println("default stop");
    Serial.println(dir);} // by default stop 

 digitalWrite(DIRR, DIRRstate);
 digitalWrite(DIRL, DIRLstate); 
 } //MODE BRACKET

//**************************************************************************
// COORDINATE MODE: REACHING SET OF COORDINATES SET BY THE USER
int target_x = 0;
int target_y = 0;

if(mode == 'C'){
  new_coordinates = 1;
  stop_Rover();
 Serial.println("new_coordinates = " + String(new_coordinates));
  if(new_coordinates == true){
   target_x = destinationX;
   target_y = destinationY;
   new_coordinates = false;
   Serial.println("New Coordinates set");
   Serial.println("new_coordinates = " + String(new_coordinates));
   }
//delay (1000);
 Serial.println("target_x = " + String(target_x));
 Serial.println("target_y = " + String(target_y));
  // Coordinates are provided by the ESP32 from Command
  // here they are just manually set - for now
  
  //int target_y = 100;
  //int target_x = 200;

  if(target_y == 0 && target_x == 0){
      stop_Rover();}

    O_to_coord = sqrt(pow(target_y,2) + pow(target_x,2));
    Distance = sqrt(pow(target_y-y_now,2) + pow(target_x-x_now,2));
    measuredDistance = sqrt(pow(dy_mm,2) + pow(dx_mm,2));
    alpha2 = toDegrees(asin(measuredDistance/(2*r))) * 4 ; 
    alphaSummed = (alphaSummed + alpha2);
    angle = alphaSummed;


    Serial.println("Distance = " + String(Distance));
    Serial.println("x_now = " + String(x_now));
    Serial.println("y_now = " + String(y_now));

    if(x_now == 0 && y_now == 0){
      beta = 90 - toDegrees((acos(target_x/O_to_coord))); // I need the complementary angle.
    }else{
      gamma = acos((pow(Distance,2) + pow(previous_O_to_coord,2) - pow(O_to_coord,2))/(2 * O_to_coord * previous_O_to_coord));
      beta = 180 - toDegrees(gamma);
      }


  if(angle_flag == true && halted){ //&& target_flag == true){   // if both the right angle and right distance have been achieved 
     stop_Rover();
     //pwm_modulate(0);
     alphaSummed = 0;
     stopped_rover = true;
     //x_now = target_x; //saving coordinates before receiving a new pair of coordinates
     //y_now = target_y;
     x_now = O_to_coord_measured*sin(dummy_angle);
     y_now = O_to_coord_measured*cos(dummy_angle);
     angle_now = dummy_angle;  //saving dummy_angle before computing new one
     previous_O_to_coord = O_to_coord;
     // total_y = y_now;
     // total_x = x_now;
     //target_x = 0;
     //target_y = 0;
     
    //SEND angle_now, x_now ang y_now TO ESP32 - Control 
     sendcoords(x_now, y_now, angle_now);     
     
     new_coordinates = true;
     angle_flag = false;
     halted = false;
     
     // tell Control that destination is reached -> code lower down
     
     Serial.println("Rover has reached the Destination coordinates YAY!");
     Serial.println("x_now = " + String(x_now));
     Serial.println("y_now = " + String(y_now));
     Serial.println("angle_now = " + String(angle_now));

  }else if(((-alphaSummed + beta) <= 2)&& (angle_flag == false)){
     coord_after_rotation = total_y;
     dummy_angle = angle; // angle of the Rover when destination is reached
     Serial.println("dummy_angle = " + String(dummy_angle));
     
     stop_Rover(); Serial.println("FINISHED ROTATING");
     angle_flag = true;
  }
    
  if(((-alphaSummed+beta) >= 2)&& angle_flag == false && target_flag == false){
     Serial.println("inside the rotation loop ");
     Serial.println("alphaSummed = " + String(alphaSummed)); 
     Serial.println("beta = " + String(beta));

     if(x_now == 0 && y_now == 0){    //for the case when starting from origin
      if((target_x > x_now)){   
        Serial.println("target_x has a greater x coord. than the current x coord.");
        // pwm_modulate(0.25);
        //goRight();
        DIRRstate = HIGH;
        DIRLstate = HIGH;   
        digitalWrite(pwmr, HIGH);       //setting right motor speed at maximum
        digitalWrite(pwml, HIGH);       //setting left motor speed at maximum
      }else if((target_x < x_now)){
        Serial.println("target_x has a smaller x coord. than the current x coord.");
        //pwm_modulate(0.25);
        //goLeft(); 
        DIRRstate = LOW;
        DIRLstate = LOW; 
        digitalWrite(pwmr, HIGH);       //setting right motor speed at maximum
        digitalWrite(pwml, HIGH);       //setting left motor speed at maximum 
      }else if ((target_x == x_now)){
         //do not rotate - no angle difference between current coordinates and destination coordinates
         Serial.println("target_x has the same x coord. than the current x coord.");
         goForwards();
         sumdist = sumdist + dy_mm;
       }
     }else{   // for all other coordinates in the 4 quadrants
      if((target_x > x_now) && target_x > 0 && x_now > 0){   
        Serial.println("target_x has a greater x coord. than the current x coord.");
        // pwm_modulate(0.25);
        if(target_y > y_now){
          //goLeft();
          DIRRstate = LOW;
          DIRLstate = LOW;
          digitalWrite(pwmr, HIGH);       //setting right motor speed at maximum
          digitalWrite(pwml, HIGH);       //setting left motor speed at maximum
        }else if(target_y <= y_now){
          //goRight();
          DIRRstate = HIGH;
          DIRLstate = HIGH;
          digitalWrite(pwmr, HIGH);       //setting right motor speed at maximum
          digitalWrite(pwml, HIGH);       //setting left motor speed at maximum
        }    
      }else if((target_x < x_now) && target_x < 0 && x_now < 0 ){
        Serial.println("target_x has a smaller x coord. than the current x coord.");
        //pwm_modulate(0.25);
        if(target_y > y_now){
          //goRight();
          DIRRstate = HIGH;
          DIRLstate = HIGH;
          digitalWrite(pwmr, HIGH);       //setting right motor speed at maximum
          digitalWrite(pwml, HIGH);       //setting left motor speed at maximum
        }else if(target_y <= y_now){
          //goLeft();
          DIRRstate = LOW;
          DIRLstate = LOW;
          digitalWrite(pwmr, HIGH);       //setting right motor speed at maximum
          digitalWrite(pwml, HIGH);       //setting left motor speed at maximum
        }   
      }else if ((target_x == x_now)){
        //do not rotate - no angle difference between current coordinates and destination coordinates
        Serial.println("target_x has the same x coord. than the current x coord.");
        goForwards();
        sumdist = sumdist + dy_mm;
      }else{
        Serial.println("Invalid coordinate has been provided given the path finding algorithm");
        }
     }  
  }  
 if(angle_flag == true && stopped_rover== false){
     go_forwards(Distance,total_y - coord_after_rotation);
      digitalWrite(pwmr, HIGH);       //setting right motor speed at maximum
      digitalWrite(pwml, HIGH);       //setting left motor speed at maximum    
     sumdist = sumdist + dy_mm;
     target_flag = true;
     Serial.println("sumdist = " + String(sumdist));
     Serial.println("coord_after_rotation in angle_flag loop = " + String(coord_after_rotation));
     Serial.println("Going forwards to the destination!");
     Serial.println("Distance = " + String(Distance));
     Serial.println("total_y - coord_after_rotation = " + String(total_y - coord_after_rotation));
     Serial.println("(total_y - coord_after_rotation) - (Distance) = " + String((total_y - coord_after_rotation) - (Distance)));
 }

// digitalWrite(DIRR, DIRRstate);
// digitalWrite(DIRL, DIRLstate);
}

//********************************************************

if (dir != change_char){
  change_char = dir;    // keeps track of the current command
  haschanged = true; 
  counter = 30;
  }
if (counter < 0 ){
    current_angle = anglechanged + current_angle;
    //sumchanged multiplied by cos(current angle) + i sin(current angle) 
    //cos is y axis sin is x axisSerial.println("current_x = "+ String(current_x));
    current_y = current_y + sumchanged * cos(current_angle);
    current_x = current_x + sumchanged * sin(current_angle);
    Serial.println("current_x = "+ String(current_x));
    Serial.println("current_y = "+ String(current_y));
    Serial.println("current_angle has changed to " + String(toDegrees(current_angle)));
    Serial1.print("{" + String(current_angle) + "}"); // Sends info back to ESP
    Serial.println("x and y have changed to " + String(current_x) + " " + String(current_y));
    Serial1.print("<" + String(current_x) + "," + String(current_y) + ">"); 
    
    
    // Sends info back to ESP
    unsigned long now = millis();
    if (now - lastMsg > 200) {
      lastMsg = now;
      sendcoords(current_x, current_y,current_angle);
    }
    
    
    sumchanged = 0;
    anglechanged = 0;
    counter = 0;
    haschanged = false;
 }
 else if (dir == 'S' && haschanged == true){
  O_to_coord_measured = sqrt(pow(dy_mm,2) + pow(dx_mm,2));
  alpha = asin(O_to_coord_measured/(2*r)) * 4 ; 
  anglechanged = (anglechanged + alpha);
  sumchanged += dy_mm;
  counter--;
  }
   
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
     pdata[count++] = (pix & 0x3f)<<2; // scale to normal greyscale byte range
     }
  }
  digitalWrite(PIN_MOUSECAM_CS,HIGH); 
  delayMicroseconds(14);
  return ret;
}

void sampling(){

  // Make the initial sampling operations for the circuit measurements
  
  sensorValue0 = analogRead(A0); //sample Vb
  sensorValue2 = analogRead(A2); //sample Vref
 // sensorValue2 = vref *(1023.0/ 4.096);  
  sensorValue3 = analogRead(A3); //sample Vpd
  current_mA = ina219.getCurrent_mA(); // sample the inductor current (via the sensor chip)
  
  /* Process the values so they are a bit more usable/readable
     The analogRead process gives a value between 0 and 1023 
     representing a voltage between 0 and the analogue reference which is 4.096V
  */
  //sensorValue2 = 500;
  vb = sensorValue0 * (4.096 / 1023.0); // Convert the Vb sensor reading to volts
  vref = sensorValue2 * (4.096 / 1023.0); // Convert the Vref sensor reading to volts
  // now vref is set at the top of the code
  //sensorValue2 = vref *(1023.0/ 4.096);
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

//PI controller for the Distance
float pidDistance(float pidDistance_input){

  float e_integration;
  e0Distance = pidDistance_input;
  e_integration = e0Distance;

  delta_uDistance = kpDistance * (e0Distance - e1Distance) + kiDistance * Ts * e_integration; // incremental PI avoids integrations
  u0Distance = u1Distance + delta_uDistance;  // this time's control output

  u1Distance = u0Distance;  // update last time's control output
  e2Distance = e1Distance;  // update last last time's error -->> not in use in this PI
  e1Distance = e0Distance;  // update last time's error

  return u0Distance;
  
  }

void F_B_PIcontroller(int sensor_total_y, int target){
  error_y = sensor_total_y - target;
  cDistance = pidDistance(error_y);
  // pwm_modulate(1);
//  reached_destination = false;
  Serial.println("error_y = " + String(error_y));
  Serial.println("cDistance = " + String(cDistance));
  
  if(error_y < -10){
    goForwards();
    }
  if(error_y > 10){
    goBackwards();
    }
  if(error_y >= -10 && error_y <= 0){
    pwm_modulate(0.3);
    goForwards;
    }
  if(error_y > 0 && error_y < 10){
    pwm_modulate(0.3);
    goBackwards;
    }
  if(error_y >= -2 && error_y <= 2){
    stop_Rover();
    Serial.println("Reached Distance with PI");
//    reached_destination = true;
    }
  }

void TurnLeft_PIcontroller(float desired_angle){
  error_angle = desired_angle - anglechanged;
  cDistance = pidDistance(error_angle);
  reached_angle = false;
  O_to_coord_measured = sqrt(pow(dy_mm,2) + pow(dx_mm,2));
  alpha = toDegrees(asin(O_to_coord_measured/(2*r))) * 4 ; 
  anglechanged = (anglechanged + alpha);
  
  Serial.println("error_angle" + String(error_angle));
  Serial.println(" anglechanged = "+ String (anglechanged));
  Serial.println(" O_to_coord_measured = "+ String (O_to_coord_measured));
  pwm_modulate(abs(cDistance)*0.01+0.39);
  //pwm_modulate(0.5);
  Serial.println("Inside the rotation PI");
  //pwm_modulate(abs(cDistance)*0.01+0.4);
   if(error_angle < -10.5){
    pwm_modulate(0.3);
    //goRight();
    stop_Rover();
    Serial.println("stopped because angle has been reached");
    }
  if(error_angle > 10.5 && error_angle < desired_angle){
    //pwm_modulate(0.5);
    goLeft();
    Serial.println("Rotation left >10.5");
    }else{Serial.println("something is wrong");
    }
  if(error_angle >= -10.5 && error_angle <= 0){
    //pwm_modulate(0.4);
    goRight;
    Serial.println("Rotation right >-10.5 && <0");
    }
  if(error_angle > 0 && error_angle < 10.5){
    //pwm_modulate(0.4);
    goLeft;
    Serial.println("Rotation left >0 && <10.5");    
    }
  if(error_angle >= -1 && error_angle <= 1){
    stop_Rover();
    Serial.println("reached_angle = " +String(reached_angle));
    Serial.println("Reached Angle with PI");
    reached_angle = true;
//    angle_PI = anglechanged;
//    Serial.println("final obtained with PI" + String(angle_PI));
    //anglechanged=0;
    //O_to_coord_measured = 0;
    
    }
  }


void go_forwards(int command_forwards_des_dist, int sensor_forwards_distance){
  int distance_error = sensor_forwards_distance - command_forwards_des_dist;
  halted = 0;
  if(abs(distance_error) < 5){
     // stop rover
     //pwm_modulate(0);
     stop_Rover();
     halted = 1;
     haltTime = millis();
     Serial.print("\n");
     Serial.print("\n");
     Serial.println("Reached Distance with go_forwards");
     Serial.print("Halting at ");
     Serial.println(haltTime);
     return;
     }
  if(distance_error <= -15){
    // goes forwards
    goForwards();
    return;
  }else if(distance_error >-15 && distance_error <0){
    pwm_modulate(0.25);
    //goes forwards
    goForwards();
    return;
  }else if(distance_error >= 15){
    // goes backwards
    goBackwards();
    return;
  }else if(distance_error >0 && distance_error <15){
    pwm_modulate(0.25);
    //goes backwards
    goBackwards();
    return;
   }  
 }

void turn_90left(unsigned long haltTime){
    unsigned long now = millis();
    angle_flag = 1;
    if(now-haltTime < 4500){
      goLeft();
    }
    else{
      Serial.print("We have stopped rotating ");
      Serial.println(now);
      Serial.println(haltTime);
      stop_Rover();
      halted = 0;
      finished_turning=true;
      y_after_rotation= total_y;
      Serial.println(reached_x_position);
    }
    return; 
}

// same as turn_90left
void turn_90right(unsigned long haltTime){
  unsigned long now = millis();
    if(now-haltTime < 4500){
      goRight();
    }
  else{
      Serial.print("We have stopped rotating ");
      Serial.println(now);
      Serial.println(haltTime);
      stop_Rover();
      halted = 0;
      finished_turning=true;
      y_after_rotation= total_y;
      Serial.println(reached_x_position);
    }
    return;
}

void stop_Rover(){
  digitalWrite(pwmr, LOW);
  digitalWrite(pwml, LOW);
  //pwm_modulate(0);
  
  digitalWrite(DIRR, DIRRstate);
  digitalWrite(DIRL, DIRLstate);
  Serial.println("Stop function activated");
}

void angle_90_timedRight(unsigned long startTime){
  unsigned long now = millis();
  if(now - startTime < 4500){
      goRight();
  }else{
    stop_Rover();}
    return;
 }

void angle_90_timedLeft(unsigned long startTime){
  unsigned long now = millis();
  if(now - startTime < 4500){
      goLeft();
  }else{
    stop_Rover();}
    return;
 }

float toDegrees(float angleRadians){
    return (angleRadians/pi)*180;
}


  void goForwards(){
    digitalWrite(pwmr, HIGH);       
    digitalWrite(pwml, HIGH);         
    DIRRstate = LOW;
    DIRLstate = HIGH;
    digitalWrite(DIRR, DIRRstate);
    digitalWrite(DIRL, DIRLstate);    
    }

   void goBackwards(){
    digitalWrite(pwmr, HIGH);       
    digitalWrite(pwml, HIGH);         
    DIRRstate = HIGH;
    DIRLstate = LOW;   
    digitalWrite(DIRR, DIRRstate);
    digitalWrite(DIRL, DIRLstate);
   }

  void goRight(){
    digitalWrite(pwmr, HIGH);       
    digitalWrite(pwml, HIGH);          
    DIRRstate = HIGH;
    DIRLstate = HIGH;  
    digitalWrite(DIRR, DIRRstate);
    digitalWrite(DIRL, DIRLstate); 
    }

   void goLeft(){
    digitalWrite(pwmr, HIGH);       
    digitalWrite(pwml, HIGH);          
    DIRRstate = LOW;
    DIRLstate = LOW;
    digitalWrite(DIRR, DIRRstate);
    digitalWrite(DIRL, DIRLstate);   
    }

void sendcoords(int x_coord_send, int y_coord_send, float angle_send){

  char buf[18];
  buf[0] = '<';
  int cur = 1;
  char num[6];
  String(x_coord_send).toCharArray(num,6);
  //sprintf(num, "%d", x_coord_send);
  int cnt = 0;
  while(num[cnt]){
    buf[cur++] = num[cnt++];
  }
  buf[cur++] = ',';
  //sprintf(num, "%d", y_coord_send);
  String(y_coord_send).toCharArray(num,6);
  cnt = 0;
  while(num[cnt]){
    buf[cur++] = num[cnt++];
  }
  buf[cur++] = ',';
  //sprintf(num, "%d", angle_send);
  String(angle_send).toCharArray(num,6);
  cnt = 0;
  while(num[cnt]){
    buf[cur++] = num[cnt++];
  }

  buf[cur++] = '>';
  buf[cur] = '\0';

  for(int i = 0; buf[i] != '\0'; i++){

    Serial1.println(buf[i]);

  }
}

void sendflag(){

  //Serial1.println('P');
}

/*end of the program.*/
