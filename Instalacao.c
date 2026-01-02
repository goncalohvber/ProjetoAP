#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Instalacao.h"
#include "Tarifas.h"
#include "Util.h"
#include "Listagens.h"

void LimpaFicheiroEstac(char *ficheirobase, char *ficheirovalido, char *ficheiroerros, Confparque config);

int lerconfig(Confparque *config){
    if (config == NULL) {
        return 0;
    }

    FILE *f = fopen(Ficheiro_config, "r");
    if (f == NULL) {
        return 0; //Ficheiro não existe (primeira vez)
    }

    int numPisos = 0, numFilas = 0, numLugares = 0; //criamos variaveis temporarias para, antes de colocar os valores na struct ler e validar os mesmos.
    int resultado = fscanf(f, "%d %d %d", &numPisos, &numFilas, &numLugares);
    fclose(f);

    if (resultado != 3) { //O resultado deve ler 3 valores. (numPisos, numFilas e numLugares). Caso contrario retornar 0 (falha)
        return 0;
    }
//Depois, validar se os números colocados são validos, por isso retornar 0 se forem menores ou iguais a 0, ou maiores que os maximos que definimos no topo.
    if (numPisos <= 0 || numPisos > MAX_PISOS) return 0;
    if (numFilas <= 0 || numFilas > MAX_FILAS) return 0;
    if (numLugares <= 0 || numLugares > MAX_LUGARES) return 0;
//Agora que as variaveis temporarias foram validadas, passamos as mesmas para as variaveis da struct
    config->numpisos = numPisos;
    config->numfilas = numFilas;
    config->numlugares = numLugares;

    return 1;
}

int gravarconfig(const Confparque *config) {
    if (config == NULL) {
        return 0;
    }

    // Validar variaveis primeiro
    if (config->numpisos <= 0 || config->numpisos > MAX_PISOS) return 0;
    if (config->numfilas <= 0 || config->numfilas > MAX_FILAS) return 0;
    if (config->numlugares <= 0 || config->numlugares > MAX_LUGARES) return 0;

    FILE *f = fopen(Ficheiro_config, "w");
    if (f == NULL) {
        return 0;
    }
    fprintf(f, "%d %d %d\n", config->numpisos, config->numfilas, config->numlugares);
    
    fprintf(f, "# Configuracao do Parque de Estacionamento\n");
    fprintf(f, "# Pisos: %d\n", config->numpisos);
    fprintf(f, "# Filas: %d (A-%c)\n", config->numfilas, 'A' + config->numfilas - 1);
    fprintf(f, "# Lugares por fila: %d\n", config->numlugares);
       
       fclose(f);
       
       printf("\nConfiguração guardada com sucesso!\n");
       return 1;
 
}


int personalizapp(Confparque config) {
    printf("É NECESSÁRIO CONFIGURAR A APLICAÇÃO\n");
    printf("===CONFIGURAÇÃO DO PARQUE===\n\n");
    do {
        printf("Qual o nome da empresa de estacionamentos: ");
        scanf("%s", config.Nomempresa);
        if(strlen(config.Nomempresa)==0){
            printf("O nome da empresa é inválido.\n");
        }
    } while(strlen(config.Nomempresa)==0);
    do {
        printf("Qual é o seu nome: ");
        scanf("%s", config.Nominstal);
        if(strlen(config.Nominstal)==0) {
            printf("nome inválido.\n");
        }
    } while(strlen(config.Nominstal)==0);
    
    printf("CONFIGURAR TAMANHO DO PARQUE:\n");
    do {
        printf("Pisos(Deve ser um valor entre 1 e 5): ");
        scanf("%d", &config.numpisos);
        if (config.numpisos > MAX_PISOS || config.numpisos <= 0){
            printf("O Parque deve ter entre 1 e 5 pisos.\n");
        }
    } while (config.numpisos > MAX_PISOS || config.numpisos <= 0);
    do {
        printf("Filas(Deve ser um valor entre 1 e 26): ");
        scanf("%d", &config.numfilas);
        if (config.numfilas > MAX_FILAS || config.numfilas <= 0) {
            printf("O Parque deve ter entre 1 e 26 filas por piso.\n");
        }
    } while (config.numfilas > MAX_FILAS || config.numfilas <= 0);
    do {
        printf("Lugares(Deve ser um valor entre 0 e 50): ");
        scanf("%d", &config.numlugares);
        if (config.numlugares > MAX_LUGARES || config.numlugares <= 0) {
            printf("O parque deve ter entre 1 e 50 lugares por fila.\n");
        }
    } while(config.numlugares > MAX_LUGARES || config.numlugares <= 0);
    
    if (gravarconfig(&config)) {
    } else {
        printf("Erro ao gravar configuração!\n");
        return 0;
    }
    LimpaFicheiroEstac("estacionamentos.txt", "estacionamentos_validos.txt", "relatorio_erros.txt", config);
    mostrarMensagem("Será apresentado o Registo de estacionamentos.");
    listarTodosEstacionamentos("estacionamentos_validos.txt");
    FILE *F1 = fopen("configfeita.txt", "w"); {
        int resultado = config.numpisos*config.numfilas*config.numlugares;
        fprintf(F1, "Empresa: %s\tUsuário: %s\tTamanho do parque %d Pisos * %d Filas * %d Lugares\tQuantidade de lugares totais: %d", config.Nomempresa, config.Nominstal, config.numpisos, config.numfilas, config.numlugares, resultado);
        fclose(F1);
    }
    limparTela();
    mostrarMensagem("A abrir o menu principal...");
    mostrarMenu();
    
        
        return 1;
}
