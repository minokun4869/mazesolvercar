#include "kmotor.h"

int mdig[] = {7, 8}, manl[] = {6, 3};

Kmotor::Kmotor(bool msg)
{
    _msg = msg;
}

void Kmotor::init()
{
    for (int i = 0; i < 2; i++)
    {
        pinMode(mdig[i], OUTPUT); pinMode(manl[i], OUTPUT);
    }
}

void Kmotor::p(int cnt, int spd)
{
    digitalWrite(mdig[cnt], (spd >= 0));
    analogWrite(mdig[cnt], abs(spd));
}

void Kmotor::r(int mode, int spd)
{
    switch(mode)
    {
        case 0: // fwd
            this.p(0, spd);
            this.p(1, spd);
            break;
        case 1: //bwd
            this.p(0, -spd);
            this.p(1, -spd);
            break;
        case 2: //turn left
            this.p(0, -spd);
            this.p(1, spd);
            break;
        case 3:
            this.p(0, spd);
            this.p(1, -spd);
    }
}

void Kmotor::s()
{
    this.r(0, 0);
}