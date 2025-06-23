
#include "main.h"

int main()
{
    ini_echo();
    init_bomba();
    set_matrix();
    init_display();
    init_button();
    
    while (true) 
    {
        controle_bomba();
        controle_display();
    }

    return 0;
}