## 6. Commande MCC basique 

### 6.1 Génération de 4 PWM

Cahier des charges :
Fréquence de la PWM : 20kHz
Temps mort minimum : à voir selon la datasheet des transistors (faire valider la valeur)
Résolution minimum : 10bits.
Pour les tests, fixer le rapport cyclique à 60%.

On prendra donc avec une clock de 170 MHz, un ARR de 1024-1 pour avoir 10 bits  et un Prescaler de 4-1, car on a diviser par 2 le prescaler car on est en commande décalée pour avoir 20 kHz en sortie. Avec la datasheet du mosfet IRF540N, on relève un deadtime d'environ 170 ns ( reverse recovery time + rise/fall time), on prendra 200 ns donc sur l'IOC, 200/5,88( valeur datasheet)=34, on notera 34 dans le deadtime.

Pour un rapport cyclique de 60 % il suffit de prendre 60% de l'arr soit 614, on fera une fonction pour le rapport et avoir à choisir entre 0 et 100 et ne pas prendre en compte des valeurs en dehors.

### 6.2. Commande de vitesse

### 6.3. Premiers tests

## 7. Commande en boucle ouverte, mesure de Vitesse et de courant

### 7.1. Commande de la vitesse
Nous avons rajouté Start et Stop qui peuvent être appelé depuis le Shell.
<img width="762" height="242" alt="image" src="https://github.com/user-attachments/assets/fe53671e-5ad0-45dc-91b5-07efd2df8bbb" />

### 7.2. Mesure de courant

- Définir quel(s) courant(s) vous devez mesurer? Pour la mesure du courant, nous allons mesurer la courant total qui passe dans le moteur



- Définir les fonctions de transfert des capteurs de mesure de courant (lecture datasheet)? En regardant la datasheet / Kicad, on se rend compte les capteurs utilisés sont des 61-GO10-SME de chez LEM avec notamment un courant RMS de 10 A et on aura :
$I=\frac{V_{mesuré}-1.65}{0.05}$

- Déterminer les pin du stm32 utilisés pour faire ces mesures de courant ? D'après le kicad, la lecture du couant pour U sera sur PA1 , V sera sur PB2 et W sera sur PB13
<img width="1646" height="1378" alt="image" src="https://github.com/user-attachments/assets/9e49453c-91f5-4c4e-8b58-4d8a04628d6d" />

Etablir une première mesure de courant avec les ADC en Pooling. Faites des tests à vitesse nulle, non nulle, et en charge (rajouter un couple resistif en consommant du courant sur la machine synchrone couplée à la MCC).
<img width="1876" height="1014" alt="image" src="https://github.com/user-attachments/assets/114576a1-b2cc-4b43-948a-5a152cdc348a" />


Une fois cette mesure validée, modifier la méthode d'acquisition de ces données en établissant une mesure à interval de temps régulier avec la mise en place d'une la chaine d'acquisition Timer/ADC/DMA. 

Vous pouvez utiliser le même timer que celui de la génération des PWM pour que les mesures de courant soient synchrones aux PWM. Pour vérifier cela, utiliser un GPIO disponible sur la carte pour établir une impulsion lors de la mesure de la valeur.
### 7.3. Mesure de vitesse
