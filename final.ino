#include <WiFi.h>
#include <PubSubClient.h>

// --- CONFIGURATION ---
const char* ssid = "ton_wifi";
const char* password = "ton_mdp_wifi";
const char* mqtt_server = "192.168.1.XXX"; 
const int mqtt_port = 1883;

// --- VARIABLES GLOBALES ---
float seuilTemperature = 0.0; // Valeur par défaut
const int sensorPin = 33;      // Pin du capteur LM35
#define VBATPIN 35             // Pin batterie (PDF) [cite: 1, 79]
#define RGB_BRIGHTNESS 64 

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;

void setup_wifi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) { delay(500); }
  rgbLedWrite(RGB_BUILTIN, 0, 0, RGB_BRIGHTNESS); // Bleu = Connecté
}

// --- RÉCEPTION DES MESSAGES ---
void callback(char* topic, byte* payload, unsigned int length) {
  String message = "";
  for (int i = 0; i < length; i++) { message += (char)payload[i]; }

  // Cas 1 : Réception du nouveau seuil
  if (String(topic) == "station/seuil") {
    seuilTemperature = message.toFloat();
    Serial.print("Nouveau seuil reçu : ");
    Serial.println(seuilTemperature);
    
    // Petit flash blanc pour confirmer la réception
    rgbLedWrite(RGB_BUILTIN, 255, 255, 255);
    delay(100);
    rgbLedWrite(RGB_BUILTIN, 0, 0, 0);
  }
}

void reconnect() {
  while (!client.connected()) {
    if (client.connect("ESP32_Ronan", "warren2", "warren")) {
      // S'abonner aux ordres de Node-RED
      client.subscribe("station/led");
      client.subscribe("station/seuil"); 
    } else {
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) { reconnect(); }
  client.loop();

  unsigned long now = millis();
  if (now - lastMsg > 1000) {
    lastMsg = now;

    // 1. Lectures (LM35 et Batterie selon PDF) [cite: 1, 109]
    float temp = ((analogRead(sensorPin) / 4095.0) * 3300.0) / 4.7;
    float vbat = (analogReadMilliVolts(VBATPIN) * 2.0) / 1000.0;

    // 2. Alerte locale avec la LED RGB
    if (temp > seuilTemperature) {
      rgbLedWrite(RGB_BUILTIN, RGB_BRIGHTNESS, 0, 0); // ROUGE si dépassement
    } else {
      rgbLedWrite(RGB_BUILTIN, 0, RGB_BRIGHTNESS, 0); // VERT si OK
    }

    // 3. Envoi des données vers Node-RED
    String payload = String(temp) + "," + String(vbat);
    client.publish("ton_topic", payload.c_str());
  }
}