## Dépannage et Solutions (Troubleshooting)

Si le système ne fonctionne pas comme prévu, voici une liste de contrôle pour diagnostiquer et résoudre les problèmes les plus fréquents.

### A. Problèmes de Connexion (Réseau)

**Symptôme :** L'ESP32 affiche "Tentative de connexion MQTT..." indéfiniment ou Node-RED reste sur "Connecting".

* **Vérifier l'Adresse IP du Raspberry Pi :**
    * L'adresse IP du Pi peut changer s'il n'est pas en IP fixe.
    * **Action :** Vérifiez que cette IP correspond exactement à la variable `mqtt_server` dans le code Arduino (`192.168.1.XXX`) et dans la configuration du nœud MQTT de Node-RED.
* **Vérifier le Réseau WiFi :**
    * L'ESP32 et le Raspberry Pi doivent être sur le même réseau (même Box ou même Routeur).
    * *Attention :* L'ESP32 ne supporte que le WiFi **2.4 GHz** (pas le 5 GHz).
* **Vérifier le Pare-feu (Firewall) :**
    * Le port 1883 doit être ouvert sur le Raspberry Pi.
    * **Commande :** `sudo ufw allow 1883`

### B. Problèmes MQTT (Broker & Topics)

**Symptôme :** L'ESP32 est "Connecté", mais aucune donnée ne bouge sur le Dashboard Node-RED.

* **Erreur de Topic :**
    * Le topic est **sensible à la casse** (majuscules/minuscules).
    * *Exemple :* Si l'ESP32 publie sur `Station/Temp` et que Node-RED écoute `station/temp`, cela ne marchera pas.
    * **Action :** Copiez-collez strictement le même nom de topic des deux côtés.
* **Erreur d'Identifiants (Connection Refused) :**
    * Si vous avez activé un mot de passe dans Mosquitto.
    * **Action :** Vérifiez que le `mqtt_user` et `mqtt_pass` dans le code Arduino correspondent à ceux créés sur le Pi.
    * **Action :** Vérifiez l'onglet "Security" dans le nœud MQTT de Node-RED.
* **Mosquitto est-il lancé ?**
    * **Commande :** `sudo systemctl status mosquitto`
    * Il doit indiquer `Active: active (running)`. Sinon, faites `sudo systemctl start mosquitto`.

### C. Problèmes d'Affichage (Node-RED)

**Symptôme :** Les jauges restent à 0 ou Node-RED affiche des erreurs dans le panneau de debug.

* **Erreur "Split is not a function" :**
    * Node-RED reçoit parfois les données sous forme brute (Buffer) au lieu de Texte.
    * **Solution :** Dans le nœud *Function*, ajoutez `.toString()` :
        ```javascript
        var values = msg.payload.toString().split(',');
        ```
* **Valeurs incohérentes (ex: Température à 500°C) :**
    * Vérifiez le câblage du capteur sur la bonne broche (GPIO 32 vs 35).
    * Vérifiez la formule de conversion dans le code Arduino (Attention aux parenthèses et aux décimales).

### D. Outils de Diagnostic Rapide

Pour savoir d'où vient le problème, isolez chaque partie :

1.  **Vérifier la sortie de l'ESP32 :**
    * Ouvrez le **Moniteur Série** dans l'IDE Arduino (Baudrate **115200**).
    * Voyez-vous "WiFi Connecté" et "Envoi température" ?
2.  **Écouter le Broker (L'arbitre) :**
    * Sur le Raspberry Pi, lancez : `mosquitto_sub -h localhost -t "#" -v`
    * (Le `#` signifie "écouter TOUS les topics").
    * Si vous voyez les messages arriver ici, le problème est dans Node-RED. Si rien n'arrive, le problème est côté ESP32 ou Réseau.

### E. Téléversement du programme Arduino

Il se peut qu'en voulant télécharger le programme Arduino sur l'ESP32, une erreur type "uploading error: exit status 2"
Il faut alors aller dans le menu "Outils" sur Arduino et changer l"Upload Speed" pour mettre "115200".
    