/*
 * ENVOI TEMPÉRATURE SIMPLE VIA WI-FI
 * Envoie uniquement la valeur du LM35 sur le topic 'test_topic'
 */

#include <WiFi.h>
#include <PubSubClient.h>

// --- CONFIGURATION RÉSEAU ---
const char* ssid = "LoraChoco"; 
const char* password = "MRB3HBM0R28"; 
const char* mqtt_server = "192.168.1.125"; 

// --- BROCHE CAPTEUR ---
#define LM35PIN 32 

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);
  
  // Configuration Wi-Fi [cite: 13, 14]
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connecté");

  // Configuration MQTT [cite: 28]
  client.setServer(mqtt_server, 1883);
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Tentative connexion MQTT...");
    // Utilisation de vos identifiants [cite: 144]
    if (client.connect("ESP32_Temp_Station", "warren2", "warren")) {
      Serial.println("Connecté !");
    } else {
      delay(5000);
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Lecture du LM35 (10mV / °C) [cite: 104, 184]
  uint32_t milliVolts = analogReadMilliVolts(LM35PIN);
  float temperature = milliVolts / 10.0;

  // Conversion de la valeur en chaîne de caractères
  String tempStr = String(temperature);
  
  // Envoi sur le topic spécifique
  Serial.print("Température envoyée : ");
  Serial.println(tempStr);
  client.publish("test_topic", tempStr.c_str());

  delay(60000); // Envoi toutes les secondes
}