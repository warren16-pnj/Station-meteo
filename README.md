# 🌡️ Station Météo IoT Sécurisée (ESP32 + MQTT SSL + Node-RED)

Ce projet implémente une station de surveillance de température connectée, robuste et sécurisée. L'objectif est de récupérer des données capteurs, de les transmettre de manière chiffrée (TLS/SSL) à un serveur central, et de les visualiser en temps réel.

## 📋 Architecture du Projet



Le système repose sur une architecture MQTT distribuée :
1.  **Capteur (Edge) :** ESP32 + LM35 (Lecture analogique).
2.  **Transport :** Wi-Fi + MQTT via **TLS/SSL (Port 8883)**.
3.  **Broker (Serveur) :** Mosquitto tournant sur un Raspberry Pi 4.
4.  **Visualisation :** Node-RED (Dashboard).

## 🛠️ Matériel Utilisé

* **Microcontrôleur :** ESP32 DevKit V1.
* **Capteur :** LM35 (Capteur de température ).
* **Serveur :** Raspberry Pi 4 (OS : Raspberry Pi OS).
* **Réseau :** Wi-Fi Local (LAN).

## 🔐 Sécurité & Fiabilité (Points Clés)

Ce projet met en œuvre des protocoles de sécurité avancés souvent absents des projets IoT basiques :

* **Chiffrement SSL/TLS :** Toutes les communications entre l'ESP32 et le Raspberry Pi sont chiffrées. Les données ne circulent jamais en clair.
* **Autorité de Certification (CA) Privée :** Création d'une infrastructure à clé publique (PKI) locale avec OpenSSL.
* **Authentification :** Connexion MQTT protégée par identifiant et mot de passe.
* **Persistance (Retain) :** Le broker garde en mémoire la dernière mesure. En cas de redémarrage de l'interface, la donnée s'affiche immédiatement.
* **Reprise Automatique (Recovery) :** L'ESP32 gère automatiquement les déconnexions Wi-Fi ou MQTT.

---

## 🚀 Installation et Configuration

### 1. Configuration du Serveur (Raspberry Pi)

Nous avons transformé le Raspberry Pi en Autorité de Certification (CA).

**Commandes OpenSSL utilisées :**
```bash
# 1. Création de l'Autorité (CA)
openssl req -new -x509 -days 3650 -extensions v3_ca -keyout ca.key -out ca.crt

# 2. Création de la clé serveur
openssl genrsa -out server.key 2048

# 3. Création de la demande de signature (CSR)
# IMPORTANT : Le Common Name (CN) DOIT être l'IP du Raspberry (ex: 192.168.1.125)
openssl req -new -out server.csr -key server.key

# 4. Signature du certificat par notre CA
openssl x509 -req -in server.csr -CA ca.crt -CAkey ca.key -CAcreateserial -out server.crt -days 3650
