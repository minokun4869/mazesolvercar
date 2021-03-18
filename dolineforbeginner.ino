#include <Arduino.h>
#include <QTRSensors.h>
#include "kmotor.h"

const float Kp = 0.1, Kd = 5;
static int lerr = 0;
const int intial_speed = 225;

Kmotor _motor(1);
QTRSensors qtr((unsigned char[]) { A0, A1, A2, A3, A4}, 6);

void calibration()
{
    for (int i = 0; i < 250; i++)
    {
        qtr.calibrate();
        delay(20);
    }
}

void setup()
{
    _motor.init();
    Serial.begin(9600);
    Serial.println("Prepare for calibrating..."); delay(5000);
    Serial.println("Calibrating...");
    calibration();
}

void loop()
{
    unsigned int sens[5];
    double pos = qtr.readLineBlack(sens),
        err = pos - 1000,
        spd = Kp*err + Kd*(err - lerr);
    lerr = err;

    double sM1 = min(max(intial_speed + err, 0), 255), sM2 = min(max(intial_speed - err, 0), 255);
    _motor.s(0, sM1); _motor.s(1, sM2);
    delay(50);
}