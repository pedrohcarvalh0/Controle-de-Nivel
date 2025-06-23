
#include "main.h"

int main()
{
    ini_echo();
    init_bomba();
    set_matrix();
    
    while (true) 
    {
        controle_bomba();
    }

    return 0;
}