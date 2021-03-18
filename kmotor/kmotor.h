#ifndef kmotor_h
#define kmotor_H
#include <Arduino.h>
class Kmotor
{
    public:
        Kmotor(bool msg);
        void init();
        void p(int cnt, int spd);
        void r(int mode, int spd);
        void s();
    private:
        bool _msg;
};
#endif
