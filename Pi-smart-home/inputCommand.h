/** 控制的头文件 */
#include <wiringPi.h>
#include <stdlib.h>

struct InputCommander
{
    char commandName[128];
    char command[32];
    char deviceName[128];

    int (*Init)(struct InputCommander *voicer, char *ipAdress, char *port);
    int (*getCommand)(struct InputCommander *voicer);

    char log[1024];
    int fd;
    char port[12];
    char ipAddress[32];
    int sfd;

    struct InputCommander *next;
};

struct InputCommander *addvoiceContrlLightToDeviceLink(struct InputCommander *phead);
struct InputCommander *addsocketContrlLightToDeviceLink(struct InputCommander *phead);