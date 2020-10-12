#include <Arduino.h>
#include <Wire.h>
#include <SoftwareSerial.h>
#include "kmotor.h"
#include <Servo.h>

#define thresholdFront 12
#define thresholdLeft 20
#define thresholdRight 20

const int timeToTurn90Degrees = 1500;
int tfr = 2750;
int timefr;
int tlbef = 440;
int tlaf = 1150;
int nf = 0;
int nlr = 0;

bool done = false;

int stepArrScan[100];
int stepArrOptimize[100];

int stepIndexScan = 0;
int stepIndexOptimize = -1;

// Front US sensor.
const int trigPinFront = 2;
const int echoPinFront = 12;
// Left US sensor.
const int trigPinLeft = 4;
const int echoPinLeft = 5;
// Right US sensor.
const int trigPinRight = 11;
const int echoPinRight = 13;

bool wallFront = false;
bool wallRight = false;
bool wallLeft = false;

void setup()
{
    Serial.begin(9600);
    // left wheel: 7 - 6, right-wheel 8- 3
    pinMode(3, OUTPUT);
    pinMode(6, OUTPUT);
    pinMode(8, OUTPUT);
    pinMode(7, OUTPUT);

    // sensors
    pinMode(trigPinFront, OUTPUT);
    pinMode(echoPinFront, INPUT);
    pinMode(trigPinRight, OUTPUT);
    pinMode(echoPinRight, INPUT);
    pinMode(trigPinLeft, OUTPUT);
    pinMode(echoPinLeft, INPUT);
    pinMode(4, INPUT);
}

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

void moveForward()
{
    float left_dist = getDis(trigPinLeft, echoPinLeft);
    float left_dist_const = left_dist;
    while (left_dist <= 5)
    {
        digitalWrite(7, HIGH);
        digitalWrite(6, LOW);
        digitalWrite(8, LOW);
        digitalWrite(3, LOW);
        delay(timeToTurn90Degrees / 65);
        left_dist = getDis(trigPinLeft, echoPinLeft);
        if (abs(left_dist_const - left_dist) >= 0.8 || (left_dist >= 3.6))
        {
            break;
        }
    }
    float right_dist = getDis(trigPinRight, echoPinRight);
    float right_dist_const = right_dist;
    while (right_dist <= 5.4)
    {
        digitalWrite(7, LOW);
        digitalWrite(6, LOW);
        digitalWrite(8, HIGH);
        digitalWrite(3, LOW);
        delay(timeToTurn90Degrees / 65);
        right_dist = getDis(trigPinRight, echoPinRight);
        if (abs(right_dist_const - right_dist) >= 0.9)
        {
            break;
        }
    }
    if (getDis(trigPinLeft,echoPinLeft)>= 7.2)
    {
        digitalWrite(7, LOW);
        digitalWrite(6, LOW);
        digitalWrite(8, HIGH);
        digitalWrite(3, LOW);
        delay(timeToTurn90Degrees / 30);
    }
    digitalWrite(7,HIGH);
    digitalWrite(6,LOW);
    digitalWrite(8,HIGH);
    digitalWrite(3,LOW);
}

void turnRight()
{
    float preFrontDist = getDis(trigPinFront, echoPinFront);
    while (abs(getDis(trigPinFront, echoPinFront) - preFrontDist) <= (preFrontDist / 2) - 2)
    {
        moveForward();
        delay(300);
        if (getDis(trigPinFront, echoPinFront) <= 4.5)
        {
            break;
        }
    }
    digitalWrite(7, HIGH);
    digitalWrite(6, LOW);
    digitalWrite(8, LOW);
    digitalWrite(3, HIGH);
    delay(timeToTurn90Degrees);

    preFrontDist = getDis(trigPinFront, echoPinFront);
    while (abs(getDis(trigPinFront, echoPinFront) - preFrontDist) <= 15.2)
    {
        moveForward();
        delay(300);
        if (getDis(trigPinFront, echoPinFront) <= 4.5)
        {
            digitalWrite(7, HIGH);
            digitalWrite(6, LOW);
            digitalWrite(8, LOW);
            digitalWrite(3, HIGH);
            delay(2 * timeToTurn90Degrees);
        }
    }
}

void turnLeft()
{
    moveForward();
    delay(tlbef);

    digitalWrite(7, LOW);
    digitalWrite(6, LOW);
    digitalWrite(8, HIGH);
    digitalWrite(3, LOW);
    delay(2 * timeToTurn90Degrees);
    for (int n = 1; n <= 8; n++)
    {
        moveForward();
        delay(tlaf / 8);
    }
    stop();
    delay(1000);
}

void turnBack(int delayTime)
{
    digitalWrite(7, HIGH);
    digitalWrite(6, LOW);
    digitalWrite(8, LOW);
    digitalWrite(3, HIGH);
    delay(delayTime);
}

void stop()
{
    digitalWrite(7, LOW);
    digitalWrite(6, LOW);
    digitalWrite(8, LOW);
    digitalWrite(3, LOW);
}

void getDone()
{
    stop();
    delay(1000);
    float prevfdist = getDis(trigPinFront, echoPinFront);
    //while( abs(frontdist()-prevfdist)<=(prevfdist/2)-1)
    for (int n = 1; n <= 5; n++)
    {
        moveForward();
        delay(260);
    }
    digitalWrite(7, HIGH);
    digitalWrite(6, LOW);
    digitalWrite(8, LOW);
    digitalWrite(3, HIGH);
    delay(timeToTurn90Degrees);
    // gofront();delay(2400);
    float prevfrdist = getDis(trigPinFront, echoPinFront);
    while (abs(getDis(trigPinFront, echoPinFront) - prevfrdist) <= 18)
    /* for(int n=1;n<=10;n++)*/
    {
        moveForward();
        delay(300);
    }
    digitalWrite(7, LOW);
    digitalWrite(6, LOW);
    digitalWrite(8, LOW);
    digitalWrite(3, LOW);
    delay(1000);
}

void reducePath(int arr, int *pointer)
{
    for(int i=0; i<10; i++)
    {
      
    }
}

void loop()
{
    // 0: forward
    // 1: turn left
    // 2: turn right
    // 3: turn back
    float distFront = getDis(trigPinFront, echoPinFront);
    float distRight = getDis(trigPinRight, echoPinRight);
    float distLeft = getDis(trigPinLeft, echoPinLeft);

    Serial.println(distFront);
    Serial.println(distRight);
    Serial.println(distLeft);

    if (distFront <= thresholdFront)
        wallFront = true;
    if (distLeft <= thresholdLeft)
        wallLeft = true;
    if (distRight <= thresholdRight)
        wallLeft = true;
    Serial.println(wallFront);
    Serial.println(wallRight);
    Serial.println(wallLeft);

    if (wallLeft == true)
    {
        if (wallFront == true && wallRight == true)
        {
            turnBack(timeToTurn90Degrees/4);
            stepIndexScan++;
            stepIndexOptimize++;
            stepArrScan[stepIndexScan] = 3;
            stepArrOptimize[stepIndexOptimize] = 3;
            reducePath(stepArrOptimize, stepIndexOptimize);
        }
        else if (wallFront == false)
        {
            if ((wallRight == false) && (distFront >= 20))
            {
                stepIndexScan++;
                stepIndexOptimize++;
                if ((done == true) && stepArrOptimize[stepIndexOptimize] != 0)
                {
                    turnRight();
                    return 1;
                }
                else
                {
                    if (done == false)
                    {
                        stepArrScan[stepIndexScan] = 0;
                        stepArrOptimize[stepIndexOptimize] = 0;
                        reducePath(stepArrOptimize, stepIndexOptimize);
                    }
                    timefr = tfr + 65 * nf;
                    nf = nf + 1;
                    stop();
                    delay(1000);
                    for (int i = 1; i <= 10; i++)
                    {
                        moveForward();
                        delay(timefr / 10);
                    }
                    stop();
                    delay(1000);
                }
            }
            else
            {
                moveForward();
                delay(300);
            }
        }
        else
        {
            stepIndexScan++;
            stepIndexOptimize++;
            stepArrScan[stepIndexScan] = 2;
            stepArrOptimize[stepIndexOptimize] = 2;
            reducePath(stepArrOptimize, stepIndexOptimize);
            turnRight();
        }
    }
    else
    {
        stepIndexScan++;
        stepIndexOptimize++;
        if ((done == true) && (stepArrOptimize[stepIndexOptimize] != 1))
        {
            if ((stepArrOptimize[stepIndexOptimize] == 2) && (getDis(trigPinRight, echoPinRight) >= 10))
            {
                getDone();
                return;
            }
            else if ((stepArrOptimize[stepIndexOptimize] == 0) && (wallFront == false))
            {
                getDone();
                return;
            }
        }
        else
        {
            stepArrOptimize[stepIndexOptimize] = 1;
            nlr = nlr + 1;
            reducePath(stepArrOptimize, stepIndexOptimize);
        }
    }
    delay(320);
}
