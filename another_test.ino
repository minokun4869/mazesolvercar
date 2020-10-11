#include <Arduino.h>
#include <Wire.h>
#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h>
#include <kmotor.h>

kmotor mt(1);

const int initial_motor_speed = 200;

float error = 0, P = 0, I = 0, D = 0, PID_value = 0;
float Kp = 10, Kd = 30, Ki = 0;
float previous_error = 0, previous_I = 0;
float disF = 1, disL = 1, disR = 1;
long en_val_left,num_val_left=0,pre_val_left=-1;
long en_val_right,num_val_right=0,pre_val_right=-1;

float getDis(int trig, int echo)
{
	pinMode(trig, OUTPUT);

	digitalWrite(trig, 0); delayMicroseconds(2);
	digitalWrite(trig, 1); delayMicroseconds(10);
	digitalWrite(trig, 0);

	pinMode(echo, INPUT);

	float dis = pulseIn(echo, 1, 30000)/58.824; if (!dis) dis = 30;
	return dis;
}

void readLE()
{
  en_val_left=digitalRead(10);
  if(en_val_left==0&&pre_val_left==1)num_val_left++;
  pre_val_left=en_val_left;
//  Serial.print(num_val_left);
//  Serial.print(' ');
  //num_val_left=0;
}

void readRE()
{
  en_val_right=digitalRead(9);
  if(en_val_right==0&&pre_val_right==1)num_val_right++;
  pre_val_right=en_val_right;
//  Serial.println(num_val_right);
  //num_val_left=0;
}

void tleft(int deg)
{
	int t = 12.6*deg/360/6.2*20, tmp = num_val_left;
	while(num_val_left < tmp + t)
	{
		readLE(); readRE();
		mt.tien(0, -150); mt.tien(1, 150);
	}
}

void tright(int deg)
{
	int t = 12.6*deg/360/6.2*20, tmp = num_val_left;
	while(num_val_left < tmp + t)
	{
		readLE(); readRE();
		mt.tien(0, 150); mt.tien(1, -150);
	}
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
  mt.tien(0, left_motor_speed);//M1
  mt.tien(1, right_motor_speed);//M2
}

void bam_trai(){
  error=disL-8;
  calculate_pid();
  motor_control();
}

void bam_phai(){
  error=8-disR;
  calculate_pid();
  motor_control();
}

void tfwd(){
  if(disL>=15&&disR>=15)
  {
  	mt.run(0,200);
//     analogWrite(A3,300);//red
  }
  else
  {
     if(disL<15)
     {
       bam_trai();
//       analogWrite(A2,300);//green
     }
     else
     {
      bam_phai();
//      analogWrite(A1,300);//blue
	 }
  }
}

void test() // used for mesure some distance
{
	delay(2000);
	mt.run(0, 200); delay(500);
	mt.stop();
}

void construn()
{
	disF = getDis(2, 12);
	disL = getDis(4, 5);
	disR = getDis(11, 13);
	Serial.println(disL);
	mt.stop(); delay(150);
	if (disL > 15) // the left path is clear
	{
		tleft(90);
		mt.stop(); delay(150);
		mt.run(0, 200); delay(450);
	}
	else if (disF > 15) // the front path is clear
	{
		mt.run(0, 200); delay(450);
	}
	else 
	{
		tright(90);
	}
	mt.stop(); delay(150);
}

void setup() 
{
	mt.cauhinh(); Serial.begin(9600); delay(1000);
//	test();
}

void loop() 
{
	construn();
//	mt.tien(1, 150);
}
