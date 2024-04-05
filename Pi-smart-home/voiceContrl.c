/** 语音模块 */
#include <stdio.h>
#include <stdlib.h>
#include <wiringSerial.h>
#include <wiringPi.h>
#include <string.h>
#include <unistd.h>
#include "InputCommand.h"

int voiceGetCommand(struct InputCommander *voicer)
{
    int nread;
    memset(voicer->command, '\0', sizeof(voicer->command));
    serialGetchar(voicer->fd);
    nread = read(voicer->fd, voicer->command, sizeof(voicer->command));
    return nread;
}

int voiceInit(struct InputCommander *voicer, char *ipAdress, char *port)
{
    int fd;
    if ((fd = serialOpen(voicer->deviceName, 9600)) == -1)
    {
        exit(-1);
    }
    voicer->fd = fd;
    return fd;
}

struct InputCommander voiceContrl = {
    .commandName = "voice",
    .command = {'\0'},
    .deviceName = "/dev/ttyAMA0",
    .Init = voiceInit,
    .getCommand = voiceGetCommand,
    .log = {'\0'},
    .next = NULL};

struct InputCommander *addvoiceContrlLightToDeviceLink(struct InputCommander *phead)
{
    if (phead == NULL)
    {
        return &voiceContrl;
    }
    else
    {
        voiceContrl.next = phead;
        phead = &voiceContrl;
    }
}