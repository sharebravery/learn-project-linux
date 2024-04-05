/** 外设设备的头文件  */
#include <wiringPi.h>
#include <stdlib.h>

struct Devices
{
    char deviceName[128];
    int status;
    int pinNum;

    int (*open)(int pinNum);
    int (*close)(int pinNum);
    int (*deviceInit)(int pinNum);

    int (*readStatus)(int pinNum);
    int (*changeStatus)(int status);

    struct Devices *next;
};

struct Devices *addBathroomLightToDeviceLink(struct Devices *phead);
struct Devices *addUpstairLightToDeviceLink(struct Devices *phead);
struct Devices *addLivingLightToDeviceLink(struct Devices *phead);
struct Devices *addrestaurantLightToDeviceLink(struct Devices *phead);
struct Devices *addPoolLightToDeviceLink(struct Devices *phead);
struct Devices *addFireToDeviceLink(struct Devices *phead);