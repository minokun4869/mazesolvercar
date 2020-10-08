#include <Arduino.h>
#include <Wire.h>
#include "kmotor.h"
#include <Encoder.h>

const long fwd = 100, turn = 150, maxDis = 55, maxBor = 12, Bor = 22;

bool isSt = 1;
kmotor mt(1);
long left, right, alterl, alterr;
String path; char dir[] = {'U', 'L', 'D', 'R'};
//int mtleft = A4, mtright = A5;

double getDis(int trig, int echo)
{
    pinMode(trig, OUTPUT);
  
    digitalWrite(trig, 0); delayMicroseconds(2);
    digitalWrite(trig, 1); delayMicroseconds(10);
    digitalWrite(trig, 0);

    pinMode(echo, INPUT);
    double dis = pulseIn(echo, 1, 30000)/58.824;
    if (dis > 55) dis = maxDis;
    if (dis <= 3) dis = 3;
    return dis;
}

void mfwd()
{
	left = constrain(fwd + alterl, 0, 255); right = constrain(fwd + alterr, 0, 255);
	mt.tien(0, left); mt.tien(1, right); // delay(500);
//  mt.stop();
}

void mleft()
{
  	left = constrain(fwd, 0, 255); right = constrain(turn, 0, 255);
    mt.tien(0, -left); mt.tien(1, right); delay(300);
    mt.stop();
}

void mleft2()
{
    left = constrain(fwd, 0, 255); right = constrain(turn, 0, 255);
    mt.tien(0, left); mt.tien(1, right); delay(720);
    mt.stop();
}

void mright()
{
  	left = constrain(turn, 0, 255); right = constrain(fwd, 0, 255);
  	mt.tien(0, left); mt.tien(1, -right); delay(300);
    mt.stop();
}

void mright2()
{
    left = constrain(turn, 0, 255); right = constrain(fwd, 0, 255);
    mt.tien(0, left); mt.tien(1, right); delay(720);
    mt.stop();
}

void bam()
{
    
}

void mback()
{
    left = constrain(turn, 0, 255); right = constrain(fwd, 0, 255);
    mt.tien(0, left); mt.tien(1, -right); delay(700);
    mt.stop();
}

void buildpath()
{
    double disF = min(getDis(2, 12), maxDis),
           disL = min(getDis(4, 5), maxDis),
           disR = min(getDis(11, 13), maxDis);
    Serial.print(disF); Serial.print(' ');
    Serial.print(disL); Serial.print(' ');
    Serial.println(disR);
    if(disL < 15 && disF < 15)
    {
      if (disL > disR)
      {
          alterl = 0; alterr = 20;
      }
      else if (disL < disR)
      {
          alterl = 20; alterr = 0;
      }
      else alterl = alterr = 0;
    }
    
    mfwd();
    if (disL > maxBor)
    {
        if (disF <= 10 || (disF >= 36 && disF <= 37.5)) mleft();
    }
    if (disL <= maxBor && disF <= maxBor && disR > maxBor)
    {
        if (disF <= 10 || (disF >= 36 && disF <= 37.5)) mright();
    }
    if (disL <= 10 && disR <= 10 && disF <= 10) mback();
//    if (disF <= 10) mt.stop();
}

void setup()
{
    mt.cauhinh();
    Serial.begin(9600);
//    pinMode(mtleft, INPUT); pinMode(mtright, INPUT);
    delay(1500);
//	  mright2();
//    mright();
//    left = constrain(left, 0, 255); right = constrain(right, 0, 255);
//    mt.tien(0, left); mt.tien(1, right); delay(1000);
//    mt.stop();
//    mt.tien(0, -left); mt.tien(1, right); delay(230);
//    mt.stop();
}


//void alternative()
//{
//	  
//    if (isSt && disL <= maxBor && disR <= maxBor)
//    {
//		if (disL < disR) mfwd(10, 0);
//    else if (disL > disR) mfwd(0, 10);
//		else mfwd(0, 0);
//    }
//	
//}

void loop()
{
	  buildpath();
    Serial.println("Connected");
}
