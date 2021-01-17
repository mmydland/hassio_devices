#ifndef ESP12F_XMAS_SETTINGS_H
#define ESP12F_XMAS_SETTINGS_H

// uncomment NDEBUG to disable debug logging
//#define NDEBUG

// WIFI CREDENTIALS
#define WIFI_SSID "FOURTOONE"
#define WIFI_PASS "whoisjohngalt"


//MQTT SETTINGS

#define MQTT_BROKER_IP 192,168,1,8
#define MQTT_PORT 1883
IPAddress mqttServer(MQTT_BROKER_IP);


#define MQTT_DEVICE_ID "remoteController"
#define MQTT_USERNAME "mqttclient"
#define MQTT_PASSWORD "whoisjohngalt"

// MQTT TOPICS
#define MQTT_TOPIC_BASE "homeassistant/switch/xmas_remote"
#define ID_BASE "xmas_tree_remote_"
#define NAME_BASE "xmas tree remote "

//REMOTE CONTROL SETTINGS
#define RC_FUNCTIONS_COUNT 3
#define RC_FUNCTIONS {"all", "clear", "multi"}

#define UPDATE_INTERVAL 1000
#endif