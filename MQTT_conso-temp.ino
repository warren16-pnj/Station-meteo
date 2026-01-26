#include <WiFi.h>
#include <PubSubClient.h>

// --- CONFIGURATION RÉSEAU ---
const char* ssid = "LoraChoco";
const char* password = "MRB3HBM0R28";
const char* mqtt_server = "192.168.1.125"; 
const int mqtt_port = 1883;

// --- DÉFINITION DES PINS ---
const int ledPin = 13;    // LED intégrée selon ta demande
const int sensorPin = 33; // Pin du capteur LM35
#define VBATPIN 35        // Pin de surveillance de la batterie [cite: 104]

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;

void setup_wifi() {
  delay(10);
  Serial.print("Connexion au Wi-Fi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnecté !");
}

// Fonction de réception (Callback) pour la LED
void callback(char* topic, byte* payload, unsigned int length) {
  String message = "";
  for (int i = 0; i < length; i++) { message += (char)payload[i]; }

  if (String(topic) == "station/led") {
    if (message == "1") digitalWrite(ledPin, HIGH);
    else digitalWrite(ledPin, LOW);
  }
}

void reconnect() {
  while (!client.connected()) {
    if (client.connect("ESP32_Ronan", "warren2", "warren")) {
      client.subscribe("station/led");
      client.subscribe("station/seuil");
    } else {
      delay(5000);
    }
  }
}

void setup() {
  pinMode(ledPin, OUTPUT);
  Serial.begin(115200); // Initialisation console
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) { reconnect(); }
  client.loop();

  unsigned long now = millis();
  if (now - lastMsg > 20000) {
    lastMsg = now;

    // 1. LECTURE TEMPÉRATURE (LM35)
    int val = analogRead(sensorPin);
    float temp_millivolts = (val / 4095.0) * 3300.0;
    float temperature = temp_millivolts / 10.0;

    // 2. LECTURE BATTERIE (Selon PDF)
    // Lecture de la tension en millivolts sur le pin 35 [cite: 101, 109]
    float measuredvbat = analogReadMilliVolts(VBATPIN); 
    // On multiplie par 2 car la tension est divisée par 2 sur la carte 
    measuredvbat *= 2; 
    // Conversion en Volts pour la lisibilité [cite: 110]
    float vbat_volts = measuredvbat / 1000.0; 

    // 3. ENVOI DU MESSAGE COMBINÉ
    // Format : "température,tension_batterie"
    String payload = String(temperature) + "," + String(vbat_volts);
    client.publish("test_topic", payload.c_str());

    // Affichage console pour vérification
    Serial.print("Temp: "); Serial.print(temperature);
    Serial.print(" C | VBat: "); Serial.print(vbat_volts); Serial.println(" V");
  }
}