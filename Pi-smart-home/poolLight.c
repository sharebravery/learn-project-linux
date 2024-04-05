/** 泳池灯 */
#include "contrlDevices.h"
#include <stdlib.h>

int poolLightOpen(int pinNum)
{
    digitalWrite(pinNum, LOW);
}

int poolLightClose(int pinNum)
{
    digitalWrite(pinNum, HIGH);
}

int poolLightCloseInit(int pinNum)
{
    pinMode(pinNum, OUTPUT);
    digitalWrite(pinNum, HIGH);
}

int poolLightCloseStatus(int status)
{
}

struct Devices poolLight = {
    .deviceName = "poolLight",
    .pinNum = 28,
    .open = poolLightOpen,
    .close = poolLightClose,
    .deviceInit = poolLightCloseInit,
    .changeStatus = poolLightCloseStatus};

struct Devices *addPoolLightToDeviceLink(struct Devices *phead)
{
    if (phead == NULL)
    {
        return &poolLight;
    }
    else
    {
        poolLight.next = phead;
        phead = &poolLight;
    }
    return phead;
}