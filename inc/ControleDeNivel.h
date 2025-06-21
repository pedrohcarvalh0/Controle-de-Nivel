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

#define TRIG_PIN 17 //Trig pin do sensor ultrassonico
#define ECHO_PIN 16 //Echo pin do sensor ultrassonico


/****************** Prototipo de Funções *********************/

void ini_echo();

void calcular_distancia();

#endif // CONTROLE_DE_NIVEL_H