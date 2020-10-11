#include <Arduino.h>
#include <Wire.h>
#include "kmotor.h"
#include <Encoder.h>

const long fwd = 100, turn = 150, maxDis = 45, maxBor = 10, Bor = 22;

bool isSt = 1;

kmotor mt(1);
long left, right;
double disF, disL, disR, left_delta, right_delta, offset;

int step_arr[150];

int street_forward = 0, street_left = 1, street_right = 2, street_fork = 3, street_cross_road = 4, street_end = 5;

double getDis(int trig, int echo)
{
    pinMode(trig, OUTPUT);

    digitalWrite(trig, 0);
    delayMicroseconds(2);
    digitalWrite(trig, 1);
    delayMicroseconds(10);
    digitalWrite(trig, 0);

    pinMode(echo, INPUT);
    double dis = pulseIn(echo, 1, 30000) / 58.824;
    // if (dis > 45)
    //     dis = maxDis;
    // if (dis <= 3)
    //     dis = 3;
    return dis;
}

void mfwd()
{
    left = constrain(fwd, 0, 255);
    right = constrain(fwd, 0, 255);
    mt.tien(0, left);
    mt.tien(1, right); // delay(500);
    //  mt.stop();
}

void mleft()
{
    left = constrain(fwd, 0, 255);
    right = constrain(turn, 0, 255);
    mt.tien(0, -left);
    mt.tien(1, right);
    delay(300);
    mt.stop();
}

void mright()
{
    left = constrain(turn, 0, 255);
    right = constrain(fwd, 0, 255);
    mt.tien(0, left);
    mt.tien(1, -right);
    delay(300);
    mt.stop();
}

void mback()
{
    left = constrain(turn, 0, 255);
    right = constrain(fwd, 0, 255);
    mt.tien(0, left);
    mt.tien(1, -right);
    delay(700);
    mt.stop();
}

void updateDistance()
{
    disF = getDis(2, 12);
    disL = getDis(4, 5);
    disR = getDis(11, 13);
    // left_delta = 1.5 * disL;
    // right_delta = 1.5 * disR;
    // offset = disL - disR;
}

void delayForward()
{
    int i = 0;
    while (i <= 10)
    {
        mfwd();
        updateDistance();
        i++;
    }
}

void getDirection()
{
    if (disF >= 20 && disL > 20 && disR > 20)
        return street_cross_road;
    if (disF < 10 && disL > 20 && disR > 20)
        return street_fork;
    if (disF < 20 && disL > 20 && disL < 20)
        return street_left;
    if (disF < 20 && disL < 20 && disR > 20)
        return street_right;
    if (disF > 20 && disL < 20 && disR < 20)
        return street_forward;
    if (disF < 20 && disL < 20 && disR < 20)
        return street_end;
}

void buildpath()
{
    updateDistance();
    // Serial.print(disF);
    // Serial.print(' ');
    // Serial.print(disL);
    // Serial.print(' ');
    // Serial.println(disR);
    int direction;
    direction = getDirection();
    if(direction == street_cross_road || direction == street_fork || direction == street_left)
        mleft();
    else if (direction == street_right)
        mright();
    else if (direction == street_forward)
        mfwd();
    else
        mback();
    
        


    // double right_delta = 1.5 * disR;
    // double left_delta = 1.5 * disL;
    // double offset = disL - disR;
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

void loop()
{
    buildpath();
}
