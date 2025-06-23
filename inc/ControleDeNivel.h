/***
 * @brief Arquivo de cabel
 */

 #ifndef CONTROLE_DE_NIVEL_H
 #define  CONTROLE_DE_NIVEL_H

/***********************  Includes ***************************/
#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/bootrom.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "pio_matrix.pio.h"

/***************  Defines e Variaveis Globais ****************/
#define led_g 11
#define led_r 13

#define TRIG_PIN 17 //Trig pin do sensor ultrassonico
#define ECHO_PIN 16 //Echo pin do sensor ultrassonico
#define ACI_BOMBA 18 //Pino de acionamento da bomba

extern float altura_maxima, altura_minima, altura_recipiente;

extern PIO pio;
extern uint sm;
extern uint32_t VALOR_LED;
extern unsigned char R, G, B;
#define NUM_PIXELS 25
#define OUT_PIN 7

/****************** Prototipo de Funções *********************/

void ini_echo();
void init_bomba();
void set_matrix();
uint32_t matrix_rgb(double b, double r, double g);
void desenho_pio_green(double *desenho, uint32_t valor_led, PIO pio, uint sm, double r, double g, double b);
void desenho_pio_red(double *desenho, uint32_t valor_led, PIO pio, uint sm, double r, double g, double b);

float calcular_distancia();
bool controle_bomba();

#endif // CONTROLE_DE_NIVEL_H