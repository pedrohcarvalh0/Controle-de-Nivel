#include "ControleDeNivel.h"
#include "FeedbackVisual.h"

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

float nivel_atual_percent = 0.0;    // Nível atual em porcentagem
bool bomba_status = false;          // Status da bomba
char str_ip[24];
char str_status_wifi[10];

// HTML CORRIGIDO - não sobrescreve inputs quando usuário está editando
const char HTML_BODY[] =
    "<!DOCTYPE html>"
    "<html><head><meta charset='UTF-8'><title>Controle de Nivel</title>"
    "<style>"
    "body{font-family:Arial;text-align:center;margin:10px;background:#f5f5f5;}"
    ".box{max-width:500px;margin:0 auto;background:#fff;padding:20px;border:1px solid #ddd;}"
    "h1{color:#333;margin-bottom:20px;}"
    ".info{margin:15px 0;font-size:18px;}"
    "#nivel{font-size:24px;color:#007bff;font-weight:bold;}"
    "#bomba{font-size:18px;padding:5px 10px;border-radius:15px;}"
    ".on{background:#28a745;color:#fff;}"
    ".off{background:#dc3545;color:#fff;}"
    ".visual{width:100%;height:120px;background:#eee;border:2px solid #ccc;position:relative;margin:15px 0;}"
    ".agua{position:absolute;bottom:0;width:100%;background:#007bff;}"
    ".config{margin:20px 0;padding:15px;background:#f8f8f8;border:1px solid #ddd;}"
    "input{width:50px;padding:5px;margin:0 5px;text-align:center;}"
    "button{padding:10px 20px;margin:10px;background:#007bff;color:#fff;border:none;cursor:pointer;}"
    "</style>"
    "</head><body>"
    "<div class='box'>"
    "<h1>Controle de Nivel</h1>"
    "<div class='info'>Nivel: <span id='nivel'>--%</span></div>"
    "<div class='visual'><div id='agua' class='agua'></div></div>"
    "<div class='info'>Bomba: <span id='bomba' class='off'>DESLIGADA</span></div>"
    "<div class='config'>"
    "<div>Min: <input type='number' id='min' value='20' min='0' max='100'>%</div><br>"
    "<div>Max: <input type='number' id='max' value='80' min='0' max='100'>%</div><br>"
    "<button onclick='salvar()'>Salvar</button>"
    "</div>"
    "<div style='margin-top:20px;font-size:12px;color:#666;'>"
    "Atual: Min=<span id='min_atual'>20</span>% Max=<span id='max_atual'>80</span>%"
    "</div>"
    "</div>"
    "<script>"
    "function atualizar(){"
    "fetch('/estado').then(r=>r.json()).then(d=>{"
    "document.getElementById('nivel').innerText=d.nivel_atual.toFixed(1)+'%';"
    "document.getElementById('agua').style.height=d.nivel_atual+'%';"
    "var bomba=document.getElementById('bomba');"
    "if(d.bomba_ligada){bomba.innerText='LIGADA';bomba.className='on';}else{bomba.innerText='DESLIGADA';bomba.className='off';}"
    "document.getElementById('min_atual').innerText=d.altura_minima;"
    "document.getElementById('max_atual').innerText=d.altura_maxima;"
    "}).catch(e=>console.log('Erro:',e));"
    "}"
    "function salvar(){"
    "var min=document.getElementById('min').value;"
    "var max=document.getElementById('max').value;"
    "if(min&&max&&parseFloat(min)<parseFloat(max)){"
    "fetch('/config',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify({altura_minima:parseFloat(min),altura_maxima:parseFloat(max)})})"
    ".then(r=>r.json()).then(d=>{if(d.success){alert('Configuracao salva!');atualizar();}}).catch(e=>alert('Erro ao salvar'));"
    "}else{alert('Valores invalidos! Min deve ser menor que Max');}"
    "}"
    "setInterval(atualizar,3000);"
    "setTimeout(atualizar,1000);"
    "</script>"
    "</body></html>";

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
    som_buz(1000, 200);
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

    while (gpio_get(ECHO_PIN) == 1) 
    {
        if (time_us_32() - t0 > 30000) break; // timeout ~30 ms
    }
    uint32_t t1 = time_us_32();
    uint32_t pulse = t1 - t0;                  // duração em µ
    float distance_cm = pulse / 58.0f;         // conversão para cm

    return distance_cm;
}

// FUNÇÃO AJUSTADA - agora usa as variáveis configuráveis
bool controle_bomba()
{
    float altura_liquido = 0; 
    uint amostras = 100;

    for(int i = 0; i<amostras; i++)
    {
        altura_liquido = altura_liquido + calcular_distancia();
        sleep_ms(10);
    }
    
    altura_liquido = altura_liquido/amostras;

    atualizar_nivel_percent(altura_liquido);

    printf("Altura liquido: %.2f cm (%.1f%%) - Limites: Min=%.1f%% Max=%.1f%%\n", 
           altura_liquido, nivel_atual_percent, altura_minima, altura_maxima);

    // AJUSTADO: Agora usa as variáveis configuráveis altura_minima e altura_maxima
    if(nivel_atual_percent >= altura_maxima) // Nível alto - Desliga bomba
    {
        led_g_flag  = 1;
        led_r_flag = !led_g_flag;

        bomba_status = false;
        gpio_put(ACI_BOMBA, 0); // Desliga bomba
        
        desenho_pio_green(v, VALOR_LED, pio, sm, R, G, B);
        printf("Bomba DESLIGADA (nível alto: %.1f%% >= %.1f%%)\n", nivel_atual_percent, altura_maxima);
        sleep_ms(1000);
    }
    else if(nivel_atual_percent <= altura_minima) // Nível baixo - Liga bomba
    {
        led_g_flag  = 0;
        led_r_flag = !led_g_flag;

        bomba_status = true;
        gpio_put(ACI_BOMBA, 1); // Liga bomba
        desenho_pio_red(emote, VALOR_LED, pio, sm, R, G, B);
        printf("Bomba LIGADA (nível baixo: %.1f%% <= %.1f%%)\n", nivel_atual_percent, altura_minima);
        sleep_ms(1000);
    }

    gpio_put(led_g, led_g_flag);
    gpio_put(led_r, led_r_flag);

    return bomba_status;
}

void set_matrix () {

    // Setando matriz de leds
   double r = 0.0, b = 0.0 , g = 0.0;
   bool ok;
   ok = set_sys_clock_khz(128000, false);
   pio = pio0;

   uint offset = pio_add_program(pio, &main_program);
   uint sm = pio_claim_unused_sm(pio, true);
   main_program_init(pio, sm, offset, OUT_PIN);
}

//rotina para definição da intensidade de cores do led
uint32_t matrix_rgb(double b, double r, double g)
{
  //unsigned char R, G, B;
  R = r * 255;
  G = g * 255;
  B = b * 255;
  return (G << 24) | (R << 16) | (B << 8);
}

// Desenha na matriz de leds em verde
void desenho_pio_green(double *desenho, uint32_t valor_led, PIO pio, uint sm, double r, double g, double b){

    for (int16_t i = 0; i < NUM_PIXELS; i++) {
            valor_led = matrix_rgb(b = 0.0, r=0.0, desenho[24-i]);
            pio_sm_put_blocking(pio, sm, valor_led);
    }
}

// Desenha na matriz de leds em vermelho
void desenho_pio_red(double *desenho, uint32_t valor_led, PIO pio, uint sm, double r, double g, double b){

    for (int16_t i = 0; i < NUM_PIXELS; i++) {
            valor_led = matrix_rgb(b = 0.0, desenho[24-i], g = 0.0);
            pio_sm_put_blocking(pio, sm, valor_led);
    }
}


void atualizar_nivel_percent(float distancia)
{
    nivel_atual_percent = ((altura_recipiente - distancia) / altura_recipiente) * 100.0;
    if (nivel_atual_percent < 0) nivel_atual_percent = 0;
    if (nivel_atual_percent > 100) nivel_atual_percent = 100;
}

//Som Buzzer
void som_buz(uint16_t freq, uint16_t duration_ms)
{
    uint period = 1000000 / freq;  // Período do sinal em microssegundos
    uint cycles = (duration_ms * 1000) / period;  // Número de ciclos a gerar

    //Configuração do Buzzer A
    gpio_init(21);
    gpio_set_dir(21, GPIO_OUT);

    for (uint i = 0; i < cycles; i++) 
    {
        gpio_put(21, 1);  // Liga o buzzer
        sleep_us(period / 2);  // Espera metade do período
        gpio_put(21, 0);  // Desliga o buzzer
        sleep_us(period / 2);  // Espera metade do período
    }

}

// Funções do servidor HTTP
void init_wifi()
{
    printf("Iniciando Wi-Fi...\n");
    sprintf(str_status_wifi, "Iniciando");
    controle_display();
    
    if (cyw43_arch_init()) {
        printf("Falha ao inicializar Wi-Fi\n");
        sprintf(str_status_wifi, "Falha!");
        controle_display();
        return;
    }

    cyw43_arch_enable_sta_mode();
    
    printf("Conectando ao Wi-Fi...\n");
    sprintf(str_status_wifi, "Conectando");
    controle_display();

    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 30000)) {
        printf("Falha ao conectar ao Wi-Fi\n");
        sprintf(str_status_wifi, "Falha!");
        controle_display();
        return;
    }

    printf("Wi-Fi conectado!\n");
    sprintf(str_status_wifi, "Conectado!");
    controle_display();
    
    uint8_t *ip = (uint8_t *)&(cyw43_state.netif[0].ip_addr.addr);
    printf("IP: %d.%d.%d.%d\n", ip[0], ip[1], ip[2], ip[3]);
    snprintf(str_ip, sizeof(str_ip), "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
}

void parse_json_config(const char* json_data)
{
    char *min_pos = strstr(json_data, "altura_minima\":");
    char *max_pos = strstr(json_data, "altura_maxima\":");
    
    if (min_pos) {
        min_pos += 15;
        float new_min = atof(min_pos);
        if (new_min >= 0 && new_min <= 100 && new_min < altura_maxima) {
            altura_minima = new_min;
            printf("Altura mínima atualizada para: %.1f%%\n", altura_minima);
        }
    }
    
    if (max_pos) {
        max_pos += 15;
        float new_max = atof(max_pos);
        if (new_max >= 0 && new_max <= 100 && new_max > altura_minima) {
            altura_maxima = new_max;
            printf("Altura máxima atualizada para: %.1f%%\n", altura_maxima);
        }
    }
    
    printf("Configuração atual - Min: %.1f%%, Max: %.1f%%\n", altura_minima, altura_maxima);
}

static err_t http_sent(void *arg, struct tcp_pcb *tpcb, u16_t len)
{
    struct http_state *hs = (struct http_state *)arg;
    hs->sent += len;
    if (hs->sent >= hs->len) {
        tcp_close(tpcb);
        free(hs);
    }
    return ERR_OK;
}

static err_t http_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
    if (!p) {
        tcp_close(tpcb);
        return ERR_OK;
    }

    char *req = (char *)p->payload;
    struct http_state *hs = malloc(sizeof(struct http_state));
    if (!hs) {
        pbuf_free(p);
        tcp_close(tpcb);
        return ERR_MEM;
    }
    hs->sent = 0;

    if (strstr(req, "POST /config")) {
        char *json_start = strstr(req, "\r\n\r\n");
        if (json_start) {
            json_start += 4;
            parse_json_config(json_start);
        }
        
        const char *response = "{\"success\":true}";
        hs->len = snprintf(hs->response, sizeof(hs->response),
                          "HTTP/1.1 200 OK\r\n"
                          "Content-Type: application/json\r\n"
                          "Content-Length: %d\r\n"
                          "Connection: close\r\n"
                          "\r\n"
                          "%s",
                          (int)strlen(response), response);
    }
    else if (strstr(req, "GET /estado")) {
        char json_payload[150];
        int json_len = snprintf(json_payload, sizeof(json_payload),
                               "{\"nivel_atual\":%.1f,\"bomba_ligada\":%s,\"altura_minima\":%.0f,\"altura_maxima\":%.0f}",
                               nivel_atual_percent,
                               bomba_status ? "true" : "false",
                               altura_minima,
                               altura_maxima);

        hs->len = snprintf(hs->response, sizeof(hs->response),
                          "HTTP/1.1 200 OK\r\n"
                          "Content-Type: application/json\r\n"
                          "Content-Length: %d\r\n"
                          "Connection: close\r\n"
                          "\r\n"
                          "%s",
                          json_len, json_payload);
    }
    else {
        hs->len = snprintf(hs->response, sizeof(hs->response),
                          "HTTP/1.1 200 OK\r\n"
                          "Content-Type: text/html\r\n"
                          "Content-Length: %d\r\n"
                          "Connection: close\r\n"
                          "\r\n"
                          "%s",
                          (int)strlen(HTML_BODY), HTML_BODY);
    }

    tcp_arg(tpcb, hs);
    tcp_sent(tpcb, http_sent);
    tcp_write(tpcb, hs->response, hs->len, TCP_WRITE_FLAG_COPY);
    tcp_output(tpcb);
    pbuf_free(p);
    return ERR_OK;
}

static err_t connection_callback(void *arg, struct tcp_pcb *newpcb, err_t err)
{
    tcp_recv(newpcb, http_recv);
    return ERR_OK;
}

void start_http_server(void)
{
    struct tcp_pcb *pcb = tcp_new();
    if (!pcb) {
        printf("Erro ao criar PCB TCP\n");
        return;
    }
    if (tcp_bind(pcb, IP_ADDR_ANY, 80) != ERR_OK) {
        printf("Erro ao ligar o servidor na porta 80\n");
        return;
    }
    pcb = tcp_listen(pcb);
    tcp_accept(pcb, connection_callback);
    printf("Servidor HTTP rodando na porta 80...\n");
}
