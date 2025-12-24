## 5. Shell

Jusqu'ici, nous prenons en main le shell et la strcuture globale du projet Cube IDE.
On le verra par la suite mais le shell va nous être très utile ! Notamment pour changer facilement la vitesse du moteur

## 6. Commande MCC basique 

### 6.1 Génération de 4 PWM

Cahier des charges :

Fréquence de la PWM : 20kHz

Temps mort minimum : à voir selon la datasheet des transistors

Résolution minimum : 10bits.

Notre nucléo STM32G474 peut être cadencé au maximum à 170MHz. On décide donc de travailler à cette fréquence et par conséquent on peut régler les paramètres de notre timer pour obtenir au moins 20kHz au borne du moteur avec une commande complémentaire décalée :

- En suivant la théorie, il faut que la commutation de nos MOSFET soit 2 fois supérieur à celle dans le moteur. De plus, on veut une précision d'au moins 10 bits. Par conséquent, on choisit PSC = 4 - 1 et ARR = 1024 - 1. On aura une fréquence de commutation MOSFET à 55 392Hz et du moteur à 27 696Hz > 20 000Hz.
<p align="center">
  <img src="../IMG/theorie_compl.png" width="400">
</p>

- Pour éviter de court-circuiter la carte, nous allons devoir imposer des deadtimes entre les commutations. D'après la datasheet des MOSFET (IRF540N),on relève un deadtime d'environ 170 ns ( reverse recovery time + rise/fall time).On prendra 200 ns donc sur l'IOC, 200/5,88( valeur datasheet)=34, on notera 34 dans le deadtime.
<p align="center">
  <img src="../IMG/DEADTIME.png" width="400">
</p>

Pour un rapport cyclique de 60 % il suffit de prendre 60% de l'ARR soit 614.


On peut alors tester tout cela avant de brancher le moteur :
<p align="center">
  <img src="../IMG/tek00001.png" width="800">
</p>

Le rapport cyclique est bien mis à 60%. On peut donc brancher le moteur et constaté que ça tourne correctement !

### 6.2. Commande de vitesse

Pour se faire, nous réaliser une fonction qui nous permet de changer facilement le PWM grâce au shell :

<div style="display: flex; gap: 40px;">

<div style="flex: 1;">
<pre>
<code class="language-c">
void Motor_SetDutyCycle(uint16_t ccr_value)
{
    // clamping
    if (ccr_value > MOTOR_CCR_MAX) {
        ccr_value = MOTOR_CCR_MAX;
    }

    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, ccr_value);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, MOTOR_CCR_MAX - ccr_value);
}
</code>
</pre>
</div>

<div style="flex: 1;">
<pre>
<code class="language-c">
static int sh_set_ccr(h_shell_t* h_shell, int argc, char** argv)
{
    // Vérification de l'argument
    if (argc < 2) {
        int size = snprintf(h_shell->print_buffer, SHELL_PRINT_BUFFER_SIZE,
                            "Erreur: Argument manquant. Usage: SETCCR <0-100>\\r\\n");
        h_shell->drv.transmit(h_shell->print_buffer, size);
        return -1;
    }

    int user_percent = atoi(argv[1]);

    if (user_percent < 0 || user_percent > 100) {
        int size = snprintf(h_shell->print_buffer, SHELL_PRINT_BUFFER_SIZE,
                            "Erreur: Valeur %d invalide. Entrez une valeur entre 0 et 100.\\r\\n",
                            user_percent);
        h_shell->drv.transmit(h_shell->print_buffer, size);
        return -1;
    }

    uint16_t ccr_register_value = (uint16_t)(((uint32_t)user_percent * MOTOR_CCR_MAX) / 100);

    Motor_SetDutyCycle(ccr_register_value);

    int size = snprintf(h_shell->print_buffer, SHELL_PRINT_BUFFER_SIZE,
                        "OK: Vitesse reglee a %d%% (Reg: %d/%d)\\r\\n",
                        user_percent, ccr_register_value, MOTOR_CCR_MAX);
    h_shell->drv.transmit(h_shell->print_buffer, size);

    return 0;
}
</code>
</pre>
</div>

</div>

### 6.3. Premiers tests

On réalise le test, d'abord à 50 % puis à 70 %. On constate un problème ! Lors du changement de vitesse, le courant augmente très rapidement et le moteur démarre de façon très sèche. 

Cela vient du fait que nous lui envoyons un échalon de tension "instantané" ce qui provoque la montée en flêche du courant et donc une mauvais démarrage. Pour remédier à cela, nous pouvons réaliser une fonction rample qui va permetre de faire monter doucement la tension donc la vitesse et donc le courant :
```C
void motor_ramp(uint16_t target_ccr)
{
    // Sécurisation de la valeur cible
    if (target_ccr > MOTOR_CCR_MAX) {
        target_ccr = MOTOR_CCR_MAX;
    }

    // Lecture de la valeur actuelle (approximation)
    uint16_t current_ccr = __HAL_TIM_GET_COMPARE(&htim1, TIM_CHANNEL_1);

    if (current_ccr < target_ccr)
    {
        // Rampe montée
        for (uint16_t ccr = current_ccr; ccr <= target_ccr; ccr += MOTOR_RAMP_STEP)
        {
            Motor_SetDutyCycle(ccr);
            HAL_Delay(MOTOR_RAMP_DELAY);
        }
    }
    else
    {
        // Rampe descente
        for (uint16_t ccr = current_ccr; ccr >= target_ccr; ccr -= MOTOR_RAMP_STEP)
        {
            Motor_SetDutyCycle(ccr);
            HAL_Delay(MOTOR_RAMP_DELAY);
            if (ccr < MOTOR_RAMP_STEP) break; // éviter underflow
        }
    }

    // Assurer que la valeur finale est exacte
    Motor_SetDutyCycle(target_ccr);
}
```

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
Avec un SETCCR=65%
<img width="314" height="78" alt="image" src="https://github.com/user-attachments/assets/cf98d527-54a4-48a2-96b1-73c3c6a37ff8" />

Une fois cette mesure validée, modifier la méthode d'acquisition de ces données en établissant une mesure à interval de temps régulier avec la mise en place d'une la chaine d'acquisition Timer/ADC/DMA. Vous pouvez utiliser le même timer que celui de la génération des PWM pour que les mesures de courant soient synchrones aux PWM. Pour vérifier cela, utiliser un GPIO disponible sur la carte pour établir une impulsion lors de la mesure de la valeur.

Pour lire le courant en DMA, on active le DMA sur l'adc, on fera un adc request et mettons le en mode circulaire :
<img width="776" height="526" alt="image" src="https://github.com/user-attachments/assets/9681768e-407e-4d67-95c6-10b61dcfcaea" />

On vérifie bien la résolution de l'adc pour nos mesures ( 4096 bits) :
<img width="958" height="44" alt="image" src="https://github.com/user-attachments/assets/e6cfd313-c583-4bc0-916d-5301815324aa" />

Prenons 10 valeurs et prenons le timer 1, celui de la génération de nos PWM comme trigger : 
<img width="1338" height="72" alt="image" src="https://github.com/user-attachments/assets/70dc111d-82d8-4b37-b845-ee169f551313" />

Activons aussi les interrupt sur l'ADC :
<img width="1600" height="134" alt="image" src="https://github.com/user-attachments/assets/89dad87b-39e4-4582-ba2f-a8ad3be2b22e" />

Ensuite, tout se passe dans le fichier suivant : input_analog.c
Voici le lien : https://github.com/LEMARIGNIER-Antoine/2526_ESE_AAA_-LEMARIGNIER-_-VOZZOLA-/blob/6fa8813b0fa559404555195125f8e4c78d4af746/software/NUCLEO-G474RET6-Inverter_Base/Core/Src/acquisition/input_analog.c

Nous avons une fonction pour iniatialiser l'analog, nous ferrons une calibration sur l'ADC et nous ferrons un start en DMA et sur le timer qui nous sert de trigger :

```c
void input_analog_init(void){
	HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED);
	HAL_ADC_Start_DMA(&hadc1, (uint16_t*)&adc_dma_buffer, 10);
	HAL_TIM_Base_Start(&htim1);
}
```
Créeons la fonction principale de lecture de nos valeurs qui reprend la fonction de transfert du capteur de courant ( toutes nos variables utilisés sont définies en amont ou dans la fonction, elles n'apparaissent pas sans être définies mais par soucis de lisibilité, montrons le coeur du sujet ): Nous faisons une moyenne sur 10 valeurs lues en DMA.

```c
void ADC_Process_DMA_Conversion(void)
{
	float Uout = 0.0f;
	int i=0;
	float sum=0;

	for (int i=0;i<10;i++)
	{
		Uout = ((float)adc_dma_buffer[i] * V_SUPPLY) / ADC_RESOLUTION;
		sum=sum+Uout;
	}
	sum=sum/10.0;

	if (flag_init == 0)
	{
		SENSOR_OFFSET = sum ;
		flag_init = 1;

	}

	I_mes = (sum - SENSOR_OFFSET) / SENSOR_SENS;

}
```


Voici une photo de notre courant : 
<img width="1024" height="600" alt="SDS2104X Plus_PNG_39" src="https://github.com/user-attachments/assets/4c450560-2e65-4df5-a422-612ecea3b930" />

Nous avons aussi ajouté la possibilité de lire la valeur avec le shell :
dans la fonction init_device :
```c
shell_add(&hshell1, "IMES", sh_get_current, "Affiche le courant mesuré en Ampères");
```
et sa fonction associé :
```c
static int sh_get_current(h_shell_t* h_shell, int argc, char** argv)
{

	int size = snprintf(h_shell->print_buffer, SHELL_PRINT_BUFFER_SIZE, "Courant = %.2f A\r\n", I_mes);
	h_shell->drv.transmit(h_shell->print_buffer, size);

	return 0;
}
```

### 7.3. Mesure de vitesse
Déterminons le PPR de notre encodeur. On relève sur l'encodeur 10V/1000 tr, branchons un voltmètre entre Vt+ et Vt-, on fait tourner notre moteur, on relève 13 V entre Vt+ et Vt- donc en comparant avec la donnée relevée sur l'encodeur, nous sommees donc avec 13 V à 1300 tr/min donc 21,6tr/s avec une fréquence sur notre pin PA6 = encodeur A de f=21,8 kHz ( valeur lue avec l'oscilloscope), nous avons donc finalement une résolution de 1024 point/tr, nous nous placerons en quadrature donc on multiplira par 4.
<img width="646" height="78" alt="image" src="https://github.com/user-attachments/assets/837bcc47-fa3c-4b05-bfc5-0e95a5f6ed6d" />


Déterminer la constant de temps mécanique du moteur :
Prenons un oscilloscope et regardons sur le pin PA6 la réponse indicielle :
<img width="1024" height="600" alt="SDS2104X Plus_PNG_80" src="https://github.com/user-attachments/assets/923df351-cfd3-4853-b2b4-ea37682bb63a" />
Regardons pour 3 tau, nous relevons 260 ms donc tau = 87ms. On constate de plus que la réponse temporelle est similaire à un premier ordre (pente à l'origine non nulle), donc on peut en déduire la fréquence de coupure :

$f_c = \frac{1}{2\pi \tau} = 1.83 Hz$

Déterminer la fréquence à laquelle vous allez faire l'asservissement en vitesse du moteur :

Pour respecter Shannon, nous devons prendre une fréquence supérieur à 4 Hz , on peut donc prendre 100Hz 

Déterminer les pin du stm32 utilisées pour faire cette mesure de vitesse, sur le datasheet : 

Enc_A=PA6

Enc_B=PA4

Enc_Z=PC8

En regardant l'IOC, cela nous aménera à utiliser le timer 3. Plaçons le en mode encoder :

<img width="636" height="46" alt="image" src="https://github.com/user-attachments/assets/b701732e-6029-4c40-a59a-54b627fa575b" />

Les fonctions de l'encoder seront instanciés dans le fichier suivant : input_encoder.c disponible ici :
https://github.com/LEMARIGNIER-Antoine/2526_ESE_AAA_-LEMARIGNIER-_-VOZZOLA-/blob/733ccdf04a4a6cdad2b4eca7dd3fb6a5cb4f20d9/software/NUCLEO-G474RET6-Inverter_Base/Core/Src/acquisition/input_encoder.c

On initialisera notre timer et dans la librairie HAL, __HAL_TIM_GET_COUNTER(&htim3); permet de counter ce qui est très pratique.

Nous avons aussi ajouté la possibilité de lire la valeur avec le shell :
dans la fonction init_device :
```c
shell_add(&hshell1, "ENC", sh_get_encoder, "Affiche la valeur de l'encodeur");
```
avec sa fonction :
```c
static int sh_get_encoder(h_shell_t* h_shell, int argc, char** argv)
{

	uint32_t cnt_actuel = encoder_get_counter();


	int size = snprintf(h_shell->print_buffer, SHELL_PRINT_BUFFER_SIZE, " CNT: %lu \r\n", nb_tours, cnt_actuel);

	h_shell->drv.transmit(h_shell->print_buffer, size);

	return 0;
}
```


