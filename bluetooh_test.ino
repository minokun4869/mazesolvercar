#include <Arduino.h>
#include <Wire.h>
#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h>
#include "kmotor.h"
#include <Servo.h>

kmotor _kmotor(true);
char state;
bool auto_mode = 0;
bool record_mode = 0;
bool turn[1024], record;
int count = 0;
long en_val_left, num_val_left = 0, pre_val_left = -1;
long en_val_right, num_val_right = 0, pre_val_right = -1;

void forward()
{
	_kmotor.run(0, 100);
}

void backward()
{
	_kmotor.run(1, 100);
}

void tleft()
{
	_kmotor.tien(0, -100);
	_kmotor.tien(1, 100);
}

void tright()
{
	_kmotor.tien(0, 100);
	_kmotor.tien(1, -100);
}

void fleft()
{
	_kmotor.tien(0, 50);
	_kmotor.tien(1, 100);
}

void fright()
{
	_kmotor.tien(0, 100);
	_kmotor.tien(1, 50);
}

void bleft()
{
	_kmotor.tien(0, -50);
	_kmotor.tien(1, -100);
}

void bright()
{
	_kmotor.tien(0, -100);
	_kmotor.tien(1, -50);
}

void control(char state)
{
	switch(state)
	{
	case 'F':
		forward(); break;
	case 'B':
		backward(); break;
	case 'L':
		tleft(); break;
	case 'R':
		tright(); break;
	case 'C':
		tleft(); break;
	case 'M':
		tright(); break;
	case 'P':
		bright(); break;
	case 'E':
		bleft(); break;
	case 'S':
		_kmotor.stop(); break;
	}
}

void auto_control()
{
	// not yet
}

void bluetooh()
{
	state = (Serial.available() > 0)? Serial.read(): 0;
//	Serial.println(state);
	if (state == 'Y')
		record_mode = !record_mode;
	if (state == 'U' || state == 'u') auto_mode = 0;
	if (state == 'O' || state == 'o') auto_mode = 1;
	if (record_mode)
	{
		Serial.println("Record mode enabled");
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
	if (state == 'T')
	{
		for (int i = 0; i < count; i++)
		{
			Serial.print(turn[i]); Serial.print(' ');
		}
		Serial.println();
	}
}

void setup()
{
	_kmotor.cauhinh();
	Serial.begin(9600);
}

void loop()
{
	bluetooh();
}
