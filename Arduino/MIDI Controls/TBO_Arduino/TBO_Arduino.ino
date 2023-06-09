
#include "MIDIUSB.h"
#include "Bounce2.h"
#include <EncoderStepCounter.h>
/* 
MIDI MAP
42 | button
91, 3  | slider
43 | up
44 | down
45 | left
46 | right
91, 4 | rotary
*/

//sauce button -- pin 5
const int buttonPin = 5;
int buttonState = 0;
Bounce2::Button button = Bounce2::Button();

//chopstick slider -- pin A0
int sliderCC = 0;
int lastSliderCC = 0;

//taco joystick -- pins 7, 8, 9, 10
const int upPin = 7;
const int downPin = 8;
const int leftPin = 9;
const int rightPin = 10;
int joyUp = 1;
int joyDown = 1;
int joyLeft = 1;
int joyRight = 1;
Bounce2::Button upButt = Bounce2::Button();
Bounce2::Button downButt = Bounce2::Button();
Bounce2::Button leftButt = Bounce2::Button();
Bounce2::Button rightButt = Bounce2::Button();


//baja rotary encoder -- pins 2, 3
const int rePin1 = 2;
const int rePin2 = 3; //ugh
EncoderStepCounter encoder(rePin1, rePin2);
int oldEncoderPos = 0;



//tests
const int ledPin = 13;
int ledState = 0;

void setup() {
  Serial.begin(9600);
  // while (!Serial);

  //sauce button
  button.attach(buttonPin, INPUT_PULLUP);
  button.interval(5);
  button.setPressedState(LOW);

  //chopstick slider

  //taco joystick
  upButt.attach(upPin, INPUT_PULLUP);
  upButt.interval(5);
  upButt.setPressedState(LOW);
  downButt.attach(downPin, INPUT_PULLUP);
  downButt.interval(5);
  downButt.setPressedState(LOW);
  leftButt.attach(leftPin, INPUT_PULLUP);
  leftButt.interval(5);
  leftButt.setPressedState(LOW);
  rightButt.attach(rightPin, INPUT_PULLUP);
  rightButt.interval(5);
  rightButt.setPressedState(LOW);
  // pinMode(upPin, INPUT_PULLUP);
  // pinMode(downPin, INPUT_PULLUP);
  // pinMode(leftPin, INPUT_PULLUP);
  // pinMode(rightPin, INPUT_PULLUP);

  //baja rotary encoder
  encoder.begin();
  attachInterrupt(digitalPinToInterrupt(rePin1), interrupt, CHANGE);
  attachInterrupt(digitalPinToInterrupt(rePin2), interrupt, CHANGE);

  //tests
  pinMode(ledPin, OUTPUT);

}

void loop() {

  //sauce button
  button.update();

  if(button.pressed()){
    ledState = !ledState;
    digitalWrite(ledPin, ledState);
    noteOn(0x90, 42, 0x45);
    delay(100);
    noteOn(0x90, 42, 0x00);
  }

  //chopstick slider
  int sliderValue = analogRead(A0);

  sliderCC = map(sliderValue, 0, 1024, 0, 127);
  // if (sliderCC != lastsliderCC){
  if (abs(sliderCC - lastSliderCC) >= 4){
    controlChange(0x91, 0x03, sliderCC);
    lastSliderCC = sliderCC;
    // Serial.println(sliderCC);
    delay(1);
  }

  //taco joystick
  upButt.update();
  downButt.update();
  leftButt.update();
  rightButt.update();

  if (upButt.pressed()){
    noteOn(0x90, 43, 0x45);
    delay(5);
  } else if (upButt.released()){
    noteOn(0x90, 43, 0x00);
  }
  if (downButt.pressed()){
    noteOn(0x90, 44, 0x45);
    delay(5);
  } else if (downButt.released()){
    noteOn(0x90, 44, 0x00);
  }
  if (leftButt.pressed()){
    noteOn(0x90, 45, 0x45);
    delay(5);
  } else if (leftButt.released()){
    noteOn(0x90, 45, 0x00);
  }
  if (rightButt.pressed()){
    noteOn(0x90, 46, 0x45);
    delay(5);
  } else if (rightButt.released()){
    noteOn(0x90, 46, 0x00);
  }

  // joyUp = digitalRead(upPin);
  // joyDown = digitalRead(downPin);
  // joyLeft = digitalRead(leftPin);
  // joyRight = digitalRead(rightPin);

  // if (joyUp == 0){
  //   noteOn(0x90, 43, 0x45);
  //   delay(20);
  //   noteOn(0x90, 43, 0x00);
  // }
  // if (joyDown == 0){
  //   noteOn(0x90, 44, 0x45);
  //   delay(20);
  //   noteOn(0x90, 44, 0x00);
  // }
  // if (joyLeft == 0){
  //   noteOn(0x90, 45, 0x45);
  //   delay(20);
  //   noteOn(0x90, 45, 0x00);
  // }
  // if (joyRight == 0){
  //   noteOn(0x90, 46, 0x45);
  //   delay(20);
  //   noteOn(0x90, 46, 0x00);
  // }

  //baja rotary encoder
  // encoder.tick();
  int pos = encoder.getPosition();
  Serial.println(pos);
  if (pos > 127 || pos < 0){ //idk
    encoder.reset();
  }
  if (pos != oldEncoderPos){
    // pos = constrain(pos, 0, 127);
    controlChange(0x91, 0x04, pos);
    oldEncoderPos = pos;
    Serial.println("change");
    delay(1);
  }

  //tests
  // delay(10);
  
}

void interrupt(){
  encoder.tick();
}

void noteOn(byte cmd, byte data1, byte data2){
  midiEventPacket_t midiMsg = {cmd >> 4, cmd, data1, data2};
  MidiUSB.sendMIDI(midiMsg);
}

void controlChange(byte channel, byte control, byte value) {
  midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};
  MidiUSB.sendMIDI(event);
}

