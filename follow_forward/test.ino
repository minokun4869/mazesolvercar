#include <Arduino.h>
#include <Wire.h>
#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h>
#include "kmotor.h"
#include <Servo.h>

LiquidCrystal_I2C lcd(0x27,16,2);
kmotor _kmotor(true);

int initial_motor_speed = 200;

int pre=0,dem=0,times=360,calm=100;
int disFront,disLeft,disRight,disBack;
float previous_error = 0, previous_I = 0;
int lastError=0;
int turn[50]={1,2,1,0,1,2};
float Kp=10,Kd=30,Ki=0;
float error = 0, P = 0, I = 0, D = 0, PID_value = 0;

long en_val_left,num_val_left=0,pre_val_left=-1;
long en_val_right,num_val_right=0,pre_val_right=-1;

void trai(int deg){
    int ticks=12.6*deg/360/6.2*20;
    int temp=num_val_left;
    while(num_val_left<temp+ticks){
      readEncoderLeft();
      readEncoderRight();
      _kmotor.tien(0, -200);
      _kmotor.tien(1, 200);
    }
}

void phai(int deg){
    int ticks=12.6*deg/360/6.2*20;
    int temp=num_val_left;
    while(num_val_left<temp+ticks){
      readEncoderLeft();
      readEncoderRight();
      _kmotor.tien(0, 200);
      _kmotor.tien(1, -200);
    }
}

void readEncoderLeft(){
  en_val_left=digitalRead(10);
  if(en_val_left==0&&pre_val_left==1)num_val_left++;
  pre_val_left=en_val_left;
 //Serial.println(num_val_left);
  //Serial.print(' ');
  //num_val_left=0;
}

void readEncoderRight(){
  en_val_right=digitalRead(9);
  if(en_val_right==0&&pre_val_right==1)num_val_right++;
  pre_val_right=en_val_right;
 // Serial.println(num_val_right);
  //num_val_left=0;
}

void setup() {
  _kmotor.cauhinh();
 Serial.begin(9600);
  
}

float getDistance(int trig,int echo){
  float dem=0;
    pinMode(trig,OUTPUT);
    digitalWrite(trig,LOW);
    delayMicroseconds(2);
    digitalWrite(trig,HIGH);
    delayMicroseconds(10);
    digitalWrite(trig,LOW);
    pinMode(echo, INPUT);
    dem = pulseIn(echo,HIGH,30000)/58.0;
    if(dem==0) dem=30;
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
  _kmotor.tien(0, left_motor_speed);//M1
  _kmotor.tien(1, right_motor_speed);//M2
}

void go(float centimet){
  int ticks=centimet/6.2/3.1416*20;
  int temp=num_val_left;
  while(num_val_left<temp+ticks){
    readEncoderLeft();
    readEncoderRight();
    calculate_pid();
    motor_control();
  }
}

void mfwd()
{
    
    _kmotor.tien(0, 100);
    _kmotor.tien(1, 100);

}

void bam_trai(){
  error=disLeft-8;
  calculate_pid();
  motor_control();
}

void bam_phai(){
  error=8-disRight;
  calculate_pid();
  motor_control();
}

void real_thang(){
  if(disLeft>=20&&disRight>=20){
     num_val_left=0;
     num_val_right=0;
     error=num_val_left-num_val_right;
     analogWrite(A3,300);//red
  }else if(disLeft<20){
     error=disLeft-8;
     analogWrite(A2,300);//green
  }else{
    error=8-disRight;
    analogWrite(A1,300);//blue
  }
  go(30);
}

void tinh(){
    disFront=getDistance(2,12);
    disLeft=getDistance(4,5);
    disRight=getDistance(11,13);
    while(disFront > 10)
    {
        mfwd();
        disFront = getDistance(2, 12);
    }
    s
   
}

void loop(){
    tinh();
}
