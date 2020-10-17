#include <Arduino.h>
#include <Wire.h>
#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h>
#include "kmotor.h"
#include <Servo.h>

LiquidCrystal_I2C lcd(0x27,16,2);
kmotor _kmotor(true);

int initial_motor_speed = 220;

bool auto_mode = 0, record_mode = 0, movable = 0;
char state;
int record,pre=0,dem=0,times=325;// chinh times de quay dung 90 do moi lan chinh nap lai code + clean banh 
int disFront,disLeft,disRight,disBack;
float previous_error = 0, previous_I = 0;
int lastError=0;
int turn[200], sturn;// mang hang dung 0 de re trai, 2 de re phai
//{2,0, 0,0,0,0,  2,0,2,0, 0,2, 2,2,0, 2,0,0,2,0,0,0,0,2,0,0};

float Kp=10,Kd=30,Ki=0;
float error = 0, P = 0, I = 0, D = 0, PID_value = 0;

void thang(){
    _kmotor.tien(0, 200);
    _kmotor.tien(1, 200);
}
void trai(){
    _kmotor.tien(0, -150);
    _kmotor.tien(1, 150); 
}
void phai(){
    _kmotor.tien(0, 150);
    _kmotor.tien(1, -150);
}
void lui(){
    _kmotor.tien(0, -200);
    _kmotor.tien(1, -200);
}


void setup() {
  _kmotor.cauhinh();
  Serial.begin(9600);
  pinMode(A4, INPUT);
  pinMode(A5, INPUT);
  pinMode(A6, INPUT);
  pinMode(A7, INPUT);
//  disBack=getDistance(9,10);
//  while(disBack>=3){
//    disBack=getDistance(9,10);
//    _kmotor.stop();
//  }
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
  if(disLeft>=10&&disRight>=10){
     _kmotor.run(0, initial_motor_speed);
     analogWrite(A3,300);//red
  }else if(disLeft<10){
     bam_trai();
     analogWrite(A2,300);//green
  }else{
    bam_phai();
    analogWrite(A1,300);//blue
  }
}

void read_turn()
{
	switch (state)
	{
	case 'L': record = 0; break;
	case 'R': record = 2; break;
	case 'S':
		Serial.println(record);
		turn[sturn++] = record;
		switch (record)
		{
		case 0: trai(); delay(times); _kmotor.stop(); break;
		case 2: phai(); delay(times); _kmotor.stop(); break;
		}
		break;
	}
}

void blue_control()
{
	switch (state)
	{
		case 'F': _kmotor.tien(0, 165); _kmotor.tien(1, 150); break;
		case 'B': _kmotor.run(1, 100); break;
		case 'L': _kmotor.run(2, 100); break;
		case 'R': _kmotor.run(3, 100); break;
		case 'S': _kmotor.stop(); break;
	}
}

void auto_control()
{
	disFront=getDistance(2,12);
    disLeft=getDistance(4,5);
    disRight=getDistance(11,13);
    disBack=getDistance(9,10);
	Serial.println(disFront);
	if (disBack <= 5) movable = 1;
	if (!movable) return;
//    movable = !(disBack > 3 || dem >= sturn);
//    if (movable)
//    {
	if(disFront>11){//sap dam vao tuong
        real_thang();
    }else{
      _kmotor.stop();
      delay(50);
      if(turn[dem]==0){
        trai();delay(times);_kmotor.stop();
        //while(!(disFront>10)){trai();disFront=getDistance(2,12);}
      }
      if(turn[dem]==1){
        thang();delay(times);_kmotor.stop();
        //while(!(disFront>10)){thang();disFront=getDistance(2,12);}
      }
      if(turn[dem]==2){
        phai();delay(times);_kmotor.stop();
        //while(!(disFront>10)){phai();disFront=getDistance(2,12);}
      }
      if(turn[dem]==3){
        lui();delay(times);_kmotor.stop();
        //while(!(disFront>10)){lui();disFront=getDistance(2,12);}
      }
      dem=dem+1;
      if (state == 'U' || dem >= sturn)
      {
      	_kmotor.stop();
      	dem = 0; movable = 0;
      }
    }
    analogWrite(A1,0);
    analogWrite(A2,0);
    analogWrite(A3,0);
//    }
}

//void tinh(){
//    disFront=getDistance(2,12);
//    disLeft=getDistance(4,5);
//    disRight=getDistance(11,13);
//    disBack=getDistance(9,10);
//    
//    Serial.println(disFront);
//    Serial.println(disLeft);
//    Serial.println(disRight);
//    Serial.println(disBack);
//    if(disFront>11){//sap dam vao tuong
//        real_thang();
//    }else{
//      _kmotor.stop();
//      delay(50);
//      if(turn[dem]==0){
//        trai();delay(times);_kmotor.stop();
//        //while(!(disFront>10)){trai();disFront=getDistance(2,12);}
//      }
//      if(turn[dem]==1){
//        thang();delay(times);_kmotor.stop();
//        //while(!(disFront>10)){thang();disFront=getDistance(2,12);}
//      }
//      if(turn[dem]==2){
//        phai();delay(times);_kmotor.stop();
//        //while(!(disFront>10)){phai();disFront=getDistance(2,12);}
//      }
//      if(turn[dem]==3){
//        lui();delay(times);_kmotor.stop();
//        //while(!(disFront>10)){lui();disFront=getDistance(2,12);}
//      }
//      dem=dem+1;
//      while (dem >= sturn) _kmotor.stop();
//    }
//    analogWrite(A1,0);
//    analogWrite(A2,0);
//    analogWrite(A3,0);
//    
//}

void loop()
{
	state = (Serial.available() > 0)? Serial.read(): 0;
	if (state == 'U')
	{
		for (int i = 0; i < sturn; i++) Serial.print(turn[i]);
		Serial.println();
	}
	if (state == 'Y')
	{
		record_mode = !record_mode;
		if (record_mode) Serial.println("Record enabled.");
		else Serial.println("Record disabled.");
	}
	if (state == 't')
	{
		Serial.print("Deleted char "); Serial.println(sturn);
		sturn--; sturn = max(sturn, 0);
		
	}
	if (state == 'O') 
	{
		auto_mode = !auto_mode; record_mode = 0;
		if (auto_mode) Serial.println("Auto mode enabled! Pls take your car to the start pos, and wait for the line sensor car come to...");
	}
	if (record_mode) read_turn();
	else if (!auto_mode) blue_control();
	else auto_control();
}
