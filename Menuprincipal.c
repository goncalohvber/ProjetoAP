//
//  Menuprincipal.c
//  ProjetoProgramacao
//
//  Created by Gonçalo Henrique Viegas Bernardino on 02/01/2026.
//

#include <stdio.h>
#include "Util.h"
#include "Estacionamento.h"
#include "GestaoLugares.h"
#include "Listagens.h"

void mostrarMenu(void) {
    int op1;
    int diaU, mesU, anoU, horaU, minU;
    Confparque config;
    if (!lerconfig(&config)) {
        printf("Erro ao carregar configuração!\n");
        return; }
        printf("\n");
        printf("\t========================================\n");
        printf("\t||           GESTÃO DE PARQUE         ||\n");
        printf("\t========================================\n");
        printf("\t||                                    ||\n");
        printf("\t||   [1] Registar Entrada             ||\n");
        printf("\t||   [2] Registar Saída               ||\n");
        printf("\t||   [3] Mostrar Mapa                 ||\n");
        printf("\t||   [4] Reconfigurar Parque          ||\n");
        printf("\t||   [5] Gestão de Lugares            ||\n");
        printf("\t||   [6] Listagens                    ||\n");
        printf("\t||                                    ||\n");
        printf("\t||   ------------------------------   ||\n");
        printf("\t||   [0] Sair                         ||\n");
        printf("\t||                                    ||\n");
        printf("\t========================================\n");
        printf("\n\t>> Escolha uma opção: ");
        scanf("%d", &op1);
        
        switch (op1) {
                
            case 1:
                if (registarEntrada(config, "estacionamentos.txt")) {
                    printf("\n Operação concluída!\n");
                } else {
                    printf("\nFalha ao registar entrada!\n");
                }
                mostrarMensagem("Prima Enter para continuar...");
                mostrarMenu();
                break;
                
            case 2:
                if (registarSaida(config, "estacionamentos.txt")) {
                        printf("\n✅ Operação concluída!\n");
                                atualizarValidacao(config);
                                } else {
                                printf("\n❌ Falha ao registar saída!\n");
                                }
                            mostrarMensagem("Prima Enter para continuar...");
                               mostrarMenu();
                            break;
                
            case 3:
                printf("Qual a data que quer ver? (DD MM AAAA): ");
                scanf("%d %d %d", &diaU, &mesU, &anoU);
                printf("Qual a hora? (HH MM): ");
                scanf("%d %d", &horaU, &minU);
                
                gerarficheiroocupacao("estacionamentos_validos.txt", "Ocupacaoatual.txt",
                                     diaU, mesU, anoU, horaU, minU);
                
                Lugar mapa[MAX_PISOS][MAX_FILAS][MAX_LUGARES];
                MostrarMapaOcupacao_ComMapa(config, "Ocupacaoatual.txt", mapa);
                
                mostrarMensagem("\nA voltar para o meu principal...");
                limparTela();
                mostrarMenu();
                break;
                
            case 4:
                remove("configfeita.txt");
                Confparque config;
                system("clear");
                personalizapp(config);
                break;

           case 5:
                menuGestaoLugares(config);
                break;
          
          case 6:
                menuListagens(config);
                break;

        }
        
    }
