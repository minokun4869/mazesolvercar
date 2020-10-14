#include <Arduino.h>
#include <Wire.h>
#include <SoftwareSerial.h>
//#include <LiquidCrystal_I2C.h>
#include "kmotor.h"
#include <Servo.h>
//LiquidCrystal_I2C lcd(0x27, 16, 2);
#include <NewPing.h>


#define TRIGGERPIN_LEFT 4
#define ECHOPIN_LEFT 5
#define TRIGGERPIN_FRONT 2
#define ECHOPIN_FRONT 12
#define TRIGGERPIN_RIGHT 11
#define ECHOPIN_RIGHT 13

const in maxDistane = 100;

NewPing sonarLeft(TRIGGERPIN_LEFT, ECHOPIN_LEFT, MAX_DISTANCE); // NewPing setup of pins and maximum distance.
NewPing sonarRight(TRIGGERPIN_RIGHT, ECHOPIN_RIGHT, MAX_DISTANCE);
NewPing sonarFront(TRIGGERPIN_FRONT, ECHOPIN_FRONT, MAX_DISTANCE);

unsigned int pingSpeed = 30; // How frequently are we going to send out a ping (in milliseconds). 50ms would be 20 times a second.
unsigned long pingTimer;     // Holds the next ping time.


float oldLeftSensor, oldRightSensor, leftSensor, rightSensor, frontSensor, oldFrontSensor, lSensor, rSensor, fSensor;





kmotor _kmotor(true);
int initial_motor_speed = 200;
int pre = 0, dem = 0, times = 562; // chinh times de quay dung 90 do moi lan chinh nap lai code + clean banh
//int frontSensor, leftSensor, rightSensor, disBack;
float previous_error = 0, previous_I = 0;
int lastError = 0;
int turn[50] = {2, 0, 2, 0, 2, 0, 2, 2, 0, 0, 2, 0, 2, 0}; // mang hang 0 la re trai 1 la di thang 2 la re phai 3 la di lui moi khi gap tuong
//{2,0, 0,0,0,0,  2,0,2,0, 0,2, 2,2,0, 2,0,0,2,0,0,0,0,2,0,0};
float Kp = 10, Kd = 30, Ki = 0;
float error = 0, P = 0, I = 0, D = 0, PID_value = 0;

bool wallLeft = false, wallRight = false, wallFront = false;
const int wallThreshold = 30;
const int frontThreshold = 10;



void thang()
{
  _kmotor.tien(0, 200);
  _kmotor.tien(1, 200);
}
void trai()
{
  /*
    _kmotor.tien(0, -200);
    _kmotor.tien(1, 200);
  */
  digitalWrite(7, 0);
  digitalWrite(6, 1);
  digitalWrite(8, 1);
  digitalWrite(3, 1);
}
void phai()
{
  /*
    _kmotor.tien(0, 100);
    _kmotor.tien(1, -100);
  */
  digitalWrite(7, 1);
  digitalWrite(6, 1);
  digitalWrite(8, 0);
  digitalWrite(3, 1);
}
void lui()
{
  /*
    _kmotor.tien(0, -200);
    _kmotor.tien(1, -200);
  */
  digitalWrite(7, 1);
  digitalWrite(6, 1);
  digitalWrite(8, 0);
  digitalWrite(3, 1);
  delay(times * 2);
}

void setup()
{
  _kmotor.cauhinh();
  Serial.begin(9600);
  pinMode(A4, INPUT);
  pinMode(A5, INPUT);
  pinMode(A6, INPUT);
  pinMode(A7, INPUT);
  /*
    do
    {
      disBack = getDistance(9, 10);
      _kmotor.stop();
    } while (disBack > 3);
  */
}
/*
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

*/

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
  error = leftSensor - 8;
  calculate_pid();
  motor_control();
}
void bam_phai()
{
  error = 8 - rightSensor;
  calculate_pid();
  motor_control();
}
void real_thang()
{
  if (leftSensor >= 10 && rightSensor >= 10)
  {
    //_kmotor.run(0, 200);
    Serial.println("Bam thang");
    analogWrite(A3, 300); //red
  }
  if (leftSensor < 10)
  {
    //bam_trai();
    Serial.println("Bam trai");
    analogWrite(A2, 300); //green
  }
  if (rightSensor < 10)
  {
    //bam_phai();
    Serial.println("Bam phai");
    analogWrite(A1, 300); //blue
  }
}

void ReadSensors()
{
  lSensor = sonarLeft.ping_cm(); //ping in cm
  rSensor = sonarRight.ping_cm();
  fSensor = sonarFront.ping_cm();


  leftSensor = (lSensor + oldLeftSensor) / 2; //average distance between old & new readings to make the change smoother
  rightSensor = (rSensor + oldRightSensor) / 2;
  frontSensor = (fSensor + oldFrontSensor) / 2;


  oldLeftSensor = leftSensor; // save old readings for movment
  oldRightSensor = rightSensor;
  oldFrontSensor = frontSensor;
}

void FindingWall()
{
  /*
  frontSensor = getDistance(2, 12);
  leftSensor = getDistance(4, 5);
  rightSensor = getDistance(11, 13);
  disBack = getDistance(9, 10);
  */
  Serial.println(frontSensor);
  Serial.println(leftSensor);
  Serial.println(rightSensor);
  Serial.println(disBack);
  wallFront = (frontSensor <= frontThreshold) ? true : false;
  wallLeft = (leftSensor <= wallThreshold) ? true : false;
  wallRight = (rightSensor <= wallThreshold) ? true : false;
}
void tinh()
{

  Serial.println("-------------");
  ReadSensors();
  FindingWall();
  if (wallLeft == true)
  {
    if (wallFront == false)
    {
      real_thang();
      Serial.println("Thang");
    }
    else
    {

      if (wallRight == true)
      {
        /*
        _kmotor.stop();
        delay(50);
        lui();
        
        _kmotor.stop();
         delay(50);
         */
        Serial.println("Lui");
      }

      else
      {
        /*
        _kmotor.stop();
        delay(50);
        phai();
        delay(times);
        _kmotor.stop();
         delay(50);
         */
        Serial.println("Phai");
      }

    }
  }
  else
  {/*
    _kmotor.stop();
    delay(50);
    trai();
    delay(times);
    _kmotor.stop();
         delay(50);
         */
    Serial.println("Trai");
  }
  Serial.println("-------------");



  /*
    if (frontSensor > 11)
    { //sap dam vao tuong
      real_thang();
    }
    else
    {
    //      _kmotor.stop();
    //      for(int i=0; i<5; i++)
    //      {
    //        trai();
    //        delay(times);
    //        _kmotor.stop();
    //      }

      _kmotor.stop();
      delay(50);
      if (turn[dem] == 0)
      {
          trai();
          delay(times);
          _kmotor.stop();
          //while(!(frontSensor>10)){trai();frontSensor=getDistance(2,12);}
      }
      if (turn[dem] == 1)
      {
          thang();
          delay(times);
          _kmotor.stop();
          //while(!(frontSensor>10)){thang();frontSensor=getDistance(2,12);}
      }
      if (turn[dem] == 2)
      {
          phai();
          delay(times);
          _kmotor.stop();
          //while(!(frontSensor>10)){phai();frontSensor=getDistance(2,12);}
      }
      if (turn[dem] == 3)
      {
          lui();
          delay(times);
          _kmotor.stop();
          //while(!(frontSensor>10)){lui();frontSensor=getDistance(2,12);}
      }

      dem = dem + 1;
    }
  */
  analogWrite(A1, 0);
  analogWrite(A2, 0);
  analogWrite(A3, 0);
  delay(100);
}
void loop()
{
  tinh(); // nho clean banh moi lan thu
}