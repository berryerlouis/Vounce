# Vounce Contrôleur MIDI - Guide d'utilisation

Ce guide est destiné aux musiciens et makers qui souhaitent utiliser Vounce comme contrôleur MIDI.

## Ce que fait ce contrôleur

Vounce convertit les commandes physiques activées en messages MIDI Control Change (CC). Selon les familles de capteurs activées dans `src/conf/conf.h`, la build peut inclure :
- Encodeur rotatif
- Bouton poussoir / commutateur
- Potentiomètre
- Capteur de distance ultrasonique

Vous pouvez modifier les paramètres des capteurs supportés à l'exécution via le port série, sans reprogrammer le firmware.

## Avant de commencer

Vous avez besoin de :
- Votre carte programmée avec le firmware Vounce
- Un câble USB branché à votre ordinateur
- Un moniteur série pour la configuration
- Un DAW ou un moniteur MIDI pour tester la sortie MIDI

Notes de connexion :
- Sur une build Pro Micro USB, Vounce peut apparaître comme un périphérique USB MIDI natif.
- Sur une build sans USB MIDI, lisez la note sur la sortie série ci-dessous avant de prévoir une passerelle série-vers-MIDI.

Avant de programmer la carte, choisissez :
- Le profil de carte dans `src/conf/pinout.h`
- Les familles de capteurs actives dans `src/conf/conf.h`

## LED de statut

Le firmware actuel utilise une seule LED de statut sur `PIN_LED_STATUS`.

- Brièvement ALLUMÉE pendant le démarrage
- ÉTEINTE pendant le fonctionnement normal
- Clignote pendant environ 1 seconde après la sauvegarde d'un paramètre en EEPROM

Il n'y a plus de LED idle séparée ni d'état LED menu permanent.

## Fonctionnement de base

### Mode de lecture normal

Pendant le fonctionnement normal :
- Les capteurs activés sont mis à jour en continu
- Les changements en attente sont envoyés en MIDI CC
- La LED de statut reste éteinte, sauf juste après une sauvegarde

Flux d'exécution principal : [vounce.ino](vounce.ino)

### Configurer via le port série

Contrôles :
- `ls` : lister les capteurs
- `s <index>` : sélectionner un capteur par index (base 1)
- `p <index>` : sélectionner un paramètre par index (base 1)
- `v` : afficher la valeur courante du paramètre et sa plage autorisée
- `v <valeur>` : définir directement la valeur du paramètre sélectionné
- `h` : afficher l'aide des commandes

États du menu :
- SelectSensor : choisir quel capteur éditer
- SelectParam : choisir un paramètre
- EditParam : modifier la valeur du paramètre

Notes :
- Les index sont en base 1 dans le menu.
- Les canaux MIDI affichés à l'exécution vont de `1` à `16`.
- Le firmware actuel n'utilise plus le flux de menu physique en appui court / appui long.
- Quand vous envoyez `v <valeur>`, le changement est appliqué immédiatement puis sauvegardé en EEPROM.

Transitions historiques : [MENU_FLOW.md](MENU_FLOW.md)

### Utiliser le configurateur navigateur

Si vous préférez un éditeur visuel plutôt que des commandes manuelles, utilisez [sensor-configurator.html](sensor-configurator.html).

- Servez le dépôt depuis `localhost` avec un petit serveur statique.
- Ouvrez la page dans Edge ou Chrome.
- Cliquez sur `Connect`, choisissez le port série Vounce, puis utilisez les cartes capteurs et contrôles générés.

La page utilise le même protocole que le menu série, supprime les préfixes de log comme `[INFO]` et `[WARN]`, et sauvegarde toujours les valeurs en EEPROM via le chemin normal `v <valeur>`.

## Important : comportement actuel de la sortie MIDI et série

Le comportement actuel du firmware est le suivant :

- Chaque CC envoyé est aussi imprimé sur le port série sous forme lisible, par exemple `Channel: 14, Control: 10, Value: 64`
- Sur les builds Pro Micro / Leonardo / Micro compatibles, ce même CC est aussi envoyé via USB MIDI natif
- Sur les builds non USB, ce même CC est aussi écrit en octets MIDI série bruts

- `ENABLE_SERIAL_LOGGING` contrôle seulement les messages supplémentaires du `Logger`, comme les lignes `[INFO]` du menu et de l'EEPROM
- Il ne désactive pas la trace CC imprimée par `MidiOut`

Référence : [src/midi/MidiOut.h](src/midi/MidiOut.h), [src/midi/MidiOut.cpp](src/midi/MidiOut.cpp)

Conséquence pratique :
- Les builds Pro Micro USB sont les plus simples pour avoir MIDI et configuration série en parallèle.
- Sur les cartes non USB, une même connexion série transporte actuellement du texte lisible, des logs optionnels et des octets MIDI bruts.

## Première configuration - Checklist

1. Programmez le firmware sur votre carte.
2. Sélectionnez le bon profil de carte dans `src/conf/pinout.h`.
3. Activez les capteurs souhaités dans `src/conf/conf.h`.
4. Branchez l'USB.
5. Vérifiez que la vitesse série est 115200.
6. Ouvrez le Moniteur Série avec fin de ligne `Newline` ou `Both NL & CR`.
7. Envoyez `h` et vérifiez que les commandes du menu s'affichent.
8. Envoyez `ls` et confirmez que les capteurs compilés apparaissent.
9. Bougez les commandes et confirmez que la sortie CC lisible apparaît.
10. Si vous utilisez une build Pro Micro compatible USB, confirmez aussi que le DAW voit un périphérique MIDI.

## Flux de travail lors d'une performance

1. Allumez l'appareil.
2. Laissez le mode lecture actif et envoyez des commandes série seulement quand nécessaire.
3. Si vous avez besoin d'ajuster la réponse, utilisez `ls` puis `s <index>` pour sélectionner un capteur.
4. Utilisez `p <index>` puis `v` pour consulter la valeur courante et sa plage.
5. Envoyez `v <valeur>` pour appliquer et sauvegarder un nouveau réglage.
6. Continuez à jouer ; les changements sont appliqués immédiatement.

## Comportement de l'encodeur et du bouton

- Appuyer sur le bouton de l'encodeur ne modifie pas la valeur MIDI de l'encodeur.
- Le mouvement de l'encodeur est ignoré tant que le bouton est maintenu.
- Après le relâchement, le mouvement de l'encodeur est brièvement bloqué pour éviter de faux deltas dus au rebond.
- L'édition du menu se fait via des commandes série.
- L'entrée du bouton ne bascule qu'après un relâchement valide, ce qui rejette les faux appuis très courts dus au bruit mécanique pendant la rotation.

## Référence des paramètres des capteurs

Chaque capteur peut être configuré via le menu avec différents paramètres. Cette section liste tous les paramètres éditables pour chaque type de capteur.

### Capteur Encodeur

Le contrôle encodeur rotatif.

| Paramètre | Min | Max | Pas | Description |
|-----------|-----|-----|-----|-------------|
| midi_channel | 1 | 16 | 1 | Canal MIDI affiché dans le menu |
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
| midi_channel | 1 | 16 | 1 | Canal MIDI affiché dans le menu |
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
| midi_channel | 1 | 16 | 1 | Canal MIDI affiché dans le menu |
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
| midi_channel | 1 | 16 | 1 | Canal MIDI affiché dans le menu |
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
- Vérifiez si votre carte utilise le transport USB MIDI ou MIDI série.
- Sur les builds Pro Micro USB, vérifiez que le profil de carte expose bien les macros USB utilisées par le firmware.
- Sur les cartes non USB, rappelez-vous que le flux série actuel mélange texte lisible et octets MIDI série.

La carte démarre, mais aucun périphérique USB MIDI n'apparaît :
- Vérifiez que le profil de carte sélectionné dans l'outil Arduino est bien un Pro Micro / Leonardo / Micro compatible USB.
- Reprogrammez puis rebranchez l'USB pour forcer la réénumération côté hôte.
- Si votre toolchain ne construit pas le chemin USB MIDI, Vounce basculera vers la sortie MIDI série.

Les commandes du menu ne répondent pas :
- Vérifiez que le Moniteur Série est à 115200 bauds et que la fin de ligne est Newline ou Both NL & CR.
- Envoyez `h` pour afficher les commandes du menu.
- Rappelez-vous que les commandes de sélection valides sont `s <index>` et `p <index>`.

Impossible de sélectionner capteur/paramètre :
- Utilisez `ls` d'abord pour afficher la liste des capteurs et les index.
- Utilisez `s <index>` et `p <index>` avec des index en base 1.

Le comportement des LEDs semble incorrect :
- Vérifiez le câblage de `PIN_LED_STATUS`.
- Assurez-vous que la polarité et la résistance sont correctes.
- Le clignotement n'a lieu qu'après une sauvegarde réussie, pas pendant la simple navigation.

Le MIDI s'arrête pendant l'édition :
- Vérifiez le câblage et l'activation des capteurs dans [vounce.ino](vounce.ino).
- Dans l'exemple de câblage actuel, le streaming capteur reste actif pendant l'édition.

## Où personnaliser

- Câblage et assignation des pins : [vounce.ino](vounce.ino)
- Sélection du profil de carte : [src/conf/pinout.h](src/conf/pinout.h)
- Activation des familles de capteurs : [src/conf/conf.h](src/conf/conf.h)
- Logique du menu : [src/menu/ConfigMenu.cpp](src/menu/ConfigMenu.cpp)
- Mode de transport MIDI : [src/midi/MidiOut.cpp](src/midi/MidiOut.cpp)
- Comportement de la LED : [src/led/Led.cpp](src/led/Led.cpp)
- Logs non-MIDI : [src/utils/Logger.cpp](src/utils/Logger.cpp)

---