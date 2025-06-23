#include "FeedbackVisual.h"
#include "ControleDeNivel.h"

/********************* Variáveis Globais **************************/

ssd1306_t ssd; // Inicializa a estrutura do display

static volatile uint32_t last_time = 0; // Armazena o tempo do último clique dos botões (debounce)

bool menu = true;

float nivel = 60;

char str_altura_maxima[3];
char str_altura_minima[3];
char str_nivel[3];

/****************** Implementação das Funções *********************/

void gpio_irq_handler(uint gpio, uint32_t events){
    //Debouncing
    uint32_t current_time = to_us_since_boot(get_absolute_time()); // Pega o tempo atual e transforma em us
    if(current_time - last_time > 1000000){
        last_time = current_time; // Atualização de tempo do último clique
        if(gpio == button_B){
            menu = !menu;
            controle_display();
        }
    }
}

void init_button(){
    gpio_init(button_B); // Inicia a GPIO 6 do botão B
    gpio_set_dir(button_B, GPIO_IN); // Define a direção da GPIO 6 do botão B como entrada
    gpio_pull_up(button_B); // Habilita o resistor de pull up da GPIO 6 do botão B
    gpio_set_irq_enabled_with_callback(button_B, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler); // Configura a interrupção no botão B
}

void init_display(){
    i2c_init(display_i2c_port, 400 * 1000); // Inicializa o I2C usando 400kHz
    gpio_set_function(display_i2c_sda, GPIO_FUNC_I2C); // Define o pino SDA (GPIO 14) na função I2C
    gpio_set_function(display_i2c_scl, GPIO_FUNC_I2C); // Define o pino SCL (GPIO 15) na função I2C
    gpio_pull_up(display_i2c_sda); // Ativa o resistor de pull up para o pino SDA (GPIO 14)
    gpio_pull_up(display_i2c_scl); // Ativa o resistor de pull up para o pino SCL (GPIO 15)
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, display_i2c_endereco, display_i2c_port); // Inicializa o display
    ssd1306_config(&ssd); // Configura o display
    ssd1306_send_data(&ssd); // Envia os dados para o display
    ssd1306_fill(&ssd, false); // Limpa o display
    ssd1306_send_data(&ssd); // Atualiza o display
}

void controle_display(){
    ssd1306_fill(&ssd, false); // Limpa o display
    ssd1306_rect(&ssd, 0, 0, 127, 63, true, false); // Borda principal

    sprintf(str_altura_maxima, "%.0f", altura_maxima); // Converte float em string
    sprintf(str_altura_minima, "%.0f", altura_minima); // Converte float em string
    sprintf(str_nivel, "%.0f", nivel); // Converte float em string

    if(menu){

        // TELA 1 - Principal
        // Bordas
        ssd1306_line(&ssd, 1, 12, 126, 12, true); // Desenha uma linha horizontal
        ssd1306_line(&ssd, 1, 24, 126, 24, true); // Desenha uma linha horizontal
        ssd1306_line(&ssd, 1, 42, 126, 42, true); // Desenha uma linha horizontal
        ssd1306_line(&ssd, 63, 25, 63, 62, true); // Desenha uma linha vertical
    
        ssd1306_draw_string(&ssd, "Nivel de Agua", 11, 3); // Desenha uma string
        ssd1306_draw_string(&ssd, "Wifi:", 4, 15); // Desenha uma string
        ssd1306_draw_string(&ssd, "Conectando", 44, 15); // Desenha uma string - prototipo para a variavel de wifi

        // Quadrante do máximo
        ssd1306_draw_string(&ssd, "Max:", 3, 30); // Desenha uma string
        if(altura_maxima > 99.5){
            ssd1306_draw_string(&ssd, str_altura_maxima, 35, 30); // Desenha uma string
        }else if(altura_maxima > 9.5){
            ssd1306_draw_string(&ssd, str_altura_maxima, 39, 30); // Desenha uma string
        }else{
            ssd1306_draw_string(&ssd, str_altura_maxima, 43, 30); // Desenha uma string
        }

        // Quadrante do mínimo
        ssd1306_draw_string(&ssd, "Min:", 66, 30); // Desenha uma string
        if(altura_minima > 99.5){
            ssd1306_draw_string(&ssd, str_altura_minima, 98, 30); // Desenha uma string
        }else if(altura_minima > 9.5){
            ssd1306_draw_string(&ssd, str_altura_minima, 102, 30); // Desenha uma string
        }else{
            ssd1306_draw_string(&ssd, str_altura_minima, 106, 30); // Desenha uma string
        }
        
        // Quadrante do nível
        ssd1306_draw_string(&ssd, "Niv:", 3, 48); // Desenha uma string
        if(nivel > 99.5){
            ssd1306_draw_string(&ssd, str_nivel, 35, 48); // Desenha uma string
        }else if(nivel > 9.5){
            ssd1306_draw_string(&ssd, str_nivel, 39, 48); // Desenha uma string
        }else{
            ssd1306_draw_string(&ssd, str_nivel, 43, 48); // Desenha uma string
        }

        // Quadrante da bomba
        ssd1306_draw_string(&ssd, "Bom:", 66, 48); // Desenha uma string
        if(gpio_get(ACI_BOMBA)){
            ssd1306_draw_string(&ssd, "On", 102, 48); // Desenha uma string
        }else{
            ssd1306_draw_string(&ssd, "Off", 98, 48); // Desenha uma string
        }

    }else{
        // TELA 2 - Secundária
        ssd1306_rect(&ssd, 2, 100, 10, 59, true, false); // Borda
        float y = (61 - (nivel*0.59));
        ssd1306_rect(&ssd, y, 100, 10, (61 - y), true, true); // Preenchimento

        float y_max = (61 - (altura_maxima*0.59));
        float y_min = (61 - (altura_minima*0.59));
        ssd1306_line(&ssd, 99, y_max, 110, y_max, true); // Desenha uma linha horizontal (altura máxima)
        ssd1306_line(&ssd, 99, y_min, 110, y_min, true); // Desenha uma linha horizontal (altura mínima)

        ssd1306_draw_string(&ssd, "Nivel:", 4, 5); // Desenha uma string
        ssd1306_draw_string(&ssd, str_nivel, 52, 5); // Desenha uma string

        ssd1306_draw_string(&ssd, "Maximo:", 4, 20); // Desenha uma string
        ssd1306_draw_string(&ssd, str_altura_maxima, 60, 20); // Desenha uma string

        ssd1306_draw_string(&ssd, "Minimo:", 4, 35); // Desenha uma string
        ssd1306_draw_string(&ssd, str_altura_minima, 60, 35); // Desenha uma string

        ssd1306_draw_string(&ssd, "Bomba:", 4, 50); // Desenha uma string
        if(gpio_get(ACI_BOMBA)){
            ssd1306_draw_string(&ssd, "On", 52, 50); // Desenha uma string
        }else{
            ssd1306_draw_string(&ssd, "Off", 52, 50); // Desenha uma string
        }
    }
    ssd1306_send_data(&ssd); // Atualiza o display
}