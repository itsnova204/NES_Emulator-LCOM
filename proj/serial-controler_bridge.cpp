#include <Arduino.h>
//This code translates the output of the NES controler to serial
//The NES controler is a shift register with 8 buttons
//The serial output is the state of the buttons !when they change!

//Tiago Aleixo, May 2021

//usefull doc: https://tresi.github.io/nes/

/*
  chinese controler dosent respect color coding
  but the pinout is this:
  
          +----> Power +5V  (black)
          |
  5 +---------+  7
  | x  x  o   \
  | o  o  o  o |
  4 +------------+ 1
    |  |  |  |
    |  |  |  +-> Ground (red)
    |  |  +----> Pulse  (yellow)
    |  +-------> Latch  (green)
    +----------> Data   (white)
*/
const int baudRate = 230400;

const int LED = 14;
const int PULSE = 0; // yellow
const int LATCH = 4; // green
const int DATA = 5; // white

void setup() {
  pinMode(LED, OUTPUT);
  pinMode(PULSE, OUTPUT);
  pinMode(LATCH, OUTPUT);
  pinMode(DATA, INPUT);

  digitalWrite(LATCH, 0);
  digitalWrite(PULSE, 1);
  digitalWrite(DATA, 1); // data is up by default, goes to ground if key is pressed

  //init serial
  Serial.begin(baudRate);
}

#define BUTTON_A (0)
#define BUTTON_B (1)
#define BUTTON_SELECT (2)
#define BUTTON_START (3)
#define BUTTON_UP (4)
#define BUTTON_DOWN (5)
#define BUTTON_LEFT (6)
#define BUTTON_RIGHT (7)

#define BYTE_TO_BINARY_PATTERN " %c%c%c%c%c%c%c%c "
#define BYTE_TO_BINARY(byte)  \
  ((byte) & 0x80 ? '1' : '0'), \
  ((byte) & 0x40 ? '1' : '0'), \
  ((byte) & 0x20 ? '1' : '0'), \
  ((byte) & 0x10 ? '1' : '0'), \
  ((byte) & 0x08 ? '1' : '0'), \
  ((byte) & 0x04 ? '1' : '0'), \
  ((byte) & 0x02 ? '1' : '0'), \
  ((byte) & 0x01 ? '1' : '0') 
  
uint8_t buttons;
uint8_t oldbuttons;

void loop() {
    buttons = 0;
    delayMicroseconds(16*1000);

    digitalWrite(LATCH, 1);
    delayMicroseconds(12);
    digitalWrite(LATCH, 0);

  
    for (int i=0; i < 8; ++i) {
      delayMicroseconds(6);
      buttons |= (!digitalRead(DATA) << i);
      digitalWrite(PULSE, 0);
      
      delayMicroseconds(5);
      digitalWrite(PULSE, 1);
    }


    // Light up the LED if any key was pressed.

    if (buttons != 0){
      digitalWrite(LED, 1);
    }else{
      digitalWrite(LED, 0);
    }
    

    if (buttons == oldbuttons) {
      return;
    }

    //visual print to Serial Monitor
    Serial.printf("Buttons: " BYTE_TO_BINARY_PATTERN "\n", BYTE_TO_BINARY(buttons)); 
    
    //TODO: package the data and send it to the serial port as bits
    //Serial.write(buttons);

    oldbuttons = buttons;
    
    delay(100);

}
