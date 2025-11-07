#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include "DHT.h"
#include <ArduinoJson.h>
#include "credentials.h"

#define DHTPIN 14
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

const int mqtt_port = 8883;
const char* mqtt_topic = "esp32/env_data";

WiFiClientSecure net;
PubSubClient client(net);

void connectWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("Connecting WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
}

void connectMQTT() {
  client.setServer(mqtt_server, mqtt_port);
  net.setInsecure(); // HiveMQ Cloud already uses valid CA — skip local CA

  Serial.print("Connecting to HiveMQ...");
  while (!client.connected()) {
    if (client.connect("esp32_dht11_client", MQTT_USER, MQTT_PASS)); {
      Serial.println("connected");
    } else {
      Serial.print(".");
      delay(1000);
    }
  }
}

void setup() {
  Serial.begin(9600);
  dht.begin();
  connectWiFi();
  connectMQTT();
}

void loop() {
  if (!client.connected()) connectMQTT();
  client.loop();

  float t = dht.readTemperature();
  float h = dht.readHumidity();

  if (!isnan(t) && !isnan(h)) {
    StaticJsonDocument<128> doc;
    doc["temperature"] = t;
    doc["humidity"] = h;

    char buf[128];
    size_t n = serializeJson(doc, buf);
    client.publish(mqtt_topic, buf, n);
    Serial.printf("Published: %s\n", buf);
  }
  delay(5000);
}
