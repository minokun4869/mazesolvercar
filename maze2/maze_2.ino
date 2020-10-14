#include <Arduino.h>
#include <Wire.h>
#include <SoftwareSerial.h>
#include "kmotor.h"
#include <Servo.h>
//#LiquidCrystal_I2C lcd(0x27,16,2);
kmotor _kmotor(true);
int initial_motor_speed = 20;
int pre = 0, dem = 0, calm = 150;
int disFront, disLeft, disRight, disBack;
float previous_error = 0, previous_I = 0;
int lastError = 0;
int turn[50] = {2, 0, 2, 0, 2, 0, 2, 2, 0, 0, 2, 0, 2, 0, 0, 2, 2};
//{2,0, 0,0,0,0,  2,0,2,0, 0,2, 2,2,0, 2,0,0,2,0,0,0,0,2,0,0};
float Kp = 10, Kd = 30, Ki = 0;
float error = 0, P = 0, I = 0, D = 0, PID_value = 0;
long en_val_left, num_val_left = 0, pre_val_left = -1;
long en_val_right, num_val_right = 0, pre_val_right = -1;
void trai(int deg)
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
void phai(int deg)
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
void setup()
{
  _kmotor.cauhinh();
  Serial.begin(19200);
  pinMode(A4, INPUT);
  pinMode(A5, INPUT);
  pinMode(A6, INPUT);
  pinMode(A7, INPUT);
}

float getDistance(int trig, int echo)
{
  float dem = 0;
  pinMode(trig, OUTPUT);
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  pinMode(echo, INPUT);
  dem = pulseIn(echo, HIGH, 30000) / 58.0;
  if (dem == 0)
    dem = 30;
  return dem;
}
void calculate_pid()
{
  P = error;
  I = I + previous_I;
  D = error - previous_error;
  PID_value = (Kp * P) + (Ki * I) + (Kd * D);
  previous_I = I;
  previous_error = error;
}
void motor_control()
{
  // Calculating the effective motor speed:
  int left_motor_speed = initial_motor_speed - PID_value;
  int right_motor_speed = initial_motor_speed + PID_value;
  // The motor speed should not exceed the max PWM value
  left_motor_speed = constrain(left_motor_speed, 0, 255);
  right_motor_speed = constrain(right_motor_speed, 0, 255);
  //Serial.println(left_motor_speed);
  //Serial.println(right_motor_speed);
  _kmotor.tien(0, left_motor_speed);  //M1
  _kmotor.tien(1, right_motor_speed); //M2
}
void bam_trai()
{
  error = disLeft - 8;
  calculate_pid();
  motor_control();
}
void bam_phai()
{
  error = 8 - disRight;
  calculate_pid();
  motor_control();
}
void go()
{
  readEncoderLeft();
  readEncoderRight();
  error = num_val_left - num_val_right;
  calculate_pid();
  motor_control();
}
void real_thang()
{
  if (disLeft >= 15 && disRight >= 15)
  {
    _kmotor.run(0, 200);
    analogWrite(A3, 300); //red
  }
  else
  {
    if (disLeft < 15)
    {
      bam_trai();
      analogWrite(A2, 300); //green
    }
    else
    {
      bam_phai();
      analogWrite(A1, 300); //blue
    }
  }
}
void tinh()
{
  disFront = getDistance(2, 12);
  disLeft = getDistance(4, 5);
  disRight = getDistance(11, 13);
  Serial.println(disFront);
  Serial.println(disLeft);
  Serial.println(disRight);
  if (disFront > 11)
  {
    real_thang();
  }
  else
  {
    _kmotor.stop();
    if(disLeft > disRight && disLeft > 20)
    {
        trai();
    }
    else if
      


    

    // delay(calm);
    // if(turn[dem]==0){
    //   trai(90);
    //   //while(!(disFront>10)){trai();disFront=getDistance(2,12);}
    // }
    // if(turn[dem]==1){
    //   go();
    //   //while(!(disFront>10)){thang();disFront=getDistance(2,12);}
    // }
    // if(turn[dem]==2){
    //   phai(90);
    //   //while(!(disFront>10)){phai();disFront=getDistance(2,12);}
    // }
    // if(turn[dem]==3){
    //   //while(!(disFront>10)){lui();disFront=getDistance(2,12);}
    // }
    // dem=dem+1;
    // _kmotor.stop();
    // delay(calm);
  }
  analogWrite(A1, 0);
  analogWrite(A2, 0);
  analogWrite(A3, 0);
}
void loop()
{
  tinh();
}
