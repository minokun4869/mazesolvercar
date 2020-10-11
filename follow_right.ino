#include <Arduino.h>
#include <Wire.h>
#include "kmotor.h"
#include <Encoder.h>

const long fwd = 100, turn = 150, maxDis = 45, maxBor = 10, Bor = 22;
bool isSt = 1;
kmotor mt(1);
long left, right;
int street_forward = 0, street_left = 1, street_right = 2, street_fork = 3, street_cross_road = 4, street_end = 5;
int direction = -1, pre_state = street_forward, curr_state = street_forward;
int step;
char step_arr[100];
char optimize_arr[100];
int number_step = 0;
bool is_first_step = false;
double disF, disL, disR;
    




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
    return dis;
}

int getDirection()
{
    if (disF >= 20 && disL > 20 && disR > 20)
        return street_cross_road;
    if (disF < 20 && disL > 20 && disR > 20)
        return street_fork;
    if (disF < 20 && disL > 20 && disL < 20)
        return street_left;
    if (disF < 20 && disL < 20 && disR > 20)
        return street_right;
    if ((disF > 20 && disL < 20 && disR < 20) || (disF > 20 && disL < 20 && disR > 20))
        return street_forward;
    if (disF < 20 && disL < 20 && disR < 20)
        return street_end;
}

void updateDistance()
{
    disF = getDis(2, 12);
    disL = getDis(4, 5);
    disR = getDis(11, 13);
}

void mfwd()
{
    left = constrain(fwd, 0, 255);
    right = constrain(fwd, 0, 255);
    mt.tien(0, left);
    mt.tien(1, right); 
   
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





void buildpath()
{
    updateDistance();
    direction = getDirection();
    if(direction != street_forward)
    {
        
        if(direction == street_left || direction == street_fork || direction == street_cross_road)
        {
            step_arr[number_step] = 'L';
            mleft();
        }
        if(direction == street_right)
        {
            step_arr[number_step] = 'R';
            mright();
        }
        if(direction == street_end)
        {
            step_arr[number_step] = 'B';
            mback();
        }
        number_step++;
    }
    else
    {
        while(pre_state == street_forward && curr_state == street_forward)
        {
            mfwd();
            updateDistance();
            direction = getDirection();
            pre_state = curr_state;
            curr_state = direction;
        }
        step_arr[number_step] = "F";
        number_step++;
    }
    Serial.print(step_arr[number_step-1]);
    
    


   
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
