#ifndef CURRENT_TRANSFORMER_H
#define CURRENT_TRANSFORMER_H

#include <DebugLog.h>
#include <Oversampling.h>
#include <Arduino.h>
#include "enum_switch_state.h"
#define VOLTAGE_THRESHOLD 0.9
#define ANALOG_PIN A0
#define CT_MULTI_PIN D6
#define CT_CLEAR_PIN D5

class CurrentTransformer
{

private:
    int lastReadMillis = 0;
    bool isReadMulti = false;
    bool isMultiOn = false;
    bool isClearOn = false;
    bool getIsCurrentOn();
    void setCurrentState();

public:
    void setup();
    void loop();
    switch_state get_state();
};

void CurrentTransformer::setup()
{
    PRINTLN("current log level is", (int)LOG_GET_LEVEL());
}

void CurrentTransformer::loop()
{
    if (millis() - lastReadMillis > 300)
    {
        
            pinMode(CT_MULTI_PIN, INPUT);
            pinMode(CT_CLEAR_PIN, OUTPUT);

            PRINT("Read CT Multi: ");
            isMultiOn = getIsCurrentOn();

            PRINTLN(isMultiOn ? "On" : "Off");
            
            pinMode(CT_CLEAR_PIN, INPUT);
            pinMode(CT_MULTI_PIN, OUTPUT);
            // delay(200);
            yield();

            PRINT("Read CT Clear: ");
            isClearOn = getIsCurrentOn();

            PRINTLN(isClearOn ? "On" : "Off");
            lastReadMillis = millis();
    }
}

bool CurrentTransformer::getIsCurrentOn()
{
    float result;
    int readValue;    //value read from the sensor
    int maxValue = 0; // store max value here
    uint32_t start_time = millis();

    maxValue = 0;
    start_time = millis();

    while ((millis() - start_time) < 1000)
    {
        readValue = analogRead(ANALOG_PIN);
        if (readValue > maxValue)
        {
            maxValue = readValue;
        }
    }

    result = (maxValue * 50.0) / 1024.0;
    PRINTLN("Voltage is: ", result);
    return result > VOLTAGE_THRESHOLD;
}

switch_state CurrentTransformer::get_state()
{
    if (isClearOn && isMultiOn)
    {
        return ALL;
        PRINTLN("switch_state: ALL");
    }

    if (isClearOn)
    {
        return CLEAR;
        PRINTLN("switch_state: CLEAR");
    }

    if (isMultiOn)
    {
        return MULTI;
        PRINTLN("switch_state: MULTI");
    }

    PRINTLN("switch_state: OFF");
    return OFF;
}

#endif