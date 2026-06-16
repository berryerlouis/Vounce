# Vounce Contrôleur MIDI - Guide d'utilisation

Ce guide est destiné aux musiciens et makers qui souhaitent utiliser Vounce comme contrôleur MIDI.

## Ce que fait ce contrôleur

Vounce convertit les commandes physiques en messages MIDI Control Change (CC) :
- Encodeur rotatif
- Bouton poussoir / commutateur
- Capteurs optionnels (potentiomètre, ultrasonique)

Vous pouvez modifier le comportement des capteurs directement sur l'appareil via le menu intégré, sans reprogrammer le firmware.

## Avant de commencer

Vous avez besoin de :
- Votre carte programmée avec le firmware Vounce
- Un câble USB branché à votre ordinateur
- Un DAW ou un moniteur MIDI

Notes de connexion :
- Sur une build Pro Micro USB, Vounce peut apparaître comme un périphérique USB MIDI natif.
- Sur une build sans USB MIDI, il faut une passerelle série-vers-MIDI côté hôte.

## Signification des LEDs

Les 2 LEDs indiquent le mode de fonctionnement :

- LED Menu (`PIN_LED_MENU`)
  - Éteinte : menu fermé
  - Allumée : menu ouvert
  - Clignotante : paramètre en édition

- LED Inactif (`PIN_LED_IDLE`)
  - Allumée : menu fermé (mode de lecture normal)
  - Éteinte : menu ouvert

Référence implémentation : [src/led/LedIndicator.cpp](src/led/LedIndicator.cpp)

## Fonctionnement de base

### Mode de lecture normal

Quand le menu est fermé :
- Les changements des capteurs sont envoyés en MIDI CC
- LED Inactif est ALLUMÉE

Flux d'exécution principal : [vounce.ino](vounce.ino)

### Ouvrir et utiliser le menu

Contrôles :
- Appui long : ouvrir le menu, revenir en arrière, ou fermer le menu
- Appui court : confirmer / entrer / quitter le mode édition
- Tourner l'encodeur : naviguer dans les éléments ou modifier la valeur

États du menu :
- SelectSensor : choisir quel capteur éditer
- SelectParam : choisir un paramètre
- EditParam : modifier la valeur du paramètre

Transitions détaillées : [MENU_FLOW.md](MENU_FLOW.md)

## Important : Mode MIDI vs Mode de débogage

Vounce `MidiOut` supporte deux styles de sortie :

- `logEnabled = true` (par défaut)
  - Affiche des lignes lisibles comme `Channel: 10, Control: 1, Value: 64`
  - Idéal pour tester et déboguer
  - Le MIDI est quand même transmis en parallèle
  - L'état du menu peut aussi être observé en texte lisible via le logger

- `logEnabled = false`
  - Masque la sortie texte de débogage
  - Continue d'envoyer le MIDI normalement
  - L'état du menu est envoyé en tant que CC MIDI 120 sur le canal 11 (canal du bouton) pour le débogage à distance

Référence : [src/midi/MidiOut.h](src/midi/MidiOut.h), [src/midi/MidiOut.cpp](src/midi/MidiOut.cpp)

Si vous voulez le comportement MIDI réel, instanciez `MidiOut` avec `false` dans [vounce.ino](vounce.ino).

Sur les cibles Pro Micro, l'USB MIDI est activé automatiquement quand le profil de carte Arduino expose une macro USB supportée. Sur les autres cibles, Vounce utilise des octets MIDI série.

### Sortie de l'état du menu via MIDI

Quand `logEnabled = false`, le système de menu envoie son état en tant que messages CC MIDI 120 sur le canal 11 (le canal du capteur bouton/menu). Cela permet aux appareils MIDI externes ou aux logiciels de suivre la navigation du menu même lorsque le journal série est désactivé.

**Codage de l'état du menu :**
- Menu fermé : valeur = 0
- Mode SelectSensor : valeur = (index du capteur sélectionné + 1) → plage 1–N (où N est le nombre de capteurs)
- Mode SelectParam : valeur = (index du paramètre sélectionné + 100) → plage 100+
- Mode EditParam : valeur = (valeur du paramètre contrainte) → plage 0–127

Cette fonctionnalité est utile pour :
- Surveiller l'état du menu depuis un DAW ou un moniteur MIDI
- Mapper la navigation du menu vers du matériel externe
- Déboguer le comportement du contrôleur dans les environnements de production

## Première configuration - Checklist

1. Programmez le firmware sur votre carte.
2. Branchez l'USB.
3. Vérifiez que la vitesse série est 115200.
4. Démarrez en mode débogage (`logEnabled = true`) pour confirmer l'activité.
5. Bougez les commandes et confirmez que la sortie apparaît.
6. Ouvrez le menu et vérifiez que les LEDs réagissent comme prévu.
7. Passez au mode MIDI brut (`logEnabled = false`) pour la production.
8. Mappez les CC entrants dans votre DAW.

## Flux de travail lors d'une performance

1. Allumez l'appareil.
2. Gardez le menu fermé pour le contrôle MIDI normal.
3. Si vous avez besoin d'ajuster la réponse, appui long pour ouvrir le menu.
4. Modifiez les paramètres des capteurs (canal/CC/plage/sensibilité).
5. Fermez le menu pour reprendre le flux MIDI.

## Comportement de l'encodeur et du bouton

- Appuyer sur le bouton de l'encodeur ne modifie pas la valeur MIDI de l'encodeur.
- Le mouvement de l'encodeur est ignoré tant que le bouton est maintenu.
- Le décodeur rotatif du menu reste synchronisé pendant l'appui, ce qui garde un mode édition précis après relâchement.
- L'entrée du bouton filtre les appuis très courts pour rejeter les faux basculements dus au bruit mécanique pendant la rotation.

## Référence des paramètres des capteurs

Chaque capteur peut être configuré via le menu avec différents paramètres. Cette section liste tous les paramètres éditables pour chaque type de capteur.

### Capteur Encodeur

Le contrôle encodeur rotatif.

| Paramètre | Min | Max | Pas | Description |
|-----------|-----|-----|-----|-------------|
| midi_channel | 0 | 15 | 1 | Canal MIDI (0-15 = canaux 1-16) |
| midi_control | 0 | 127 | 1 | Numéro CC MIDI en sortie |
| min | 0 | 127 | 1 | Valeur MIDI minimale (limite inférieure) |
| max | 0 | 127 | 1 | Valeur MIDI maximale (limite supérieure) |
| sensitivity | 1 | 16 | 1 | Variation par pas d'encodeur (1-16) |

**Conseils :**
- Augmentez la sensibilité pour un contrôle grossier (grands sauts par pas)
- Diminuez la sensibilité pour un contrôle fin (petits incréments)
- Définissez min/max pour délimiter la plage de valeurs envoyées

### Capteur Bouton à bascule

Le bouton poussoir ou commutateur momentané.

| Paramètre | Min | Max | Pas | Description |
|-----------|-----|-----|-----|-------------|
| midi_channel | 0 | 15 | 1 | Canal MIDI (0-15 = canaux 1-16) |
| midi_control | 0 | 127 | 1 | Numéro CC MIDI en sortie |
| value_off | 0 | 127 | 1 | Valeur MIDI quand le bouton est relâché |
| value_on | 0 | 127 | 1 | Valeur MIDI quand le bouton est appuyé |
| debounce_ms | 5 | 500 | 5 | Délai de filtrage des rebonds (millisecondes) |

**Conseils :**
- Réglez value_off à 0 et value_on à 127 pour un comportement binaire on/off
- Utilisez d'autres paires (ex: 32/96) pour des points de basculement différents
- Augmentez debounce_ms si le bouton provoque plusieurs déclenchements rapides

### Capteur Potentiomètre (optionnel)

Curseur ou bouton analogique sur pin A0.

| Paramètre | Min | Max | Pas | Description |
|-----------|-----|-----|-----|-------------|
| midi_channel | 0 | 15 | 1 | Canal MIDI (0-15 = canaux 1-16) |
| midi_control | 0 | 127 | 1 | Numéro CC MIDI en sortie |
| analog_min | 0 | 1023 | 1 | Valeur ADC brute au minimum (0-1023) |
| analog_max | 0 | 1023 | 1 | Valeur ADC brute au maximum (0-1023) |
| midi_min | 0 | 127 | 1 | Sortie MIDI quand l'analogique est au minimum |
| midi_max | 0 | 127 | 1 | Sortie MIDI quand l'analogique est au maximum |
| threshold | 1 | 16 | 1 | Changement minimum pour envoyer une nouvelle valeur MIDI |

**Conseils :**
- Calibrez analog_min/max en déplaçant le potentiomètre aux extrêmes et notez les valeurs brutes
- Utilisez threshold pour réduire le trafic MIDI (plus élevé = mises à jour moins fréquentes)
- Inversez midi_min et midi_max pour inverser la réponse du potentiomètre

### Capteur Ultrasonique (optionnel)

Capteur de distance HC-SR04 (pins 6=trigger, 5=echo).

| Paramètre | Min | Max | Pas | Description |
|-----------|-----|-----|-----|-------------|
| midi_channel | 0 | 15 | 1 | Canal MIDI (0-15 = canaux 1-16) |
| midi_control | 0 | 127 | 1 | Numéro CC MIDI en sortie |
| dist_min_cm | 1 | 400 | 1 | Distance la plus proche à mapper (centimètres) |
| dist_max_cm | 1 | 400 | 1 | Distance la plus éloignée à mapper (centimètres) |
| midi_min | 0 | 127 | 1 | Sortie MIDI quand l'objet est à dist_min |
| midi_max | 0 | 127 | 1 | Sortie MIDI quand l'objet est à dist_max |
| sample_ms | 10 | 500 | 5 | Intervalle entre les mesures (millisecondes) |
| threshold | 1 | 16 | 1 | Changement minimum pour envoyer une nouvelle valeur MIDI |

**Conseils :**
- La portée maximale typique est 200-300 cm (limite HC-SR04 ~4m)
- Des intervalles d'échantillonnage plus courts = plus réactif mais CPU élevé
- Utilisez threshold pour ignorer les petites fluctuations dues aux réflexions
- Inversez midi_min/max pour inverser le comportement (loin = élevé, proche = faible)

## Dépannage

Aucune sortie du tout :
- Vérifiez le câble USB et le port de la carte.
- Confirmez que le firmware s'exécute.
- Confirmez que le débit est 115200.

Du texte lisible apparaît, mais le DAW ne reçoit pas de MIDI :
- Sur le firmware actuel, le logging ne désactive pas la sortie MIDI.
- S'il n'y a toujours pas de MIDI, vérifiez si votre carte utilise le transport USB MIDI ou MIDI série.
- Pour les cartes non USB, utilisez une passerelle série-vers-MIDI.

La carte démarre, mais aucun périphérique USB MIDI n'apparaît :
- Vérifiez que le profil de carte sélectionné dans l'outil Arduino est bien un Pro Micro / Leonardo / Micro compatible USB.
- Reprogrammez puis rebranchez l'USB pour forcer la réénumération côté hôte.
- Si votre toolchain ne construit pas le chemin USB MIDI, Vounce basculera vers la sortie MIDI série.

Le menu ne s'ouvre pas :
- Vérifiez le câblage du bouton utilisé par `menuButton` dans [vounce.ino](vounce.ino).
- Vérifiez la durée de l'appui long dans l'initialisation de `ButtonInput`.

Le comportement des LEDs semble incorrect :
- Vérifiez le câblage des deux LEDs.
- Assurez-vous que la polarité et les résistances des LEDs sont correctes.

Le MIDI s'arrête pendant l'édition :
- C'est normal.
- Vounce met en pause le MIDI quand le menu est ouvert, puis reprend à la fermeture.

## Où personnaliser

- Câblage et assignation des pins : [vounce.ino](vounce.ino)
- Logique du menu : [src/menu/ConfigMenu.cpp](src/menu/ConfigMenu.cpp)
- Mode de transport MIDI : [src/midi/MidiOut.cpp](src/midi/MidiOut.cpp)
- Comportement des LEDs : [src/led/LedIndicator.cpp](src/led/LedIndicator.cpp)
- Logs non-MIDI : [src/utils/Logger.cpp](src/utils/Logger.cpp)

---