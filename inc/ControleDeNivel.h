/***
 * @brief Arquivo de cabel
 */

 #ifndef CONTROLE_DE_NIVEL_H
 #define  CONTROLE_DE_NIVEL_H

/***********************  Includes ***************************/
#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/bootrom.h"

/***************  Defines e Variaveis Globais ****************/
#define led_g 11
#define led_r 13

#define TRIG_PIN 17 //Trig pin do sensor ultrassonico
#define ECHO_PIN 16 //Echo pin do sensor ultrassonico
#define ACI_BOMBA 18 //Pino de acionamento da bomba

extern float altura_maxima, altura_minima, altura_recipiente;

/****************** Prototipo de Funções *********************/

void ini_echo();
void init_bomba();

float calcular_distancia();
bool controle_bomba();

#endif // CONTROLE_DE_NIVEL_H