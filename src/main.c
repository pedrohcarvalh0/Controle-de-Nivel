
#include "main.h"

int main()
{
    ini_echo();
    init_bomba();
    set_matrix();
    
    // Inicializar Wi-Fi e servidor HTTP
    init_wifi();
    start_http_server();
    
    printf("Sistema inicializado com sucesso!\n");
    printf("Acesse a interface web pelo IP mostrado acima\n");
    
    // Loop principal
    while (true)
    {
        // Processar requisições de rede
        cyw43_arch_poll();
        
        // Executar controle da bomba
        controle_bomba();
        
        // Pequeno delay para não sobrecarregar o sistema
        sleep_ms(100);
    }

    // Cleanup (nunca será executado neste caso)
    cyw43_arch_deinit();

    return 0;
}