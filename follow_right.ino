#include <Arduino.h>
#include <Wire.h>
#include "kmotor.h"
#include <Encoder.h>

const long fwd = 100, turn = 150, maxDis = 45, maxBor = 10, Bor = 22;

bool isSt = 1;

kmotor mt(1);
long left, right;
double disF, disL, disR, left_delta, right_delta, offset;


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
    left_delta = 1.5 * disL;
    right_delta = 1.5 * disR;
    offset = disL - disR;
}

void delayForward()
{
    int i = 0;
    while(i<=10){
        mfwd();
        updateDistance();
        i++;
    }
}


void buildpath()
{
    updateDistance();
    Serial.print(disF);
    Serial.print(' ');
    Serial.print(disL);
    Serial.print(' ');
    Serial.println(disR);

    // double right_delta = 1.5 * disR;
    // double left_delta = 1.5 * disL;
    // double offset = disL - disR;
    if (offset > 0)
    {
        if (offset > right_delta)
        {
            while (disF > 10)
            {
                updateDistance();
                mfwd();
            }
            if (offset > right_delta)
            {
                mleft();
                delayForward();

            }
            else
            {
                mback();
                while (-offset < left_delta)
                {
                    updateDistance();
                    mfwd();
                }
                mright();
                delayForward();
            }
        }
        else
        {
            mfwd();
        }
    }
    else if (offset < 0)
    {
        if (-offset > left_delta)
        {
            while (disF > 10)
            {
                updateDistance();
                mfwd();
            }
            if (-offset > left_delta)
            {
                mright();
                delayForward();
            }
            else
            {
                mback();
                while(offset < right_delta){
                    updateDistance();
                    mfwd();
                }
                mleft();
                delayForward();
            }
        }
        else
        {
            mfwd();
        }
    }
    else
    {
        mfwd();
    }

    //    if(disL < 12 && disF < 12)
    //    {
    //
    //    }
    // if (disL > disR)
    // {
    //     alterl = 0;
    //     alterr = 20;
    // }
    // else if (disL < disR)
    // {
    //     alterl = 20;
    //     alterr = 0;
    // }
    // else
    //     alterl = alterr = 0;
    // mfwd();
    // if (disL > maxBor)
    // {
    //     if (disF <= 10 || (disF >= 36 && disF <= 37.5))
    //         mleft();
    // }
    // if (disL <= maxBor && disF <= maxBor && disR > maxBor)
    // {
    //     if (disF <= 10 || (disF >= 36 && disF <= 37.5))
    //         mright();
    // }
    // if (disL <= 10 && disR <= 10 && disF <= 10)
    //     mback();
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

void loop()
{
    buildpath();
}
