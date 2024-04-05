/** 火焰传感器 */
#include "contrlDevices.h"
#include <stdlib.h>
#include <stdio.h>

int fireIfOrNotInit(int pinNum)
{
    printf("fireIfOrNotInit pinNum = %d\n, pinNum");
    pinMode(pinNum, INPUT);
    // digitalWrite(pinNum, HIGH);
    digitalWrite(pinNum, HIGH);
}

int fireStatusRead(int pinNum)
{
    return digitalRead(pinNum);
}

struct Devices fireIfOrNot = {
    .deviceName = "fireIfOrNot",
    .pinNum = 28,
    .deviceInit = fireIfOrNotInit,
    .readStatus = fireStatusRead};

struct Devices *addFireToDeviceLink(struct Devices *phead)
{
    if (phead == NULL)
    {
        return &fireIfOrNot;
    }
    else
    {
        fireIfOrNot.next = phead;
        phead = &fireIfOrNot;
    }
    return phead;
}