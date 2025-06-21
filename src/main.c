
#include "main.h"

int main()
{
    ini_echo();
    init_bomba();

    while (true) 
    {
        controle_bomba();
    }

    return 0;
}