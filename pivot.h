#ifndef __PIVOT_h__
#define  __PIVOT_h__


const byte moteur_d = A1;
const byte moteur_g = A2;

float theta;
float t_rot;
uint8_t sens;
float vitesse;

const float a_max = 0.5;
const float v_max = 1.0;
const float e = 0.10; //VALEUR A CHANGER

int etat;
unsigned long time;


//On va effectuer une rotation ultra smooth, accel max divisée par 2
//Dans ce cas, même dans le pire des cas, un demi-tour, la vitesse maximale au niveau d'une roue serait de 0.4 m/s
// On va toujours partir du prncipe qu'on en va pas atteindre un plateau donc

void setup() {

    pinMode(A0, INPUT);
    pinMode(A1, OUTPUT);
    pinMode(A2, OUTPUT);

    etat = 0;

    Serial.begin(9600);
}

int pivot(float consigne_rot) {
    
    switch (etat) {

    case 0: {//On est vient de recevoir l'ordre
        theta = analogRead(consigne_rot);
        t_rot = sqrt(4 * e * abs(theta) / a_max);

        if (theta < 0) {
            sens = 0; //On tourne vers la droite
        }

        else {
            sens = 1; //On tourne vers la gauche
        }
        time = micros();
        etat = 1;
        break;
    }

    case 1: {//Phase d'acceleration
        if (micros() >= time + t_rot / 2) {
            time = micros();
            etat = 2;
        }
        else {
            vitesse = a_max * (micros() - time) / 2;
        }
        break;
    }

    case 2: {//Phase de décélération
        if (micros() >= time + t_rot / 2) {
            time = micros();
            etat = 3;
            vitesse = 0;
        }
        else {
            vitesse = a_max / 2 * (t_rot / 2 - micros() + time);
        }
        break;
    }

    case 3: { //Attente d'un nouvel ordre
        if (analogRead(consigne_rot) != 0) {
            etat = 0;
        }
        else {}
        break;
    }
    }

    if (sens == 0) { //on tourne à droite
        analogWrite(moteur_d, 127 - 127 * vitesse / v_max);
        analogWrite(moteur_g, 127 + 127 * vitesse / v_max);
    }

    else { //on tourne à gauche
        analogWrite(moteur_g, 127 - 127 * vitesse / v_max);
        analogWrite(moteur_d, 127 + 127 * vitesse / v_max);
    }

    return etat;
}

#endif
