#include <RCSwitch.h>
#include <DebugLog.h>


#define MULTI 13440
#define CLEAR 13376
#define ALL 13328
#define OFF 13320

#define TRANSMIT_BIT_LENGTH 15
#define PULSE_LENGTH 128
#define PROTOCOL 1
#define GPIO_TRANSMIT_PIN 5 //D1 on the Wemos D1 mini

RCSwitch mySendSwitch = RCSwitch();

void rcSetup()
{
    mySendSwitch.enableTransmit(GPIO_TRANSMIT_PIN);
    mySendSwitch.setRepeatTransmit(20);
    mySendSwitch.setProtocol(PROTOCOL);
    mySendSwitch.setPulseLength(PULSE_LENGTH);

    Serial.print("rc setup complete\n");
}

void sendRemoteCodeClear()
{
    PRINTLN("Send remote code clear");
    sendRemoteCode(CLEAR);
}

void sendRemoteCodeAll()
{
    PRINTLN("Send remote code all");
    sendRemoteCode(ALL);
}

void sendRemoteCodeMulti()
{
    PRINTLN("Send remote code multi");
    sendRemoteCode(MULTI);
}

void sendRemoteCode(int setting)
{
    for (size_t i = 0; i < 5; i++)
    {
        mySendSwitch.send(setting, TRANSMIT_BIT_LENGTH);
        delay(200);
    }
}