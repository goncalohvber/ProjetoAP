#include <stdlib.h>
#include <stdio.h>
#include "Instalacao.h"
#include "Tarifas.h"
#include "Estacionamento.h"
#include <locale.h>
#include "GestaoLugares.h"
#include "Util.h"


int main(void) {
    setlocale(LC_ALL, "Portuguese");
    
    FILE *F1 = fopen("configfeita.txt", "r");
    Confparque config;
    
    if (!F1) {
        personalizapp(config);
    } else {
        fclose(F1);
        mostrarMenu();
    }
    
    return 0;
    
    
}
