//
//  Listagens.c
//  ProjetoEstacionamento
//
//  Sistema completo de listagens paginadas
//

#include "Listagens.h"
#include "Tarifas.h"
#include "validacoes.h"
#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

ControlePaginacao inicializarPaginacao(int totalRegistos, int registosPorPag) {
    ControlePaginacao ctrl;

    ctrl.totalRegistos = totalRegistos;
    ctrl.registosPorPag = registosPorPag;
    ctrl.paginaAtual = 1;

    ctrl.totalPaginas = (totalRegistos + registosPorPag - 1) / registosPorPag;

    if (ctrl.totalPaginas == 0) {
        ctrl.totalPaginas = 1;
    }

    return ctrl;
}

void mostrarBarraNavegacao(ControlePaginacao ctrl) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════╗\n");
    printf("║  Página %d de %d | Mostrando %d registos de %d         ║\n",
           ctrl.paginaAtual, ctrl.totalPaginas,
           (ctrl.paginaAtual < ctrl.totalPaginas) ? ctrl.registosPorPag :
           (ctrl.totalRegistos - (ctrl.paginaAtual - 1) * ctrl.registosPorPag),
           ctrl.totalRegistos);
    printf("╠═══════════════════════════════════════════════════════════╣\n");
    printf("║  [N] Próxima página  [P] Página anterior                  ║\n");
    printf("║  [G] Gravar em TXT   [C] Gravar em CSV                    ║\n");
    printf("║  [0] Voltar                                               ║\n");
    printf("╚═══════════════════════════════════════════════════════════╝\n");
}

void gravarListagemTXT(estacionamento est[], int numRegistos, char *nomeFicheiro) {
    if (numRegistos == 0) {
        printf("\n❌ Não há registos para gravar!\n");
        return;
    }

    FILE *f = fopen(nomeFicheiro, "w");
    if (f == NULL) {
        printf("\n❌ Erro ao criar ficheiro!\n");
        return;
    }

    // Cabeçalho
    fprintf(f, "===============================================================================================================\n");
    fprintf(f, "                                    LISTAGEM DE ESTACIONAMENTOS                                              \n");
    fprintf(f, "===============================================================================================================\n\n");
    fprintf(f, "%-6s | %-10s | %-5s | %-16s | %-16s | %-8s\n",
            "Nº", "Matrícula", "Lugar", "Entrada", "Saída", "Valor");
    fprintf(f, "-------+------------+-------+------------------+------------------+----------\n");

    // Dados
    for (int i = 0; i < numRegistos; i++) {
        if (est[i].anoS == 0) {
            // Ainda no parque
            fprintf(f, "%-6d | %-10s | %-5s | %02d/%02d/%d %02d:%02d | NO PARQUE        | -\n",
                    est[i].numE, est[i].matricula, est[i].lugar,
                    est[i].diaE, est[i].mesE, est[i].anoE,
                    est[i].horaE, est[i].minE);
        } else {
            // Já saiu
            fprintf(f, "%-6d | %-10s | %-5s | %02d/%02d/%d %02d:%02d | %02d/%02d/%d %02d:%02d | %.2f €\n",
                    est[i].numE, est[i].matricula, est[i].lugar,
                    est[i].diaE, est[i].mesE, est[i].anoE,
                    est[i].horaE, est[i].minE,
                    est[i].diaS, est[i].mesS, est[i].anoS,
                    est[i].horaS, est[i].minS,
                    est[i].valorPago);
        }
    }

    fprintf(f, "\n===============================================================================================================\n");
    fprintf(f, "Total de registos: %d\n", numRegistos);
    fprintf(f, "===============================================================================================================\n");

    fclose(f);
    printf("\n Listagem gravada em '%s'\n", nomeFicheiro);
}


void gravarListagemCSV(estacionamento est[], int numRegistos, char *nomeFicheiro) {
    if (numRegistos == 0) {
        printf("\n Não há registos para gravar!\n");
        return;
    }

    char separador;
    printf("\nEscolha o separador (, ou ;): ");
    scanf(" %c", &separador);

    if (separador != ',' && separador != ';') {
        separador = ';';
        printf("  Usando ; como separador padrão\n");
    }

    FILE *f = fopen(nomeFicheiro, "w");
    if (f == NULL) {
        printf("\n❌ Erro ao criar ficheiro!\n");
        return;
    }

    // Cabeçalho
    fprintf(f, "NumEntrada%cMatricula%cLugar%cAnoE%cMesE%cDiaE%cHoraE%cMinE%c",
            separador, separador, separador, separador, separador,
            separador, separador, separador);
    fprintf(f, "AnoS%cMesS%cDiaS%cHoraS%cMinS%cValorPago\n",
            separador, separador, separador, separador, separador);

    // Dados
    for (int i = 0; i < numRegistos; i++) {
        fprintf(f, "%d%c%s%c%s%c%d%c%d%c%d%c%d%c%d%c",
                est[i].numE, separador,
                est[i].matricula, separador,
                est[i].lugar, separador,
                est[i].anoE, separador,
                est[i].mesE, separador,
                est[i].diaE, separador,
                est[i].horaE, separador,
                est[i].minE, separador);

        fprintf(f, "%d%c%d%c%d%c%d%c%d%c%.2f\n",
                est[i].anoS, separador,
                est[i].mesS, separador,
                est[i].diaS, separador,
                est[i].horaS, separador,
                est[i].minS, separador,
                est[i].valorPago);
    }

    fclose(f);
    printf("\n Listagem CSV gravada em '%s'\n", nomeFicheiro);
}

void listarTodosEstacionamentos(char *ficheiroEstacionamentos) {
    // Carregar todos os estacionamentos
    estacionamento est[MAX_REG_EST];
    int numTotal = 0;

    FILE *f = fopen(ficheiroEstacionamentos, "r");
    if (f == NULL) {
        printf("\n❌ Erro ao abrir ficheiro!\n");
        return;
    }

    // Ler todos os registos
    while (fscanf(f, "%d %s %d %d %d %d %d %s %d %d %d %d %d %f",
                  &est[numTotal].numE, est[numTotal].matricula,
                  &est[numTotal].anoE, &est[numTotal].mesE, &est[numTotal].diaE,
                  &est[numTotal].horaE, &est[numTotal].minE,
                  est[numTotal].lugar,
                  &est[numTotal].anoS, &est[numTotal].mesS, &est[numTotal].diaS,
                  &est[numTotal].horaS, &est[numTotal].minS, &est[numTotal].valorPago) == 14) {

        // Calcular valor pago se já saiu
        if (est[numTotal].anoS != 0) {
            Tarifa tarifas[MAX_TARIFAS];
            int numTarifas = 0;
            lertarifas(tarifas, &numTarifas);

            est[numTotal].valorPago = CalcularPreco(
                est[numTotal].diaE, est[numTotal].mesE, est[numTotal].anoE,
                est[numTotal].horaE, est[numTotal].minE,
                est[numTotal].diaS, est[numTotal].mesS, est[numTotal].anoS,
                est[numTotal].horaS, est[numTotal].minS,
                tarifas, numTarifas);
        } else {
            est[numTotal].valorPago = 0.0;
        }

        numTotal++;
        if (numTotal >= MAX_REG_EST) break;
    }
    fclose(f);

    if (numTotal == 0) {
        printf("\n  Não há estacionamentos registados!\n");
        return;
    }

    // Inicializar paginação (15 registos por página)
    ControlePaginacao ctrl = inicializarPaginacao(numTotal, 15);

    char opcao;
    do {
        system("cls");
        printf("\n");
        printf("╔═══════════════════════════════════════════════════════════╗\n");
        printf("║              LISTAGEM DE TODOS OS ESTACIONAMENTOS         ║\n");
        printf("╚═══════════════════════════════════════════════════════════╝\n\n");

        // Calcular índices da página atual
        int inicio = (ctrl.paginaAtual - 1) * ctrl.registosPorPag;
        int fim = inicio + ctrl.registosPorPag;
        if (fim > numTotal) fim = numTotal;

        // Cabeçalho da tabela
        printf("%-6s | %-10s | %-5s | %-16s | %-16s | %-8s\n",
               "Nº", "Matrícula", "Lugar", "Entrada", "Saída", "Valor");
        printf("-------+------------+-------+------------------+------------------+----------\n");

        // Mostrar registos da página atual
        for (int i = inicio; i < fim; i++) {
            if (est[i].anoS == 0) {
                // Ainda no parque
                printf("%-6d | %-10s | %-5s | %02d/%02d/%d %02d:%02d | \033[1;33mNO PARQUE\033[0m        | -\n",
                       est[i].numE, est[i].matricula, est[i].lugar,
                       est[i].diaE, est[i].mesE, est[i].anoE,
                       est[i].horaE, est[i].minE);
            } else {
                // Já saiu
                printf("%-6d | %-10s | %-5s | %02d/%02d/%d %02d:%02d | %02d/%02d/%d %02d:%02d | %.2f €\n",
                       est[i].numE, est[i].matricula, est[i].lugar,
                       est[i].diaE, est[i].mesE, est[i].anoE,
                       est[i].horaE, est[i].minE,
                       est[i].diaS, est[i].mesS, est[i].anoS,
                       est[i].horaS, est[i].minS,
                       est[i].valorPago);
            }
        }

        // Barra de navegação
        mostrarBarraNavegacao(ctrl);
        printf("\nOpção: ");
        scanf(" %c", &opcao);

        switch (opcao) {
            case 'N':
            case 'n':
                if (ctrl.paginaAtual < ctrl.totalPaginas) {
                    ctrl.paginaAtual++;
                } else {
                    printf("\n  Já está na última página!\n");
                    printf("Pressione ENTER para continuar...");
                    getchar();
                    getchar();
                }
                break;

            case 'P':
            case 'p':
                if (ctrl.paginaAtual > 1) {
                    ctrl.paginaAtual--;
                } else {
                    printf("\n  Já está na primeira página!\n");
                    printf("Pressione ENTER para continuar...");
                    getchar();
                    getchar();
                }
                break;

            case 'G':
            case 'g':
                gravarListagemTXT(est, numTotal, "listagem_completa.txt");
                printf("Pressione ENTER para continuar...");
                getchar();
                getchar();
                break;

            case 'C':
            case 'c':
                gravarListagemCSV(est, numTotal, "listagem_completa.csv");
                printf("Pressione ENTER para continuar...");
                getchar();
                getchar();
                break;

            case '0':
                break;

            default:
                printf("\n❌ Opção inválida!\n");
                printf("Pressione ENTER para continuar...");
                getchar();
                getchar();
        }

    } while (opcao != '0');
}

void listarPorData(char *ficheiroEstacionamentos) {
    int dia, mes, ano;

    printf("\n╔═══════════════════════════════════════════════════════════╗\n");
    printf("║                LISTAR ESTACIONAMENTOS POR DATA              ║\n");
    printf("╚═══════════════════════════════════════════════════════════╝\n\n");

    do {
        printf("Data (DD MM AAAA): ");
        scanf("%d %d %d", &dia, &mes, &ano);
        if (!validaData(dia, mes, ano)) {
            printf(" Data inválida!\n");
        }
    } while (!validaData(dia, mes, ano));

    // Carregar e filtrar
    estacionamento est[MAX_REG_EST];
    estacionamento filtrados[MAX_REG_EST];
    int numTotal = 0, numFiltrados = 0;

    FILE *f = fopen(ficheiroEstacionamentos, "r");
    if (f == NULL) {
        printf("\n Erro ao abrir ficheiro!\n");
        return;
    }

    while (fscanf(f, "%d %s %d %d %d %d %d %s %d %d %d %d %d",
                  &est[numTotal].numE, est[numTotal].matricula,
                  &est[numTotal].anoE, &est[numTotal].mesE, &est[numTotal].diaE,
                  &est[numTotal].horaE, &est[numTotal].minE,
                  est[numTotal].lugar,
                  &est[numTotal].anoS, &est[numTotal].mesS, &est[numTotal].diaS,
                  &est[numTotal].horaS, &est[numTotal].minS) == 13) {

        // Filtrar por data de ENTRADA
        if (est[numTotal].anoE == ano &&
            est[numTotal].mesE == mes &&
            est[numTotal].diaE == dia) {

            // Calcular valor
            if (est[numTotal].anoS != 0) {
                Tarifa tarifas[MAX_TARIFAS];
                int numTarifas = 0;
                lertarifas(tarifas, &numTarifas);

                est[numTotal].valorPago = CalcularPreco(
                    est[numTotal].diaE, est[numTotal].mesE, est[numTotal].anoE,
                    est[numTotal].horaE, est[numTotal].minE,
                    est[numTotal].diaS, est[numTotal].mesS, est[numTotal].anoS,
                    est[numTotal].horaS, est[numTotal].minS,
                    tarifas, numTarifas);
            } else {
                est[numTotal].valorPago = 0.0;
            }

            filtrados[numFiltrados] = est[numTotal];
            numFiltrados++;
        }

        numTotal++;
        if (numTotal >= MAX_REG_EST) break;
    }
    fclose(f);

    if (numFiltrados == 0) {
        printf("\n  Não há estacionamentos para essa data!\n");
        printf("Pressione ENTER para continuar...");
        getchar();
        getchar();
        return;
    }

    // Mostrar com paginação
    ControlePaginacao ctrl = inicializarPaginacao(numFiltrados, 15);

    char opcao;
    do {
        system("cls");
        printf("\n");
        printf("╔═══════════════════════════════════════════════════════════╗\n");
        printf("║        ESTACIONAMENTOS EM %02d/%02d/%d                    ║\n", dia, mes, ano);
        printf("╚═══════════════════════════════════════════════════════════╝\n\n");

        int inicio = (ctrl.paginaAtual - 1) * ctrl.registosPorPag;
        int fim = inicio + ctrl.registosPorPag;
        if (fim > numFiltrados) fim = numFiltrados;

        printf("%-6s | %-10s | %-5s | %-8s | %-16s | %-8s\n",
               "Nº", "Matrícula", "Lugar", "Hora Ent", "Saída", "Valor");
        printf("-------+------------+-------+----------+------------------+----------\n");

        for (int i = inicio; i < fim; i++) {
            if (filtrados[i].anoS == 0) {
                printf("%-6d | %-10s | %-5s | %02d:%02d    | \033[1;33mNO PARQUE\033[0m        | -\n",
                       filtrados[i].numE, filtrados[i].matricula, filtrados[i].lugar,
                       filtrados[i].horaE, filtrados[i].minE);
            } else {
                printf("%-6d | %-10s | %-5s | %02d:%02d    | %02d/%02d/%d %02d:%02d | %.2f €\n",
                       filtrados[i].numE, filtrados[i].matricula, filtrados[i].lugar,
                       filtrados[i].horaE, filtrados[i].minE,
                       filtrados[i].diaS, filtrados[i].mesS, filtrados[i].anoS,
                       filtrados[i].horaS, filtrados[i].minS,
                       filtrados[i].valorPago);
            }
        }

        mostrarBarraNavegacao(ctrl);
        printf("\nOpção: ");
        scanf(" %c", &opcao);

        switch (opcao) {
            case 'N':
            case 'n':
                if (ctrl.paginaAtual < ctrl.totalPaginas) ctrl.paginaAtual++;
                break;
            case 'P':
            case 'p':
                if (ctrl.paginaAtual > 1) ctrl.paginaAtual--;
                break;
            case 'G':
            case 'g': {
                char nomeArq[100];
                sprintf(nomeArq, "listagem_%02d_%02d_%d.txt", dia, mes, ano);
                gravarListagemTXT(filtrados, numFiltrados, nomeArq);
                printf("Pressione ENTER...");
                getchar();
                getchar();
                break;
            }
            case 'C':
            case 'c': {
                char nomeArq[100];
                sprintf(nomeArq, "listagem_%02d_%02d_%d.csv", dia, mes, ano);
                gravarListagemCSV(filtrados, numFiltrados, nomeArq);
                printf("Pressione ENTER...");
                getchar();
                getchar();
                break;
            }
        }

    } while (opcao != '0');
}

// ============================================================
// 3. LISTAR POR MATRÍCULA (FUNCIONALIDADE EXTRA 2)
// ============================================================
void listarPorMatricula(char *ficheiroEstacionamentos) {
    char matriculaProcurada[10];

    printf("\n╔═══════════════════════════════════════════════════════════╗\n");
    printf("║       🚗 LISTAR ESTACIONAMENTOS POR MATRÍCULA            ║\n");
    printf("╚═══════════════════════════════════════════════════════════╝\n\n");

    do {
        printf("Matrícula (XX-XX-XX): ");
        scanf("%s", matriculaProcurada);
        if (!validamatricula(matriculaProcurada)) {
            printf("❌ Matrícula inválida!\n");
        }
    } while (!validamatricula(matriculaProcurada));

    // Carregar e filtrar
    estacionamento filtrados[MAX_REG_EST];
    int numFiltrados = 0;

    FILE *f = fopen(ficheiroEstacionamentos, "r");
    if (f == NULL) {
        printf("\n❌ Erro ao abrir ficheiro!\n");
        return;
    }

    estacionamento temp;
    while (fscanf(f, "%d %s %d %d %d %d %d %s %d %d %d %d %d",
                  &temp.numE, temp.matricula,
                  &temp.anoE, &temp.mesE, &temp.diaE,
                  &temp.horaE, &temp.minE,
                  temp.lugar,
                  &temp.anoS, &temp.mesS, &temp.diaS,
                  &temp.horaS, &temp.minS) == 13) {

        if (strcmp(temp.matricula, matriculaProcurada) == 0) {
            if (temp.anoS != 0) {
                Tarifa tarifas[MAX_TARIFAS];
                int numTarifas = 0;
                lertarifas(tarifas, &numTarifas);

                temp.valorPago = CalcularPreco(
                    temp.diaE, temp.mesE, temp.anoE, temp.horaE, temp.minE,
                    temp.diaS, temp.mesS, temp.anoS, temp.horaS, temp.minS,
                    tarifas, numTarifas);
            } else {
                temp.valorPago = 0.0;
            }

            filtrados[numFiltrados] = temp;
            numFiltrados++;
        }
    }
    fclose(f);

    if (numFiltrados == 0) {
        printf("\n⚠️  Não há estacionamentos para essa matrícula!\n");
        printf("Pressione ENTER para continuar...");
        getchar();
        getchar();
        return;
    }

    // Mostrar resultados
    system("cls");
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════╗\n");
    printf("║      🚗 HISTÓRICO DA MATRÍCULA: %s                  ║\n", matriculaProcurada);
    printf("╚═══════════════════════════════════════════════════════════╝\n\n");

    printf("Total de estacionamentos: %d\n\n", numFiltrados);

    printf("%-6s | %-5s | %-16s | %-16s | %-8s\n",
           "Nº", "Lugar", "Entrada", "Saída", "Valor");
    printf("-------+-------+------------------+------------------+----------\n");

    float totalPago = 0.0;
    for (int i = 0; i < numFiltrados; i++) {
        if (filtrados[i].anoS == 0) {
            printf("%-6d | %-5s | %02d/%02d/%d %02d:%02d | \033[1;33mNO PARQUE\033[0m        | -\n",
                   filtrados[i].numE, filtrados[i].lugar,
                   filtrados[i].diaE, filtrados[i].mesE, filtrados[i].anoE,
                   filtrados[i].horaE, filtrados[i].minE);
        } else {
            printf("%-6d | %-5s | %02d/%02d/%d %02d:%02d | %02d/%02d/%d %02d:%02d | %.2f €\n",
                   filtrados[i].numE, filtrados[i].lugar,
                   filtrados[i].diaE, filtrados[i].mesE, filtrados[i].anoE,
                   filtrados[i].horaE, filtrados[i].minE,
                   filtrados[i].diaS, filtrados[i].mesS, filtrados[i].anoS,
                   filtrados[i].horaS, filtrados[i].minS,
                   filtrados[i].valorPago);
            totalPago += filtrados[i].valorPago;
        }
    }

    printf("\n💰 Total pago: %.2f €\n", totalPago);

    printf("\n[G] Gravar em TXT | [C] Gravar em CSV | [0] Voltar: ");
    char op;
    scanf(" %c", &op);

    if (op == 'G' || op == 'g') {
        char nomeArq[100];
        sprintf(nomeArq, "historico_%s.txt", matriculaProcurada);
        gravarListagemTXT(filtrados, numFiltrados, nomeArq);
    } else if (op == 'C' || op == 'c') {
        char nomeArq[100];
        sprintf(nomeArq, "historico_%s.csv", matriculaProcurada);
        gravarListagemCSV(filtrados, numFiltrados, nomeArq);
    }

    printf("Pressione ENTER para continuar...");
    getchar();
    getchar();
}

void listarVeiculosNoParque(char *ficheiroEstacionamentos) {
    estacionamento filtrados[MAX_REG_EST];
    int numFiltrados = 0;

    FILE *f = fopen(ficheiroEstacionamentos, "r");
    if (f == NULL) {
        printf("\n❌ Erro ao abrir ficheiro!\n");
        return;
    }

    estacionamento temp;
    while (fscanf(f, "%d %s %d %d %d %d %d %s %d %d %d %d %d",
                  &temp.numE, temp.matricula,
                  &temp.anoE, &temp.mesE, &temp.diaE,
                  &temp.horaE, &temp.minE,
                  temp.lugar,
                  &temp.anoS, &temp.mesS, &temp.diaS,
                  &temp.horaS, &temp.minS) == 13) {

        // Apenas veículos que ainda não saíram
        if (temp.anoS == 0) {
            filtrados[numFiltrados] = temp;
            numFiltrados++;
        }
    }
    fclose(f);

    if (numFiltrados == 0) {
        printf("\n  Não há veículos no parque!\n");
        printf("Pressione ENTER para continuar...");
        getchar();
        getchar();
        return;
    }

    // Paginação
    ControlePaginacao ctrl = inicializarPaginacao(numFiltrados, 15);

    char opcao;
    do {
        system("clear");
        printf("\n");
        printf("╔═══════════════════════════════════════════════════════════╗\n");
        printf("║               VEÍCULOS ATUALMENTE NO PARQUE               ║\n");
        printf("╚═══════════════════════════════════════════════════════════╝\n\n");

        int inicio = (ctrl.paginaAtual - 1) * ctrl.registosPorPag;
        int fim = inicio + ctrl.registosPorPag;
        if (fim > numFiltrados) fim = numFiltrados;

        printf("%-6s | %-10s | %-5s | %-16s\n",
               "Nº", "Matrícula", "Lugar", "Entrada");
        printf("-------+------------+-------+------------------\n");

        for (int i = inicio; i < fim; i++) {
            printf("%-6d | %-10s | %-5s | %02d/%02d/%d %02d:%02d\n",
                   filtrados[i].numE, filtrados[i].matricula, filtrados[i].lugar,
                   filtrados[i].diaE, filtrados[i].mesE, filtrados[i].anoE,
                   filtrados[i].horaE, filtrados[i].minE);
        }

        mostrarBarraNavegacao(ctrl);
        printf("\nOpção: ");
        scanf(" %c", &opcao);

        switch (opcao) {
            case 'N':
            case 'n':
                if (ctrl.paginaAtual < ctrl.totalPaginas) ctrl.paginaAtual++;
                break;
            case 'P':
            case 'p':
                if (ctrl.paginaAtual > 1) ctrl.paginaAtual--;
                break;
            case 'G':
            case 'g':
                gravarListagemTXT(filtrados, numFiltrados, "veiculos_no_parque.txt");
                printf("Pressione ENTER...");
                getchar();
                getchar();
                break;
            case 'C':
            case 'c':
                gravarListagemCSV(filtrados, numFiltrados, "veiculos_no_parque.csv");
                printf("Pressione ENTER...");
                getchar();
                getchar();
                break;
        }

    } while (opcao != '0');
}

void menuListagens(Confparque config) {
    int opcao;

    do {
        system("cls");
        printf("\n");
        printf("╔═══════════════════════════════════════════════════════════╗\n");
        printf("║               MENU DE LISTAGENS                           ║\n");
        printf("╠═══════════════════════════════════════════════════════════╣\n");
        printf("║                                                           ║\n");
        printf("║  1.   Listar TODOS os estacionamentos                     ║\n");
        printf("║  2.   Listar por DATA                                     ║\n");
        printf("║  3.   Listar por MATRÍCULA                                ║\n");
        printf("║  4.   Listar veículos NO PARQUE                           ║\n");
        printf("║                                                           ║\n");
        printf("║  0.   Voltar ao menu principal                            ║\n");
        printf("║                                                           ║\n");
        printf("╚═══════════════════════════════════════════════════════════╝\n");
        printf("\nEscolha uma opção: ");
        scanf("%d", &opcao);

        switch (opcao) {
            case 1:
                listarTodosEstacionamentos("estacionamentos_validos.txt");
                break;

            case 2:
                listarPorData("estacionamentos_validos.txt");
                break;

            case 3:
                listarPorMatricula("estacionamentos_validos.txt");
                break;

            case 4:
                listarVeiculosNoParque("estacionamentos_validos.txt");
                break;

            case 0:
                
                mostrarMenu();
                break;

            default:
                printf("\n Opção inválida!\n");
                printf("Pressione ENTER para continuar...");
                getchar();
                getchar();
        }

    } while (opcao != 0);
}
