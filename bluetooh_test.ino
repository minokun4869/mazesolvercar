#include <Arduino.h>
#include <Wire.h>
#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h>
#include <kmotor.h>
#include <Servo.h>

const int motor_base = 200;

kmotor _kmotor(true);
char state, last;
bool auto_mode = 0;
bool record_mode = 0;
int turn[512], record;
int count = 0, cnt = 0;
float disF, disL, disR;
long en_val_left, num_val_left = 0, pre_val_left = -1;
long en_val_right, num_val_right = 0, pre_val_right = -1;

float Dis(int trig, int echo)
{
	pinMode(trig, OUTPUT);

	digitalWrite(trig, 0); delayMicroseconds(2);
	digitalWrite(trig, 1); delayMicroseconds(10);
	digitalWrite(trig, 0);

	pinMode(echo, INPUT);
	return pulseIn(echo, 1, 30000)/58.824;
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
		_kmotor.tien(0, -150); _kmotor.tien(1, 150);
	}
}

void tright(int deg)
{
	int t = 12.6*deg/360/6.2*20, tmp = num_val_left;
	while(num_val_left < tmp + t)
	{
		readLE(); readRE();
		_kmotor.tien(0, 150); _kmotor.tien(1, -150);
	}
}

void forward()
{
	_kmotor.run(0, motor_base);
}

void backward()
{
	_kmotor.run(1, motor_base);
}

void tleft()
{
	_kmotor.tien(0, -motor_base);
	_kmotor.tien(1, motor_base);
	delay(225); _kmotor.stop();
}

void tright()
{
	_kmotor.tien(0, motor_base);
	_kmotor.tien(1, -motor_base);
	delay(225); _kmotor.stop();
}

void fleft()
{
	_kmotor.tien(0, -motor_base/2);
	_kmotor.tien(1, motor_base);
}

void fright()
{
	_kmotor.tien(0, motor_base);
	_kmotor.tien(1, -motor_base/2);
}

void bleft()
{
	_kmotor.tien(0, -motor_base/2);
	_kmotor.tien(1, motor_base);
}

void bright()
{
	_kmotor.tien(0, motor_base);
	_kmotor.tien(1, -motor_base/2);
}

void control(char state)
{
	switch(state)
	{
	case 'F': forward(); break;
	case 'B': backward(); break;
	case 'C': fleft(); break;
	case 'M': fright(); break;
	case 'P': bright(); break;
	case 'E': bleft(); break;
	case 'S': 
		_kmotor.stop(); delay(50);
		switch (last)
		{
			case 'L': tleft(); break;
			case 'R': tright(); break;
		}
		break;
	}
}

void auto_control()
{
	do
	{
		disL = Dis(4, 5); disR = Dis(11, 13);
		_kmotor.run(0, 175);
	}
	while (disL > 8 && disR > 8);
	_kmotor.stop();
	switch (turn[cnt])
	{
	case 1:
		tright(90); _kmotor.stop(); delay(50);
		_kmotor.run(0, 175); delay(300);
		_kmotor.stop(); break;
	case 0:
		tleft(90); _kmotor.stop(); delay(50);
		_kmotor.run(0, 175); delay(300);
		_kmotor.stop(); break;
	}
	cnt++;
}

void bluetooh()
{
	last = state;
	state = (Serial.available() > 0)? Serial.read(): 0;
	Serial.print("Read char: "); Serial.println(state);
	if (state == 'Y')
	{
		record_mode = !record_mode; 
		if (record_mode) Serial.println("Record mode enabled");
	}
	if (state == 'U' || state == 'u') auto_mode = 0;
	if (state == 'O' || state == 'o') auto_mode = 1;
	if (record_mode)
	{
		switch(state)
		{
		case 'L':
			record = 0; break;
		case 'R':
			record = 1; break;
		case 'S':
			turn[count++] = record; break;
		}
	}
	else if (!auto_mode)
		control(state);
	else 
	{
		Serial.println("In auto...");
		auto_control();
	}
}

void setup()
{
	_kmotor.cauhinh();
	Serial.begin(9600); Serial.println("Init completed."); delay(250);
}

void loop()
{
	bluetooh();
}
