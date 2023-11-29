#ifndef TEMP_H
#define TEMP_H
#include "mbed.h"

class Temp {
    public:
        float temp;
        char cmd[2];

        const int addr7bit = 0x48;      // 7 bit I2C address
        const int addr8bit = 0x48 << 1; // 8bit I2C address, 0x90

        float read(int addr8bit, char *cmd, float temp);

        Temp(PinName sda, PinName scl);

        //Temp Temp(I2C_SDA, I2C_SCL);
        //PinName sda = P1_I2C_SDA;
        //PinName scl = P1_I2C_SCL;

    private:
        I2C i2c;
        

};

#endif