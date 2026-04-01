# AquaServeur - base complète V1

Cette base reprend l'architecture du cahier des charges V3 et fournit un serveur C++ modulaire pour ESP32-S3.

## Ce que le code fait déjà

- charge un fichier `config.json` depuis `LittleFS`
- valide la structure logique de la configuration
- construit le runtime à partir du JSON
- initialise capteurs et sorties
- lit les capteurs sur une boucle principale
- enregistre des tests manuels
- applique les règles d'analyse
- associe les conseils préventifs / correctifs
- applique des routines horaires
- publie un état global JSON vers l'IHM
- accepte le principe d'un remplacement complet de configuration envoyé par l'IHM

## Ce qu'il reste à brancher pour passer en vrai matériel

- drivers réels des capteurs dans `SensorManager::readMockSensor`
- vraie couche de communication IHM dans `CommManager`
- stockage atomique du nouveau JSON dans LittleFS avant activation
- synchronisation horaire NTP pour des routines fiables
- éventuelle persistance d'historique / logs si tu veux aller plus loin

## Bibliothèques prévues

- framework Arduino pour ESP32
- `ArduinoJson`
- `LittleFS`

## Arborescence

Voir directement les dossiers du projet. Chaque module correspond à une responsabilité unique.

## Point d'attention

Le code est volontairement générique. Il ne connaît pas en dur le nom des capteurs, des sorties ou des analyses métiers. Toute la logique passe par les rôles et identifiants définis dans le JSON.
