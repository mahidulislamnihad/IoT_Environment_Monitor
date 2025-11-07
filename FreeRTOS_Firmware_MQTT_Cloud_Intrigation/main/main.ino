#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include "DHT.h"
#include <ArduinoJson.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "credentials.h"

// ---------- Hardware ----------
#define DHTPIN   14
#define DHTTYPE  DHT11
#define LED_PIN   2     // On-board LED for publish indication
DHT dht(DHTPIN, DHTTYPE);


const int   mqtt_port   = 8883;
const char* mqtt_topic  = "esp32/env_data";

WiFiClientSecure net;
PubSubClient client(net);

// ---------- Global Sensor Data ----------
float temperature = 0.0f;
float humidity    = 0.0f;
unsigned long lastReconnectAttempt = 0;

// ---------- Connection Utilities ----------
void connectWiFi() {
  Serial.printf("[WiFi] Connecting to %s", ssid);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.printf("\n[WiFi] Connected! IP: %s\n", WiFi.localIP().toString().c_str());
}

bool connectMQTT() {
  client.setServer(mqtt_server, mqtt_port);
  net.setInsecure();              // HiveMQ Cloud uses trusted CA
  if (client.connected()) return true;

  Serial.print("[MQTT] Connecting...");
  if (client.connect("esp32_dht11_client", MQTT_USER, MQTT_PASS)); {
    Serial.println("connected!");
    return true;
  }
  Serial.printf("failed, state %d\n", client.state());
  return false;
}

// ---------- FreeRTOS Tasks ----------
void TaskReadSensor(void *pvParameters) {
  for (;;) {
    float t = dht.readTemperature();
    float h = dht.readHumidity();

    if (isnan(t) || isnan(h)) {
      Serial.println("[Sensor] Reading failed!");
    } else {
      temperature = t;
      humidity    = h;
      Serial.printf("[ReadSensor][Core %d] T: %.2f °C | H: %.2f %%\n",
                    xPortGetCoreID(), temperature, humidity);
    }

    vTaskDelay(pdMS_TO_TICKS(3000));
  }
}

void TaskPublishMQTT(void *pvParameters) {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  for (;;) {
    if (!client.connected()) {
      unsigned long now = millis();
      if (now - lastReconnectAttempt > 10000) {
        if (connectMQTT()) lastReconnectAttempt = now;
      }
    }

    client.loop();

    StaticJsonDocument<160> doc;
    doc["temperature"] = temperature;
    doc["humidity"]    = humidity;
    doc["timestamp"]   = millis();

    char buf[160];
    size_t n = serializeJson(doc, buf);

    if (client.publish(mqtt_topic, buf, n)) {
      Serial.printf("[PublishMQTT][Core %d] %s\n", xPortGetCoreID(), buf);
      digitalWrite(LED_PIN, HIGH);
      vTaskDelay(pdMS_TO_TICKS(200));
      digitalWrite(LED_PIN, LOW);
    } else {
      Serial.println("[PublishMQTT] Publish failed!");
    }

    vTaskDelay(pdMS_TO_TICKS(5000));
  }
}

// ---------- Setup / Loop ----------
void setup() {
  Serial.begin(9600);
  dht.begin();
  connectWiFi();
  connectMQTT();

  xTaskCreatePinnedToCore(TaskReadSensor, "ReadSensor",
                          4096, NULL, 2, NULL, 0);
  xTaskCreatePinnedToCore(TaskPublishMQTT, "PublishMQTT",
                          4096, NULL, 1, NULL, 1);
}

void loop() {
  // Main loop left empty; FreeRTOS tasks handle everything.
}
