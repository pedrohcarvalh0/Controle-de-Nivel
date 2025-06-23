#include "ControleDeNivel.h"



/********************* Variáveis Globais **************************/

float altura_maxima = 80.00, altura_minima = 20.00; //Variáveis altura máxima e minima em percentual

float altura_recipiente = 20; //Define a altura do recipiente como 20 cm

bool led_g_flag = 0, led_r_flag = 0;

PIO pio;
uint sm;
uint32_t VALOR_LED;
unsigned char R, G, B;

double v[] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.1, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.1, 0.0, 0.0, 0.0, 0.1, 0.0, 0.1, 0.0, 0.1, 0.0, 0.0, 0.0};
double apaga[] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
double emote[] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.1, 0.0, 0.1, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.1, 0.0, 0.0, 0.0, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1};

/****************** Implementação das Funções *********************/

void ini_echo()
{
    stdio_init_all(); // inicializa USB serial (para printf)

    gpio_init(TRIG_PIN);
    gpio_set_dir(TRIG_PIN, GPIO_OUT);

    gpio_init(ECHO_PIN);
    gpio_set_dir(ECHO_PIN, GPIO_IN);
}

void init_bomba()
{
    gpio_init(ACI_BOMBA);
    gpio_set_dir(ACI_BOMBA, GPIO_OUT);

    gpio_init(led_g);
    gpio_set_dir(led_g, GPIO_OUT);

    gpio_init(led_r);
    gpio_set_dir(led_r, GPIO_OUT);

    gpio_put(led_r, 1);
    gpio_put(led_g, 1);
    gpio_put(ACI_BOMBA, 0);

    sleep_ms(1000);
}

float calcular_distancia()
{
    gpio_put(TRIG_PIN, 0); // Garante trigger baixo
    sleep_us(2);

    
    // Pulso de 10µs no trigger
    gpio_put(TRIG_PIN, 1);
    sleep_us(10);
    gpio_put(TRIG_PIN, 0);

    // Espera echo subir
    uint32_t start = time_us_32();
    while (gpio_get(ECHO_PIN) == 0) 
    {
        if (time_us_32() - start > 30000) break; // timeout ~30 ms
    }

    uint32_t t0 = time_us_32();

    // Espera echo cair
    while (gpio_get(ECHO_PIN) == 1) 
    {
        if (time_us_32() - t0 > 30000) break; // timeout ~30 ms
    }
    uint32_t t1 = time_us_32();
    uint32_t pulse = t1 - t0;                  // duração em µ
    float distance_cm = pulse / 58.0f;         // conversão para cm

    return distance_cm;
}

bool controle_bomba()
{
    float altura_liquido = 0; bool controle_bomba = 0; uint amostras = 100;

    for(int i = 0; i<amostras; i++)
    {
        altura_liquido = altura_liquido + calcular_distancia();
        sleep_ms(10);
    }
    
    altura_liquido = altura_liquido/amostras;

    printf("altura liquido %0.2f\n", altura_liquido);

    if(altura_liquido <= (altura_minima/100)*altura_recipiente) //Desliga
    {
        led_g_flag  = 1;
        led_r_flag = !led_g_flag;

        controle_bomba = 1;
        gpio_put(ACI_BOMBA, controle_bomba);
        
        printf("Bomba Desligada\n");
        sleep_ms(1000);
    }
    else if(altura_liquido >= (altura_maxima/100)*altura_recipiente) //Liga
    {
        led_g_flag  = 0;
        led_r_flag = !led_g_flag;

        controle_bomba = 0;
        gpio_put(ACI_BOMBA, controle_bomba);
        desenho_pio_red(emote, VALOR_LED, pio, sm, R, G, B);
        printf("Bomba Acionada\n");
        sleep_ms(1000);
    }

    gpio_put(led_g, led_g_flag);
    gpio_put(led_r, led_r_flag);

    return controle_bomba;
}