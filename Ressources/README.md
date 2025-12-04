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

### 7.2. Mesure de courant

### 7.3. Mesure de vitesse