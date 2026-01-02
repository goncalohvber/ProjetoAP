#include "Estacionamento.h"
#include "Util.h"
#include "Instalacao.h"
#include "Listagens.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
void menuGestaoLugares(Confparque config);
void atualizarValidacao(Confparque config);
int personalizapp(Confparque config);
void LimpaFicheiroEstac(char *ficheirobase, char *ficheirovalido, char *ficheiroerros, Confparque config);
int lerconfig(Confparque *config);
void gerarficheiroocupacao(char *ficheirovalido, char *ficheiroocupacao,
                           int diaU, int mesU, int anoU,int horaU,int minU);
int ABissexto(int ano){
    if ((ano % 4 == 0 && ano % 100 != 0) || (ano % 400 == 0)) {
            return 1;
        }
        return 0;
    }

int validaData(int dia, int mes, int ano) {
    if (ano < 0 || ano > 2100|| mes < 1 || mes > 12) {
        return 0;
    }

    int diasnomes;
    if (mes == 2) {
        if (ABissexto(ano)) {
            diasnomes = 29;
        } else {
            diasnomes = 28;
        }
    } else if (mes == 4 || mes == 6 || mes == 9 || mes == 11) {
        diasnomes = 30;
    } else {
        diasnomes = 31;
    }

    if (dia >= 1 && dia <= diasnomes) {
        return 1;
    } else {
        return 0;
    }
}

int validamatricula(char *mat) {
        // Verificar o tamanho
        // Se não tiver exatamente 8 caracteres (ex: "AA-00-AA"), falha logo.
        if (strlen(mat) != 8) {
            return 0;
        }

        // Verificar os separadores
        // As posições 2 e 5 devem ser o traço
        if (mat[2] != '-' || mat[5] != '-') {
            return 0;
        }

        //Verificar os caracteres alfanuméricos
        for (int i = 0; i < 8; i++) {
            if (i == 2 || i == 5) { //para ignorar os hifens
                continue;
            }

            if (!isalnum(mat[i])) {
                return 0;
            }
        }
        //passou por tudo, valida
        return 1;
    }

int validaLugar(char *lugar, int maxPisos, char maxFila, int maxLugares) {
    // Verificar tamanho mínimo
    if (strlen(lugar) < 3) {
        return 0;
    }
    
    // PISO - Deve ser um dígito
    if (!isdigit(lugar[0])) {
        return 0;
    }
    
    int piso = lugar[0] - '0';
    if (piso < 1 || piso > maxPisos) {
        return 0;
    }
    
    // FILA - Deve ser uma letra maiúscula
    char fila = lugar[1];
    if (fila < 'A' || fila > 'Z') {
        return 0;
    }
    if (fila > maxFila) {
        return 0;
    }
    
    // NÚMERO - Converter o resto da string para número
    int numero = atoi(&lugar[2]);
    if (numero < 1 || numero > maxLugares) {
        return 0;
    }
    
    return 1;
}

int validaEantesS(int diaE, int mesE, int anoE, int horaE, int minE, int diaS, int mesS, int anoS, int horaS, int minS) { //função que garante o carro sai, depois de entrar)
    if(anoE>anoS)
        return 0;
    if(anoS>anoE)
        return 1;
    
    //Se o código chegar aqui, os anos são iguais (o mesmo aplica-se aos meses, dias e horas, que vao seguir o mesmo formato
    
    if(mesE>mesS)
        return 0;
    if(mesS>mesE)
        return 1;
    
    if(diaE>diaS)
        return 0;
    if(diaS>diaE)
        return 1;
    
    if(horaE>horaS)
        return 0;
    if(horaS>horaE)
        return 1;
    
    if(minS>minE)
        return 1;
    
    return 0;
}

void mostrarMensagem(char *mens){
    printf("\n%s", mens);
    printf("\nDigite <Enter> para continuar... ");
    getchar();
    getchar();
}





int ComparaDatas(int d1, int m1, int a1, int h1, int min1,
                 int d2, int m2, int a2, int h2, int min2) {
    
    if (a1 > a2) return 1;
    if (a1 < a2) return -1;
    
    if (m1 > m2) return 1;
    if (m1 < m2) return -1;

    if (d1 > d2) return 1;
    if (d1 < d2) return -1;

    if (h1 > h2) return 1;
    if (h1 < h2) return -1;

    if (min1 > min2) return 1;
    if (min1 < min2) return -1;

    return 0;
}


void limparTela(void) {
    #ifdef _WIN32 // Se for Windows
        system("cls");
    #else // Para Linux, macOS...
        system("clear");
    #endif
}
