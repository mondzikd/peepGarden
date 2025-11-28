#include "secrets.h"

#include <MQTTClient.h>
#include <ArduinoJson.h>
#include <WiFi.h>

#define MQTT_PUBLISH_TOPIC   "peep/esp32/measurements"
#define MQTT_SUBSCRIBE_TOPIC "peep/esp32/commands"

WiFiClient wifiClient;
MQTTClient mqttClient = MQTTClient(256);

// ADC1 Pins only. ADC2 doesn't work with WiFi :/
int pins[] = {36,39,34,35,32,33};
String ids[] = {"Iga Świątek","Andrzej Gołota","Adam Małysz","Mariusz Pudzianowski","Robert Kubica","Jan III Sobieski"};

// Reference values
const int referenceAir = 2520;
const int referenceWater = 880;
const int referenceMargin = 100;

// Reference values on 27: 2540 and 980
// saw 820 once :O

long lastMsg = 0;
const int baseIntervals = 5 * 60 * 1000;
const int wateringIntervals = 10 * 1000;
const int wateringMessages = 12;
int wateringMessagesLeft = 0;
int calculateInterval() {
  return (wateringMessagesLeft > 0) ? wateringIntervals : baseIntervals;
}
void startWatering() {
  Serial.println("Watering started!");
  wateringMessagesLeft = wateringMessages;
}
void wateringFlush() {
  if (wateringMessagesLeft > 0) {
    wateringMessagesLeft -= 1;
  } else {
    Serial.println("Watering finished :)");
  }
}

void messageHandler(String &topic, String &payload) {
  Serial.println("incoming: " + topic + " - " + payload);

  if (payload == "measure") {
    startWatering();
  }
}

void connectMQTT() {
  Serial.println();
  Serial.println("******************************************************");
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED){
    delay(1000);
    Serial.println(WiFi.status());
  }

  Serial.println();
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  mqttClient.begin(MQTT_ENDPOINT, 1883, wifiClient);

  mqttClient.onMessage(messageHandler);

  Serial.print("Connecting to MQTT");
  
  mqttClient.setKeepAlive(baseIntervals / 1000 + 1);

  while (!mqttClient.connect(THINGNAME)) {
    Serial.print(".");
    delay(100);
  }
  Serial.println();

  if(!mqttClient.connected()){
    Serial.println("MQTT Timeout!");
    return;
  }

  mqttClient.subscribe(MQTT_SUBSCRIBE_TOPIC);

  Serial.println("MQTT Connected!");
}

int calculatePercent(int value) {
  int percent = map(value, referenceAir, referenceWater, 0, 100);
  return constrain(percent, 0, 100);
}

int readSensorData(int pin) {
  return analogRead(pin);
}

bool isPinConnected(int moisture) {
  return moisture > referenceWater - 100 && moisture < referenceAir + 100;
}

void createMeasurementsMessage(char outputJsonBuffer[512]) {
  JsonDocument doc;
  JsonArray data = doc["data"].to<JsonArray>();
  
  int length = sizeof(pins) / sizeof(pins[0]);
  for (int i = 0; i < length; i++) {
    int moisture = readSensorData(pins[i]);

    if (isPinConnected(moisture)) {
      int moisturePercent = calculatePercent(moisture);
      JsonDocument sensorData;
      sensorData["sensor_id"] = ids[i];
      sensorData["moisture"] = moisturePercent;
      data.add(sensorData);
    }
  }
  
  serializeJson(doc, outputJsonBuffer, 512);
}

void publishMessage(String mqtt_topic, char jsonBuffer[512]) { 
  mqttClient.publish(mqtt_topic, jsonBuffer);
}

void setup() {
  Serial.begin(115200);
  delay(2000);
  connectMQTT();
}

void loop() {
  if(!mqttClient.connected()){
    Serial.println("MQTT disconnected. Connecting again...");
    connectMQTT();
  }
  mqttClient.loop();
  
  long now = millis();
  if (now - lastMsg > calculateInterval()) {
    lastMsg = now;
    char jsonBuffer[512];
    createMeasurementsMessage(jsonBuffer);
    
    Serial.println(jsonBuffer);
    publishMessage(MQTT_PUBLISH_TOPIC, jsonBuffer);

    wateringFlush();
  }
}
