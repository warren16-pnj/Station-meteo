# Station Météo IoT Sécurisée <br> (LM35 + ESP32 + Raspberry Pi)

L'objectif de se projet  est de mettre en place une station de surveillance de température connectée, robuste et sécurisée. L'objectif est de récupérer les données d'un capteurs de température, de les transmettre de manière chiffrée (TLS/SSL) à un serveur central, de les visualiser en temps réel et également de les stocker dans une base de données. <br>

L'objectif premier est d'acheminer les mesures de températures jusqu'à la carte Raspberry Pi. Pour ce faire, la température est lu avec le LM35 sur l'ESP32. 
L’utilisation de la carte ESP32 en amont du Raspberry Pi s'explique par la capacité de l'ESP32 à interagir avec des capteurs analogiques, contrairement à le Raspberry Pi. L'ESP32 permet également de convertir un signal analogique en un signal numérique pour transmettre l'information à le Raspberry Pi. 
Pour finir, l'ESP32 offre une consommation énergétique très faible grâce à ses modes de veille, ce qui le rend adapté aux mesures périodiques. 

## Architecture du projet

```mermaid
graph LR
    %% Définition des nœuds 
    LM35[Capteur LM35]
    ESP32[Microcontrôleur ESP32]
    WIFI((Routeur Wi-Fi))
    RPI[Raspberry Pi 4<br/>Mosquitto + Node-RED]
    DASH[Interface Dashboard]
    SQL[Base de données SQLite]

    %% Définition des liens 
    LM35 -- Signal Analogique<br/>(mV) --> ESP32
    ESP32 -- MQTT Sécurisé (SSL)<br/>Port 8883 --> WIFI
    WIFI -- Réseau Local --> RPI
    RPI --> DASH
    RPI --> SQL

    %% Styles 
    style LM35 fill:#f9f,stroke:#333,stroke-width:2px
    style ESP32 fill:#bbf,stroke:#333,stroke-width:2px
    style RPI fill:#bfb,stroke:#333,stroke-width:2px
```
Le système repose sur une architecture MQTT distribuée :
1.  **Capteur :** capteur LM35 + microcontrôleur ESP32 (Lecture analogique).
2.  **Transport :** MQTT via TLS/SSL (Port 8883) + Wi-Fi.
3.  **Broker :** Mosquitto tournant sur la Raspberry Pi 4.
4.  **Visualisation :** Node-RED (Dashboard).
5.  **Stockage :** base de données SQLite 

## Matériel utilisé

* **Capteur :** LM35 (Capteur de température).
* **Microcontrôleur :** ESP32.
* **Serveur :** Raspberry Pi 4 (OS : Raspberry Pi OS).
* **Réseau :** Wi-Fi Local (LAN).

## Sécurité & fiabilité 

Pour garantir la sécurité des informations qui transitent au cours de ce projet, différents protocoles de sécurité des informations sont mis en œuvres :

* **Chiffrement SSL/TLS :** Toutes les communications entre l'ESP32 et le Raspberry Pi sont chiffrées. Les données ne circulent jamais en clair.
* **Autorité de Certification (CA) Privée :** Création d'une infrastructure à clé publique (PKI) locale avec OpenSSL.
* **Authentification :** Connexion MQTT protégée par un identifiant et un mot de passe.
* **Persistance (Retain) :** Le broker garde en mémoire la dernière mesure. En cas de redémarrage de l'interface, la donnée s'affiche immédiatement.
* **Reprise Automatique (Recovery) :** L'ESP32 gère automatiquement les déconnexions Wi-Fi ou MQTT.

---
