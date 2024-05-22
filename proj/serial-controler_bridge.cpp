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
const int baudRate = 9600;

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

char buttons[8];
char oldbuttons[8];

void loop() {
    delayMicroseconds(16*1000);

    digitalWrite(LATCH, 1);
    delayMicroseconds(12);
    digitalWrite(LATCH, 0);

  
    for (int i=0; i < 8; ++i) {
      delayMicroseconds(6);
       buttons[i] = !digitalRead(DATA);
      digitalWrite(PULSE, 0);
      
      delayMicroseconds(5);
      digitalWrite(PULSE, 1);
    }


    // Light up the LED if any key was pressed.
    int blink = 0;
    for (int i=0;i<8;++i) {
      blink = blink || buttons[i];
    }
    digitalWrite(LED, blink);

    if (buttons[0] == oldbuttons[0] && buttons[1] == oldbuttons[1] && buttons[2] == oldbuttons[2] && buttons[3] == oldbuttons[3] && buttons[4] == oldbuttons[4] && buttons[5] == oldbuttons[5] && buttons[6] == oldbuttons[6] && buttons[7] == oldbuttons[7]) {
      return;
    }
    
    Serial.printf("OLD Buttons: %d %d %d %d %d %d %d %d\n", oldbuttons[0], oldbuttons[1], oldbuttons[2], oldbuttons[3], oldbuttons[4], oldbuttons[5], oldbuttons[6], oldbuttons[7]); 
    Serial.printf("Buttons: %d %d %d %d %d %d %d %d\n", buttons[0], buttons[1], buttons[2], buttons[3], buttons[4], buttons[5], buttons[6], buttons[7]); 
    
    for (size_t i = 0; i < 8; i++){
      oldbuttons[i] = buttons[i];
    }
    
    delay(100);

}
