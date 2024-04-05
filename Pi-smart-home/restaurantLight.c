/** 厨房灯 */
#include "contrlDevices.h"

int restaurantLightOpen(int pinNum)
{
    digitalWrite(pinNum, LOW);
}

int restaurantLightClose(int pinNum)
{
    digitalWrite(pinNum, HIGH);
}

int restaurantLightCloseInit(int pinNum)
{
    pinMode(pinNum, OUTPUT);
    digitalWrite(pinNum, HIGH);
}

int restaurantLightCloseStatus(int status)
{
}

struct Devices restaurantLight = {
    .deviceName = "restaurantLight",
    .pinNum = 24,
    .open = restaurantLightOpen,
    .close = restaurantLightClose,
    .deviceInit = restaurantLightCloseInit,
    .changeStatus = restaurantLightCloseStatus};

struct Devices *addrestaurantLightToDeviceLink(struct Devices *phead)
{
    if (phead == NULL)
    {
        return &restaurantLight;
    }
    else
    {
        restaurantLight.next = phead;
        phead = &restaurantLight;
    }
    return phead;
}