/** 卧室灯 */
#include "contrlDevices.h"

int livingLightOpen(int pinNum)
{
    digitalWrite(pinNum, LOW);
}

int livingLightClose(int pinNum)
{
    digitalWrite(pinNum, HIGH);
}

int livingLightCloseInit(int pinNum)
{
    pinMode(pinNum, OUTPUT);
    digitalWrite(pinNum, HIGH);
}

int livingLightCloseStatus(int status)
{
}

struct Devices livingLight = {
    .deviceName = "livingLight",
    .pinNum = 23,
    .open = livingLightOpen,
    .close = livingLightClose,
    .deviceInit = livingLightCloseInit,
    .changeStatus = livingLightCloseStatus};

struct Devices *addLivingLightToDeviceLink(struct Devices *phead)
{
    if (phead == NULL)
    {
        return &livingLight;
    }
    else
    {
        livingLight.next = phead;
        phead = &livingLight;
    }
    return phead;
}