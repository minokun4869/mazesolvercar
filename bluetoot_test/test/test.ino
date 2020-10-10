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

int pre = 0, dem = 0, times = 360, calm = 100;
int disFront, disLeft, disRight, disBack;
float previous_error = 0, previous_I = 0;
int lastError = 0;
int turn[50] = {1, 2, 1, 0, 1, 2};
float Kp = 10, Kd = 30, Ki = 0;
float error = 0, P = 0, I = 0, D = 0, PID_value = 0;

long en_val_left, num_val_left = 0, pre_val_left = -1;
long en_val_right, num_val_right = 0, pre_val_right = -1;

void setup()
{
  _kmotor.cauhinh();
  Serial.begin(9600);
}

void readEncoderLeft()
{
  en_val_left = digitalRead(10);
  if (en_val_left == 0 && pre_val_left == 1)
    num_val_left++;
  pre_val_left = en_val_left;
  Serial.print(num_val_left);
  Serial.print(' ');
  //num_val_left=0;
}

void readEncoderRight()
{
  en_val_right = digitalRead(9);
  if (en_val_right == 0 && pre_val_right == 1)
    num_val_right++;
  pre_val_right = en_val_right;
  Serial.println(num_val_right);
  //num_val_left=0;
}

void forward()
{
  _kmotor.tien(0, 100);
  _kmotor.tien(1, 100);
}

void back()
{
  left = constrain(turn, 0, 255);
  right = constrain(fwd, 0, 255);
  mt.tien(0, left);
  mt.tien(1, -right);
  delay(700);
  mt.stop();
}

void left(int deg)
{
  int ticks = 12.6 * deg / 360 / 6.2 * 20;
  int temp = num_val_right;
  while (num_val_right < temp + ticks)
  {
    readEncoderLeft();
    readEncoderRight();
    _kmotor.tien(0, -150);
    _kmotor.tien(1, 150);
  }
}

void right(int deg)
{
  int ticks = 12.6 * deg / 360 / 6.2 * 20;
  int temp = num_val_left;
  while (num_val_left < temp + ticks)
  {
    readEncoderLeft();
    readEncoderRight();
    _kmotor.tien(0, 150);
    _kmotor.tien(1, -150);
  }
}

void controller(char command)
{
  if (command == 'F')
  {
    forward();
  }
  if (command == 'M')
  {
    right(45);
  }
  if (command == 'R' || state == 'P')
  {
    right(90);
  }
  if (command == 'B')
  {
    back();
  }
  if (command == 'E' || state == 'L')
  {
    left(90);
  }
  if (command == 'C')
  {
    left(45);
  }
}

void manual_controller(char start_record, char command)
{
  if (start_record == true)
  {
    record[count] = command;
    count++;
  }
  controller(command);
}

void auto_controller()
{
  char command;
  for (int i = 0; i < count; i++)
  {
    command = record[i];
    controller(command);
  }
}

void read_bluetooth()
{
  if (Serial.available() > 0)
  {
    state = Serial.read();
  }
  else
  {
    state = 0;
  }
  Serial.println(state);
  if (state == 'U' || state == 'u')
  {
    if (manual_mode == false)
    {
      manual_mode = true;
      Serial.println("ON Manual mode");
    }
  }
  if (state == 'O' || state == 'o')
  {
    if (manual_mode == true)
    {
      manual_mode = false;
      Serial.println("OFF Manual mode");
    }
  }

  if (manual_mode == false)
  {
    if (state == 'Y' || state == 'y')
    {
      if (auto_mode == false)
      {
        auto_mode = true;
        Serial.println("ON Auto mode");
      }
    }
    if (state == 'T' || state == 't')
    {
      if (auto_mode == true)
      {
        auto_mode = false;
        Serial.println("OFF Auto mode");
      }
    }
  }

  if (state == 'F' || state == 'M' ||
      state == 'R' || state == 'P' ||
      state == 'B' || state == 'E' ||
      state == 'L' || state == 'C')
  {
    if (manual_mode == true)
      manual_controller(true, state);
    else if (auto_mode == true)
      auto_controller();
  }
}
void loop()
{
  read_bluetooth();
}
