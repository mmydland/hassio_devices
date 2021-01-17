#include <ESP8266WiFi.h>
#include <DebugLog.h> 
#include "HaRemoteMqtt.hpp"
#include "esp12f_xmas_settings.h"

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient espClient;
HaRemoteMqtt haRemoteClient(espClient);


void setup_wifi()
{

  delay(10);

  // We start by connecting to a WiFi network
  PRINTLN("Connecting to ", WIFI_SSID);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    PRINT(".");
  }
  PRINTLN("", "WiFi connected", "IP address: ", WiFi.localIP());
}

void setup()
{
  // Setup serial port
  Serial.begin(115200);
  rcSetup();
  setup_wifi();
  haRemoteClient.setup();

}

uint32_t x = 0;
void loop()
{
  haRemoteClient.loop();
}