#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/bootrom.h"
#define BUTTON_B 6

// Ajuste de acordo com os GPIOs que você conectou
#define TRIG_PIN 17
#define ECHO_PIN 16

void gpio_irq_handler(uint gpio, uint32_t events)
{
  reset_usb_boot(0, 0);
}

int main()
{
    stdio_init_all();

    gpio_init(BUTTON_B);
    gpio_set_dir(BUTTON_B, GPIO_IN);
    gpio_pull_up(BUTTON_B);
    gpio_set_irq_enabled_with_callback(BUTTON_B, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

    // inicializa USB serial (para printf)
    gpio_init(TRIG_PIN);
    gpio_set_dir(TRIG_PIN, GPIO_OUT);
    gpio_init(ECHO_PIN);
    gpio_set_dir(ECHO_PIN, GPIO_IN);

    while (true) {
        // Garante trigger baixo
        gpio_put(TRIG_PIN, 0);
        sleep_us(2);
        // Pulso de 10µs no trigger
        gpio_put(TRIG_PIN, 1);
        sleep_us(10);
        gpio_put(TRIG_PIN, 0);

        // Espera echo subir
        uint32_t start = time_us_32();
        while (gpio_get(ECHO_PIN) == 0) {
            if (time_us_32() - start > 30000) break; // timeout ~30 ms
        }
        uint32_t t0 = time_us_32();

        // Espera echo cair
        while (gpio_get(ECHO_PIN) == 1) {
            if (time_us_32() - t0 > 30000) break; // timeout ~30 ms
        }
        uint32_t t1 = time_us_32();

        uint32_t pulse = t1 - t0;                  // duração em µs
        float distance_cm = pulse / 58.0f;         // conversão para cm

        if (pulse == 0 || pulse > 30000) {
            printf("Fora de alcance\n");
        } else {
            printf("Distância: %.2f cm\n", distance_cm);
        }

        sleep_ms(500);
    }

    return 0;
}
