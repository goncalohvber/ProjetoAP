#ifndef Funcs_h
#define Funcs_h
#include <string.h>
#include <stdio.h>
#define MAX_PISOS 5
#define MAX_FILAS 26
#define MAX_LUGARES 50
#define Ficheiro_config "ficheiroconf.txt"
#define MAX_REG_EST 100000
typedef struct{
    int numpisos, numfilas, numlugares;
    char Nomempresa[50], Nominstal[50];
}Confparque;

int lerconfig(Confparque *config);
int gravarconfig(const Confparque *config);
int personalizapp(Confparque config);

#endif /* Funcs_h */
