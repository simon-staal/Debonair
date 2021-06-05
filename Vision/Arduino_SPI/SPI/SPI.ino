#include <SPI.h>

#define VSPI_MISO   MISO
#define VSPI_MOSI   MOSI
#define VSPI_SCLK   SCK
#define VSPI_SS     SS

SPISettings settings(100000, MSBFIRST, SPI_MODE0);

uint16_t val;
uint8_t counter20;
uint8_t counter21;
uint8_t counter22;
uint8_t counter23;
uint8_t counter24;
uint8_t counter25;
uint8_t reg;
uint16_t returnval;

//vspi default pins SCLK = 18, MISO = 19, MOSI = 23, SS = 5 
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(VSPI_SS, OUTPUT);

  SPI.begin();
  resetcounter();

  returnval = 0;
}

void resetcounter(){
  counter20 = 0;
  counter21 = 0;
  counter22 = 0;
  counter23 = 0;
  counter24 = 0;
  counter25 = 0;
}

void loop() {
  // put your main code here, to run repeatedly:

  SPI.beginTransaction(settings);

  digitalWrite(VSPI_SS, LOW);

  val = SPI.transfer16(returnval);
  returnval = 0;
  
  
  digitalWrite(VSPI_SS, HIGH);

  SPI.endTransaction();
  if (val == 2048){
    Serial.print("Drone rotate right \n");
    resetcounter();
  }
  if (val == 4096){
    Serial.print("Drone rotate left\n");
    resetcounter();
  }
  if (val == 8192){
    Serial.print("Drone move backwards \n");
    resetcounter();
  }
  if (val == 16384){
    Serial.print("Drone move forwards \n");
    resetcounter();
  }
  if (val > 32768){
    Serial.print("Calculating... \n");
    val = val - 32768;
    val = val >> 7;
    reg = val & 7;
    val = val >> 3;
    if (reg == 0){
      Serial.print("We have a red ball \n");
      if (val == 20){
        counter20 += 1;
        if(counter20 == 100){
          Serial.print("Distance is 20cm");
          returnval = 100;
          resetcounter();
          delay(10000);
        }
      }
      else if (val == 21){
        counter21 += 1;
        if(counter21 == 100){
          Serial.print("Distance is 21cm");
          returnval = 100;
          resetcounter();
          delay(10000);
        }
      }
      else if (val == 22){
        counter22 += 1;
        if(counter22 == 100){
          Serial.print("Distance is 22cm");
          returnval = 100;
          resetcounter();
          delay(10000);
        }
      }
      else if (val == 23){
        counter23 += 1;
        if(counter23 == 100){
          Serial.print("Distance is 23cm");
          returnval = 100;
          resetcounter();
          delay(10000);
        }
      }
      else if (val == 24){
        counter24 += 1;
        if(counter24 == 100){
          Serial.print("Distance is 24cm");
          returnval = 100;
          resetcounter();
          delay(10000);
        }
      }
      else if (val == 25){
        counter25 += 1;
        if(counter25 == 100){
          Serial.print("Distance is 25cm");
          returnval = 100;
          resetcounter();
          delay(10000);
        }
      }
    }
    if (reg == 1){
      Serial.print("We have a yellow ball \n");
      if (val == 20){
        counter20 += 1;
        if(counter20 == 100){
          Serial.print("Distance is 20cm");
          returnval = 200;
          resetcounter();
          delay(10000);
        }
      }
      else if (val == 21){
        counter21 += 1;
        if(counter21 == 100){
          Serial.print("Distance is 21cm");
          returnval = 200;
          resetcounter();
          delay(10000);
        }
      }
      else if (val == 22){
        counter22 += 1;
        if(counter22 == 100){
          Serial.print("Distance is 22cm");
          returnval = 200;
          resetcounter();
          delay(10000);
        }
      }
      else if (val == 23){
        counter23 += 1;
        if(counter23 == 100){
          Serial.print("Distance is 23cm");
          returnval = 200;
          resetcounter();
          delay(10000);
        }
      }
      else if (val == 24){
        counter24 += 1;
        if(counter24 == 100){
          Serial.print("Distance is 24cm");
          returnval = 200;
          resetcounter();
          delay(10000);
        }
      }
      else if (val == 25){
        counter25 += 1;
        if(counter25 == 100){
          Serial.print("Distance is 25cm");
          returnval = 200;
          resetcounter();
          delay(10000);
        }
      }
    }
    if (reg == 2){
      Serial.print("We have a green ball \n");
      Serial.print("Distance is: ");
      Serial.println(val);
    }
    if (reg == 3){
      Serial.print("We have a blue ball \n");
      Serial.print("Distance is: ");
      Serial.println(val);
    }
  }

  
  delay(10);
}
