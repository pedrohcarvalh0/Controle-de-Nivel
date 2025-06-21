#include "ControleDeNivel.h"

void ini_echo()
{
    stdio_init_all(); // inicializa USB serial (para printf)
    
    gpio_init(TRIG_PIN);
    gpio_set_dir(TRIG_PIN, GPIO_OUT);

    gpio_init(ECHO_PIN);
    gpio_set_dir(ECHO_PIN, GPIO_IN);
}

void calcular_distancia()
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

    if (pulse == 0 || pulse > 30000) 
    {
        printf("Fora de alcance\n");
    } 
    else 
    {
        printf("Distância: %.2f cm\n", distance_cm);
    }
    sleep_ms(500);
}