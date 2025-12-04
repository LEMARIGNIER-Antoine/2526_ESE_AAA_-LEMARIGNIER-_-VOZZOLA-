## 📝 README du Projet d'Automatisation

### **Projet :** Actionneurs - #2526_ESE_AAA_-LEMARIGNIER-_-VOZZOLA-

---

### **1. 🚀 Introduction**

Ce dépôt contient l'ensemble des travaux pratiques (TP) et des comptes rendus associés au thème des **Actionneurs** dans le cadre du projet d'automatisation **#2526_ESE_AAA**.

L'objectif de ce module est d'étudier, de mettre en œuvre et de rendre compte du fonctionnement, de la programmation et des performances des différents types d'actionneurs utilisés dans les systèmes automatisés.

### **2. 📂 Structure du Dépôt**

Le dépôt est organisé de la manière suivante pour garantir la traçabilité des travaux :
* `Software/` : Contient les programmes sources (ex: Arduino, Python, Ladder, etc.) développés pour les TP.
    * `TPX_Actionneur_XXX/`
* `Ressources/` : Contient les fiches techniques, schémas ou documentation spécifique utilisés.

---

### **3. 🛠️ OBJECTIFS DU TP **

* Générer 4 PWM en complémentaire décalée pour contrôler en boucle ouverte le moteur en respectant le cahier des charges,
* Inclure le temps mort,
* Vérifier les signaux de commande à l'oscilloscope,
* Prendre en main le hacheur,
* Faire un premier essai de commande moteur.


Cahier des charges :
Fréquence de la PWM : 20kHz
Temps mort minimum : à voir selon la datasheet des transistors (faire valider la valeur)
Résolution minimum : 10bits.
Pour les tests, fixer le rapport cyclique à 60%.

On prendra donc avec une clock de 170 MHz, un ARR de 1024-1 pour avoir 10 bits  et un Prescaler de 4-1, car on a diviser par 2 le prescaler car on est en commande décalée pour avoir 20 kHz en sortie. Avec la datasheet du mosfet IRF540N, on relève un deadtime d'environ 170 ns ( reverse recovery time + rise/fall time), on prendra 200 ns donc sur l'IOC, 200/5,88( valeur datasheet)=34, on notera 34 dans le deadtime.
Pour un rapport cyclique de 60 % il suffit de prendre 60% de l'arr soit 614, on fera une fonction pour le rapport et avoir à choisir entre 0 et 100 et ne pas prendre en compte des valeurs en dehors.
On se placera en center aligned mode 1.
Voici les photos de nos PWM.
---

### **4. 🔗 Liens Utiles**

* [Lien vers le Cahier des Charges du Projet](https://...)
* [Documentation Technique Globale du Module Actionneurs](https://...)

---

### **5. ⚙️ Environnement de Travail / Outils**

Pour reproduire les résultats ou compiler les codes :

* **Logiciel de Programmation :** STM32CubeIDE
* **Langage :** C
* **Matériel Utilisé :** STM32

---

### **6. 📧 Contact**

Pour toute question ou information sur ce travail, veuillez contacter :

* LEMARIGNIER : [antoine.lemarignier@ensea.com]
* VOZZOLA : [louis.vozzola@ensea.com]
