#include "Temp.h"

Temp::Temp(PinName sda, PinName scl): i2c(sda,scl)
{

}

float Temp::read(int addr8bit, char *cmd, float temp)
{
    cmd[0] = 0x01;
    cmd[1] = 0x00;
    i2c.write(addr8bit, cmd, 2);

    ThisThread::sleep_for(500);

    cmd[0] = 0x00;
    i2c.write(addr8bit, cmd, 1);
    i2c.read(addr8bit, cmd, 2);
    temp = (float((cmd[0] << 8) | cmd[1]) / 128.0);
    return temp;
}