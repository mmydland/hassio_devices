#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DebugLog.h>
#include "esp12f_xmas_settings.h"
#include <StringSplitter.h>
#include "current_transformer.hpp"
#include "enum_switch_state.h"

void sendRemoteCodeClear();
void sendRemoteCodeAll();
void sendRemoteCodeMulti();

void callback(char *topic, byte *payload, unsigned int length)
{
  PRINTLN("Message arrived [", topic, "]");
  char str_payload[length];
  for (int i = 0; i < length; i++)
  {
    str_payload[i] = (char)payload[i];
  }
  str_payload[length] = '\0';
  PRINTLN("Payload is:", String(str_payload));

  String topic_name(topic);
  StringSplitter splitter(topic_name, '/', 10);
  int num_nodes = splitter.getItemCount();
  if (splitter.getItemAtIndex(num_nodes - 1) == "set")
  {
    String mode = splitter.getItemAtIndex(num_nodes - 2);
    if (mode == "clear")
    {
      sendRemoteCodeClear();
    }
    else if (mode == "multi")
    {
      sendRemoteCodeMulti();
    }
    else if (mode == "all")
    {
      sendRemoteCodeAll();
    }
  }
}

class HaRemoteMqtt
{
private:
  /* data */
  PubSubClient pubsub_client;
  CurrentTransformer currentTransformer;
  switch_state statelastReported = NONE;
  int msLastReported = millis();
  char *function_names[RC_FUNCTIONS_COUNT] = RC_FUNCTIONS;
  void discover();
  void reconnect();
  void publish(const char *, const char *);
  void subscribe_trigger_topics();
  String buildConfigTopic(String);
  String build_config_payload(String);
  String build_state_topic(String);
  void update_switch_state();

public:
  HaRemoteMqtt(WiFiClient &);
  ~HaRemoteMqtt();
  void setup();
  void loop();
};

HaRemoteMqtt::HaRemoteMqtt(WiFiClient &wifi_client)
{
  pubsub_client = PubSubClient(wifi_client);
  pubsub_client.setServer(mqttServer, MQTT_PORT);
  pubsub_client.setCallback(callback);
}

HaRemoteMqtt::~HaRemoteMqtt()
{
}

void HaRemoteMqtt::setup()
{
  if (!pubsub_client.connected())
  {
    reconnect();
  }
  discover();
  currentTransformer.setup();
}

void HaRemoteMqtt::loop()
{
  if (!pubsub_client.connected())
  {
    reconnect();
  }

  currentTransformer.loop();
  update_switch_state();
  pubsub_client.loop();
}

void HaRemoteMqtt::discover()
{
  PRINTLN("Discovery");

  for (size_t i = 0; i < RC_FUNCTIONS_COUNT; i++)
  {
    String function_name(function_names[i]);
    String configTopic = buildConfigTopic(function_name);
    PRINTLN("\ttopic: \t\t", configTopic);

    String configPayload = build_config_payload(function_name);
    PRINTLN("\tpayload: \t", configPayload);

    pubsub_client.publish(configTopic.c_str(), (char *)configPayload.c_str());
  }
  delay(1000);
}

void HaRemoteMqtt::reconnect()
{
  while (!pubsub_client.connected())
  {
    PRINT("Attempting MQTT connection...");
    if (pubsub_client.connect(MQTT_DEVICE_ID, MQTT_USERNAME, MQTT_PASSWORD))
    {
      PRINTLN("connected");
      subscribe_trigger_topics();
    }
    else
    {
      PRINTLN("failed, rc=", pubsub_client.state(), " try again in 5 seconds");
      delay(5000);
    }
  }
}

void HaRemoteMqtt::subscribe_trigger_topics()
{
  PRINTLN("Subscribe to:");

  String topicName;
  for (size_t i = 0; i < RC_FUNCTIONS_COUNT; i++)
  {
    topicName = String(MQTT_TOPIC_BASE) + String("/") + String(function_names[i]) + String("/set");
    pubsub_client.subscribe(topicName.c_str());
    PRINTLN("\t", topicName);
  }
}

String HaRemoteMqtt::build_config_payload(String control_type)
{
  StaticJsonDocument<200> doc;

  doc["~"] = String(MQTT_TOPIC_BASE) + String("/") + control_type;
  doc["unique_id"] = String(ID_BASE) + control_type;
  doc["name"] = String(NAME_BASE) + control_type;
  doc["cmd_t"] = String("~/set");
  doc["stat_t"] = String("~/state");

  String json;
  serializeJson(doc, json);
  return json;
}

String HaRemoteMqtt::buildConfigTopic(String control_type)
{
  String topic = String(MQTT_TOPIC_BASE) + String("/") + control_type + String("/config");
  return topic;
}

String HaRemoteMqtt::build_state_topic(String control_type)
{
  String topic = String(MQTT_TOPIC_BASE) + String("/") + control_type + String("/state");
  return topic;
}

void HaRemoteMqtt::update_switch_state()
{
  switch_state ctCurrent = currentTransformer.get_state();

  if (ctCurrent == statelastReported) //&& millis() - msLastReported < UPDATE_INTERVAL)
  {
    return;
  }

  for (int i = 0; i < RC_FUNCTIONS_COUNT; i++)
  {
    String rc_function = String(function_names[i]);
    String payload = ctCurrent == i ? String("ON") : String("OFF");
    String topic = build_state_topic(rc_function);
    publish(topic.c_str(), payload.c_str());
  }
  statelastReported = ctCurrent;
  msLastReported = millis();
}

void HaRemoteMqtt::publish(const char *topic, const char *payload)
{
  if (!pubsub_client.connected())
  {
    reconnect();
  }

  PRINTLN("Publish to topic: ", topic);
  PRINTLN("Payload: ", payload);
  bool pub_res = pubsub_client.publish(topic, payload);
  PRINTLN("Publish result", pub_res);
  delay(200);
}
