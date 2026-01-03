#ifndef Tarifas_h
#define Tarifas_h
#define MAX_TARIFAS 10
#include <stdio.h>

//Estrutura que guarda todas as informações referentes às tarifas
typedef struct tarifa{
    char tipo;
    char codigo[10];
    int horaInf;
    int minInf;
    int horaSup;
    int minSup;
    float valor;
} Tarifa;

int guardartarifas(Tarifa tarifas[], int numtarifas);
int modificatarifa(Tarifa tarifas[], int *numtarifas);
void mostrarTabela(Tarifa tarifas[], int n);
int lertarifas(Tarifa tarifas[], int *numtarifas);
float ObterPreco(Tarifa tarifas[], int numTarifas, char *codigoProcurado);
int ObterHoraInicio(Tarifa tarifas[], int numTarifas, char *codigoProcurado);
float CalcularPreco(int dE, int mE, int aE, int hE, int minE,
                    int dS, int mS, int aS, int hS, int minS,
                    Tarifa tarifas[], int numTarifas);

#endif /* Tarifas_h */
