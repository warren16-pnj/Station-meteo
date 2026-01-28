##  Détail des Blocs Node-RED

Ce projet Node-RED est composé de plusieurs nœuds interconnectés. Voici l'explication technique et fonctionnelle de chaque bloc, classée par fonction.

### 1.  Zone de Réception et Affichage (Entrées)

Cette partie gère l'arrivée des données brutes venant de l'ESP32 et leur visualisation.

####  Nœud `mqtt in` (Nom : "Réception ESP32")
* **Type :** Entrée MQTT.
* **Fonction :** Il s'abonne au topic `test_topic` sur le Broker local (Mosquitto). Il agit comme une "oreille" qui écoute en permanence les messages envoyés par l'ESP32.
* **Données reçues :** Une chaîne de caractères contenant la température (ex: `"24.5"` ou `"24.5,4.1"`).

####  Nœud `ui_gauge` (Nom : "Jauge Temps Réel")
* **Type :** Dashboard Gauge.
* **Fonction :** Affiche la dernière valeur reçue sous forme de cadran (compteur de vitesse).
* **Configuration :** Plage de 0 à 35°C. Les couleurs (Vert/Jaune/Rouge) changent automatiquement selon la température.

####  Nœud `ui_chart` (Nom : "Graphique Historique")
* **Type :** Dashboard Chart.
* **Fonction :** Trace une courbe évolutive de la température en fonction du temps.
* **Utilité :** Permet de visualiser les tendances (chauffage ou refroidissement) sur les dernières minutes/heures.

---

### 2.  Zone de Logique et Alertes (Traitement)

C'est le "cerveau" du flux. Il analyse les données pour décider si une alerte doit être envoyée.

####  Nœud `function` (Nom : "Comparaison & Alerte")
* **Type :** Script JavaScript.
* **Fonction :** C'est ici que l'intelligence réside. Ce bloc exécute trois actions :
    1.  **Nettoyage :** Il convertit le message reçu en texte et le découpe (si format "temp,batterie").
    2.  **Comparaison :** Il récupère la valeur du seuil (réglée par le curseur) depuis la mémoire de Node-RED (`flow.get`) et la compare à la température actuelle.
    3.  **Formatage :** Si `Température > Seuil`, il crée un message JSON formaté spécifiquement pour l'API de Discord.

####  Nœud `http request` (Nom : "Envoi Discord")
* **Type :** Sortie HTTP (POST).
* **Fonction :** Il envoie le message JSON généré par le bloc précédent vers le serveur Discord via un **Webhook**.
* **Rôle :** Agit comme le facteur qui livre l'alerte sur votre téléphone/PC.

---

### 3.  Zone de Contrôle (Commandes)

Cette partie permet à l'utilisateur d'agir sur le système depuis l'interface graphique.

####  Nœud `ui_slider` (Nom : "Curseur Seuil")
* **Type :** Dashboard Slider.
* **Fonction :** Un curseur glissière sur l'interface web qui permet de choisir une température critique (entre 0°C et 30°C).
* **Comportement :** Dès qu'on le bouge, il envoie la nouvelle valeur à deux endroits : vers la mémoire interne et vers l'ESP32.

####  Nœud `function` (Nom : "Stockage Seuil")
* **Type :** Script JavaScript.
* **Fonction :** Sauvegarde la valeur choisie sur le curseur dans une variable globale (`flow.set('seuil_critique', ...)`).
* **Pourquoi ?** Pour que le bloc d'alerte (voir plus haut) puisse connaître le seuil actuel même si l'utilisateur ne touche pas au curseur à ce moment-là.

####  Nœud `mqtt out` (Topic : "station/seuil")
* **Type :** Sortie MQTT.
* **Fonction :** Publie la valeur du curseur sur le réseau.
* **Destinataire :** L'ESP32 est abonné à ce topic. Quand il reçoit ce message, il met à jour sa propre variable seuil pour allumer sa LED en rouge localement.

---

### 4.  Zone de Débogage

Ces blocs ne sont pas visibles par l'utilisateur final mais servent au développeur.

####  Nœud `debug` (Nom : "Vérif Debug")
* **Type :** Sortie Console.
* **Fonction :** Affiche les messages bruts dans la barre latérale droite de l'éditeur Node-RED.
* **Utilité :** Permet de vérifier si l'ESP32 envoie bien des données ou si le format est correct sans avoir à regarder le Dashboard.
