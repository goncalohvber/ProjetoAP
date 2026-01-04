#include <stdio.h>
#include "Util.h"
#include "Estacionamento.h"
#include "GestaoLugares.h"
#include "Listagens.h"

//Esta função é chamada diversas vezes ao longo do codigo, o seu objetivo é mostrar o menu principal, e depois, nos cases, chamar as funcoes de cada uma das opcoes.
void mostrarMenu(void) {
    int op1;
    int diaU, mesU, anoU, horaU, minU;
    Confparque config;
    
    if (!lerconfig(&config)) {
        printf("Erro ao carregar configuração!\n");
        return;
    }
    
    // Obter estatísticas do parque
    EstatisticasParque stats = obterEstatisticasParque(config);
    
    // Calcular percentagem de ocupação
    float percentagemOcupacao = (stats.ocupados * 100.0) / stats.total;
    
    printf("\n");
    printf("\t========================================\n");
    printf("\t||           GESTÃO DE PARQUE         ||\n");
    printf("\t========================================\n");
    printf("\t||                                    ||\n");
    printf("\t||   [1] Registar Entrada             ||\n");
    printf("\t||   [2] Registar Saída               ||\n");
    printf("\t||   [3] Mostrar Mapa                 ||\n");
    printf("\t||   [4] Configurações                ||\n");
    printf("\t||   [5] Gestão de Lugares            ||\n");
    printf("\t||   [6] Listagens                    ||\n");
    printf("\t||                                    ||\n");
    printf("\t||   ------------------------------   ||\n");
    printf("\t||   [0] Sair                         ||\n");
    printf("\t||                                    ||\n");
    printf("\t========================================\n");
    printf("\t||                                    ||\n");
    
    printf("\t||      ESTATÍSTICAS DO PARQUE        ||\n");
    printf("\t||   ──────────────────────────────   ||\n");
    printf("\t||   Total de lugares: %-5d         ||\n", stats.total);
    printf("\t||      Disponíveis:    %-5d         ||\n", stats.disponiveis);
    printf("\t||      Ocupados:       %-5d         ||\n", stats.ocupados);
    printf("\t||      Indisponíveis:  %-5d         ||\n", stats.indisponiveis);
    printf("\t||   Taxa ocupação:    %.1f%%          ||\n", percentagemOcupacao);
    printf("\t||                                    ||\n");
    printf("\t========================================\n");
    
    printf("\n\t>> Escolha uma opção: ");
    scanf("%d", &op1);
    
    switch (op1) {
        case 1: {
            int opcao;
            printf("\n╔═══════════════════════════════════════╗\n");
            printf("║     MODO DE ENTRADA                   ║\n");
            printf("╠═══════════════════════════════════════╣\n");
            printf("║  1. Entrada automática (data atual)   ║\n");
            printf("║  2. Entrada manual (inserir data)     ║\n");
            printf("╚═══════════════════════════════════════╝\n");
            printf("Escolha: ");
            scanf("%d", &opcao);
            
            if (opcao == 1) {
                if (registarEntradaAutomatica(config, "estacionamentos.txt")) {
                    printf("\n   Operação concluída!\n");
                } else {
                    printf("\n   Falha ao registar entrada!\n");
                }
            } else if (opcao == 2) {
                if (registarEntrada(config, "estacionamentos.txt")) {
                    printf("\n   Operação concluída!\n");
                } else {
                    printf("\n   Falha ao registar entrada!\n");
                }
            }
            
            mostrarMensagem("Prima Enter para continuar...");
            mostrarMenu();
            break;
        }

        case 2: {
            int opcao;
            printf("\n╔═══════════════════════════════════════╗\n");
            printf("║     MODO DE SAÍDA                     ║\n");
            printf("╠═══════════════════════════════════════╣\n");
            printf("║  1. Saída automática (data atual)     ║\n");
            printf("║  2. Saída manual (inserir data)       ║\n");
            printf("╚═══════════════════════════════════════╝\n");
            printf("Escolha: ");
            scanf("%d", &opcao);
            
            if (opcao == 1) {
                if (registarSaidaAutomatica(config, "estacionamentos.txt")) {
                    printf("\n   Operação concluída!\n");
                    atualizarValidacao(config);
                } else {
                    printf("\n   Falha ao registar saída!\n");
                }
            } else if (opcao == 2) {
                if (registarSaida(config, "estacionamentos.txt")) {
                    printf("\n   Operação concluída!\n");
                    atualizarValidacao(config);
                } else {
                    printf("\n   Falha ao registar saída!\n");
                }
            }
    
            mostrarMensagem("Prima Enter para continuar...");
            mostrarMenu();
            break;
        }
            
        case 3: {
            int opcao;
            printf("\n╔═══════════════════════════════════════╗\n");
            printf("║     MODO DE VISUALIZAÇÃO              ║\n");
            printf("╠═══════════════════════════════════════╣\n");
            printf("║  1. Mapa atual (data do sistema)      ║\n");
            printf("║  2. Mapa personalizado (inserir data) ║\n");
            printf("╚═══════════════════════════════════════╝\n");
            printf("Escolha: ");
            scanf("%d", &opcao);
            
            int diaU, mesU, anoU, horaU, minU;
            
            if (opcao == 1) {
                obterDataHoraAtual(&diaU, &mesU, &anoU, &horaU, &minU);
                printf("\n📅 Mostrando mapa para: %02d/%02d/%d às %02d:%02d\n",
                       diaU, mesU, anoU, horaU, minU);
            } else if (opcao == 2) {
                printf("Qual a data que quer ver? (DD MM AAAA): ");
                scanf("%d %d %d", &diaU, &mesU, &anoU);
                printf("Qual a hora? (HH MM): ");
                scanf("%d %d", &horaU, &minU);
            } else {
                printf("   Opção inválida!\n");
                mostrarMensagem("Prima Enter para continuar...");
                mostrarMenu();
                break;
            }
            
            gerarficheiroocupacao("estacionamentos_validos.txt", "Ocupacaoatual.txt",
                                 diaU, mesU, anoU, horaU, minU);
            
            Lugar mapa[MAX_PISOS][MAX_FILAS][MAX_LUGARES];
            MostrarMapaOcupacao_Paginado(config, "Ocupacaoatual.txt", mapa);

            mostrarMensagem("\nPrima Enter para continuar...");
            mostrarMenu();
            break;
        }
            
        case 4:
            menuConfiguracoes(config);
            mostrarMenu();
            break;
            
       case 5:
            menuGestaoLugares(config);
            mostrarMenu();
            break;
      
      case 6:
            menuListagens(config);
            mostrarMenu();
            break;
            
       case 0:
            printf("\n   A encerrar o sistema...\n");
            exit(0);
            break;
            
       default:
            printf("\n   Opção inválida!\n");
            mostrarMensagem("Prima Enter para continuar...");
            mostrarMenu();
            break;
    }
}
