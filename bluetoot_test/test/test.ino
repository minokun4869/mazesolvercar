#include <Arduino.h>
#include <Wire.h>
#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h>
#include "kmotor.h"
#include <Servo.h>


kmotor _kmotor(true);
char state;
bool manual_mode = false;
bool auto_mode = false;
bool start_record = false;
char record[100];
int count = 0;

void setup() {
  _kmotor.cauhinh();
  Serial.begin(9600);
  
}

void left(int deg){
    int ticks=12.6*deg/360/6.2*20;
    int temp=num_val_right;
    while(num_val_right<temp+ticks){
      readEncoderLeft();
      readEncoderRight();
      _kmotor.tien(0, -150);
      _kmotor.tien(1, 150);
    }
}

void right(int deg){
    int ticks=12.6*deg/360/6.2*20;
    int temp=num_val_left;
    while(num_val_left<temp+ticks){
      readEncoderLeft();
      readEncoderRight();
      _kmotor.tien(0, 150);
      _kmotor.tien(1, -150);
    }
}

void readEncoderLeft(){
  en_val_left=digitalRead(10);
  if(en_val_left==0&&pre_val_left==1)num_val_left++;
  pre_val_left=en_val_left;
  Serial.print(num_val_left);
  Serial.print(' ');
  //num_val_left=0;
}

void readEncoderRight(){
  en_val_right=digitalRead(9);
  if(en_val_right==0&&pre_val_right==1)num_val_right++;
  pre_val_right=en_val_right;
  Serial.println(num_val_right);
  //num_val_left=0;
}

void controller(char command) {
  if (state == 'F') {
    forward();
  }
  if (state == 'M') {
    forward_right();
  }
  if (state == 'R' || state == 'P') {
    right();
  }
  if (state == 'B') {
    back();
  }
  if (state == 'E' || state == 'L') {
    left();
  }
  if (state == 'C') {
    forward_left();
  }

 }

void manual_controller(char start_record, char command) {
  if (start_record == true) {
    record[count] = command;
    count++;
  }
  controller(command);
}

void auto_controller() {
  char command;
  for (int i = 0; i < count; i++) {
    command = record[i];
    controller(command);
  }
}

void read_bluetooth() {
  if (Serial.available() > 0) {
    state = Serial.read();
  }
  else {
    state = 0;
  }
  Serial.println(state);
  if (state == 'U' || state == 'u') {
    manual_mode = true if manual_mode == false else false;
}
if (state == 'O' || state == 'o') {
    if (manual_mode == false) {
      auto_mode = true if auto_mode == false else false;
  }
  else {
    Serial.println('In manual mode');
    }
  }
  if (manual_mode == true && state == 'F' || state == 'M' ||
      state == 'R' || state == 'P' ||
      state == 'B' || state == 'E' ||
      state == 'L' || state == 'C' )
  {
    controller(state);
  }
  if (auto_mode == true) {
    auto_controller();
  }
}
void loop() {
  read_bluetooth();
}
