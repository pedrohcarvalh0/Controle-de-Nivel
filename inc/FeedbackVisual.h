/***
 * @brief Arquivo de cabeçalho do Feedback Visual
 */

 #ifndef FEEDBACK_VISUAL_H
 #define  FEEDBACK_VISUAL_H

 /***********************  Includes ***************************/

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "ssd1306.h"
#include "font.h"

/***************  Defines e Variaveis Globais ****************/

#define display_i2c_port i2c1 // Define a porta I2C
#define display_i2c_sda 14 // Define o pino SDA na GPIO 14
#define display_i2c_scl 15 // Define o pino SCL na GPIO 15
#define display_i2c_endereco 0x3C // Define o endereço do I2C

#define button_B 6 // Botão B GPIO 6

/****************** Prototipo de Funções *********************/

void gpio_irq_handler(uint gpio, uint32_t events);
void init_button();
void init_display();
void controle_display();

#endif // FEEDBACK_VISUAL_H