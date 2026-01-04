#include "Listagens.h"
#include "Tarifas.h"
#include "validacoes.h"
#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// função para iniciar a paginacao
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

//funcao que mostra ao utilizador as opcoes, juntamente com a pagina onde está, o total de paginas, a quantidade de registos/registos por pagina
// porteriormente, em outra funcao, temos os cases das opcoes
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
    printf("║  [0] Sair                                                 ║\n");
    printf("╚═══════════════════════════════════════════════════════════╝\n");
}

//guarda a listagem que o utilizador dejesa ver num novo ficheiro .txt
void gravarListagemTXT(estacionamento est[], int numRegistos, char *nomeFicheiro) {
    if (numRegistos == 0) {
        printf("\n Não há registos para gravar!\n");
        return;
    }

    FILE *f = fopen(nomeFicheiro, "w"); //abrir o novo ficheiro em formato de escrita
    if (f == NULL) {
        printf("\n Erro ao criar ficheiro!\n");
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
        if (est[i].anoS == 0) { //se o anoS (ano de saida) for 0
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

//guarda a listagem num ficheiro .csv
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
        printf("  Utilizando ; como separador padrão\n");
    }

    FILE *f = fopen(nomeFicheiro, "w");
    if (f == NULL) {
        printf("\n Erro ao criar ficheiro!\n");
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

    // Carregar todos os estacionamentos
void listarTodosEstacionamentos(char *ficheiroEstacionamentos) {

    estacionamento est[MAX_REG_EST];
    int numTotal = 0;

    FILE *f = fopen(ficheiroEstacionamentos, "r");
    if (f == NULL) {
        printf("\n❌ Erro ao abrir ficheiro!\n");
        return;
    }

    //lê todos os dados, se nao existirem exatamente 14 para
    while (fscanf(f, "%d %s %d %d %d %d %d %s %d %d %d %d %d %f",
                  &est[numTotal].numE, est[numTotal].matricula,
                  &est[numTotal].anoE, &est[numTotal].mesE, &est[numTotal].diaE,
                  &est[numTotal].horaE, &est[numTotal].minE,
                  est[numTotal].lugar,
                  &est[numTotal].anoS, &est[numTotal].mesS, &est[numTotal].diaS,
                  &est[numTotal].horaS, &est[numTotal].minS,
                  &est[numTotal].valorPago) == 14) {

        // Se já saiu mas não tem preço calculado, calcular o preço
        if (est[numTotal].anoS != 0 && est[numTotal].valorPago == 0.0) {
            Tarifa tarifas[MAX_TARIFAS];
            int numTarifas = 0;
            if (lertarifas(tarifas, &numTarifas)) {
                est[numTotal].valorPago = CalcularPreco(
                    est[numTotal].diaE, est[numTotal].mesE, est[numTotal].anoE,
                    est[numTotal].horaE, est[numTotal].minE,
                    est[numTotal].diaS, est[numTotal].mesS, est[numTotal].anoS,
                    est[numTotal].horaS, est[numTotal].minS,
                    tarifas, numTarifas);
            }
        }

        numTotal++;
        if (numTotal >= MAX_REG_EST) break;
    }
    fclose(f);

    
    if (numTotal == 0) {
        printf("\n  Não há estacionamentos registados!\n");
        printf("Pressione ENTER para continuar...");
        getchar();
        getchar();
        return;
    }

    printf("\n Total de registos carregados: %d\n", numTotal);

    // Inicializar paginação (15 registos por página)
    ControlePaginacao ctrl = inicializarPaginacao(numTotal, 15);

    char opcao;
    do {
        system("cls"); //Limpa ecra
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
//o swich para todas as opcoes demonstradas na barra de navegação (Todas as outras listagens sem um logica igual ou parecida para as suas opcoes)
        switch (opcao) {
            case 'N':
            case 'n': //ir para a pagina seguinte
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
            case 'p': // ir para a pagina anterior
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
            case 'g': //guardar dados em txt
                gravarListagemTXT(est, numTotal, "listagem_completa.txt");
                printf("Pressione ENTER para continuar...");
                getchar();
                getchar();
                break;

            case 'C':
            case 'c': //guardar em csv
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

//listagem por data
void listarPorData(char *ficheiroEstacionamentos) {
    int dia, mes, ano;

    printf("\n╔═══════════════════════════════════════════════════════════╗\n");
    printf("║                LISTAR ESTACIONAMENTOS POR DATA              ║\n");
    printf("╚═══════════════════════════════════════════════════════════╝\n\n");

    do {
        printf("Data (DD MM AAAA): ");
        int resultado = scanf("%d %d %d", &dia, &mes, &ano);
        
        if (resultado != 3) {
            printf(" Entrada inválida! Use números.\n");
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
            continue;
        }
        
        if (!validaData(dia, mes, ano)) {
            printf("❌ Data inválida!\n");
        }
    } while (!validaData(dia, mes, ano));

    // Carregar e filtrar
    estacionamento filtrados[MAX_REG_EST];
    int numFiltrados = 0;

    FILE *f = fopen(ficheiroEstacionamentos, "r");
    if (f == NULL) {
        printf("\n❌ Erro ao abrir ficheiro!\n");
        printf("Pressione ENTER para continuar...");
        getchar();
        getchar();
        return;
    }

    estacionamento temp;
    while (fscanf(f, "%d %s %d %d %d %d %d %s %d %d %d %d %d %f",
                  &temp.numE, temp.matricula,
                  &temp.anoE, &temp.mesE, &temp.diaE,
                  &temp.horaE, &temp.minE,
                  temp.lugar,
                  &temp.anoS, &temp.mesS, &temp.diaS,
                  &temp.horaS, &temp.minS,
                  &temp.valorPago) == 14) {

        // Filtrar por data de ENTRADA
        if (temp.anoE == ano && temp.mesE == mes && temp.diaE == dia) {

            // Calcular valor se necessário
            if (temp.anoS != 0 && temp.valorPago == 0.0) {
                Tarifa tarifas[MAX_TARIFAS];
                int numTarifas = 0;
                if (lertarifas(tarifas, &numTarifas)) {
                    temp.valorPago = CalcularPreco(
                        temp.diaE, temp.mesE, temp.anoE,
                        temp.horaE, temp.minE,
                        temp.diaS, temp.mesS, temp.anoS,
                        temp.horaS, temp.minS,
                        tarifas, numTarifas);
                }
            }

            filtrados[numFiltrados] = temp;
            numFiltrados++;
        }
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

void listarPorMatricula(char *ficheiroEstacionamentos) {
    char matriculaProcurada[10];

    printf("\n╔═══════════════════════════════════════════════════════════╗\n");
    printf("║         LISTAR ESTACIONAMENTOS POR MATRÍCULA            ║\n");
    printf("╚═══════════════════════════════════════════════════════════╝\n\n");

    //  CORREÇÃO: Limpar buffer antes de ler
    int c;
    while ((c = getchar()) != '\n' && c != EOF);

    do {
        printf("Matrícula (XX-XX-XX): ");
        fgets(matriculaProcurada, sizeof(matriculaProcurada), stdin);
        
        // Remover newline
        matriculaProcurada[strcspn(matriculaProcurada, "\n")] = 0;
        
        if (!validamatricula(matriculaProcurada)) {
            printf(" Matrícula inválida! Formato: XX-XX-XX\n\n");
        }
    } while (!validamatricula(matriculaProcurada));

    // Carregar e filtrar
    estacionamento filtrados[MAX_REG_EST];
    int numFiltrados = 0;

    FILE *f = fopen(ficheiroEstacionamentos, "r");
    if (f == NULL) {
        printf("\n Erro ao abrir ficheiro!\n");
        printf("Pressione ENTER para continuar...");
        getchar();
        return;
    }

    estacionamento temp;
   
    while (fscanf(f, "%d %s %d %d %d %d %d %s %d %d %d %d %d %f",
                  &temp.numE, temp.matricula,
                  &temp.anoE, &temp.mesE, &temp.diaE,
                  &temp.horaE, &temp.minE,
                  temp.lugar,
                  &temp.anoS, &temp.mesS, &temp.diaS,
                  &temp.horaS, &temp.minS,
                  &temp.valorPago) == 14) {

        if (strcmp(temp.matricula, matriculaProcurada) == 0) {
            if (temp.anoS != 0 && temp.valorPago == 0.0) {
                Tarifa tarifas[MAX_TARIFAS];
                int numTarifas = 0;
                if (lertarifas(tarifas, &numTarifas)) {
                    temp.valorPago = CalcularPreco(
                        temp.diaE, temp.mesE, temp.anoE, temp.horaE, temp.minE,
                        temp.diaS, temp.mesS, temp.anoS, temp.horaS, temp.minS,
                        tarifas, numTarifas);
                }
            }

            filtrados[numFiltrados] = temp;
            numFiltrados++;
        }
    }
    fclose(f);

    if (numFiltrados == 0) {
        printf("\n   Não há estacionamentos para essa matrícula!\n");
        printf("Pressione ENTER para continuar...");
        getchar();
        return;
    }

    // Mostrar resultados
    system("cls");
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════╗\n");
    printf("║       HISTÓRICO DA MATRÍCULA: %-10s             ║\n", matriculaProcurada);
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

    printf("\n Total pago: %.2f €\n", totalPago);

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
        printf("\n Erro ao abrir ficheiro!\n");
        printf("Pressione ENTER para continuar...");
        getchar();
        getchar();
        return;
    }

    estacionamento temp;
    while (fscanf(f, "%d %s %d %d %d %d %d %s %d %d %d %d %d %f",
                  &temp.numE, temp.matricula,
                  &temp.anoE, &temp.mesE, &temp.diaE,
                  &temp.horaE, &temp.minE,
                  temp.lugar,
                  &temp.anoS, &temp.mesS, &temp.diaS,
                  &temp.horaS, &temp.minS,
                  &temp.valorPago) == 14) {

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
        system("cls");
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
        printf("║  === EXTRAS ===                                           ║\n");
        printf("║  5.   E2 - Tabela Dinâmica (Saídas por Data)              ║\n");
        printf("║  6.   E3 - Exportar para CSV                              ║\n");
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
            
            case 5:
                gerarTabelaDinamica("estacionamentos_validos.txt");
                break;
            
            case 6:
                printf("E3 - Exportar CSV\n");
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


// Estrutura para armazenar dados da tabela dinâmica
typedef struct {
    int dia;
    int mes;
    int totalSaidas;
    float totalValor;
} DadosDiario;

// Função auxiliar: Comparar duas datas
int compararDatas(int d1, int m1, int a1, int d2, int m2, int a2) {
    if (a1 != a2) return a1 - a2;
    if (m1 != m2) return m1 - m2;
    return d1 - d2;
}

// Função auxiliar: Verificar se data está entre intervalo
int dataNoIntervalo(int dia, int mes, int ano,
                    int diaInicio, int mesInicio, int anoInicio,
                    int diaFim, int mesFim, int anoFim) {
    
    int resultado1 = compararDatas(dia, mes, ano, diaInicio, mesInicio, anoInicio);
    int resultado2 = compararDatas(dia, mes, ano, diaFim, mesFim, anoFim);
    
    // Data está entre início e fim (inclusive)
    return (resultado1 >= 0) && (resultado2 <= 0);
}

// cria a tabela 
void gerarTabelaDinamica(char *ficheiroEstacionamentos) {
    int diaInicio, mesInicio, anoInicio;
    int diaFim, mesFim, anoFim;
    
    printf("\n╔═══════════════════════════════════════════════════════════╗\n");
    printf("║            TABELA DINÂMICA - SAÍDAS POR DATA            ║\n");
    printf("╚═══════════════════════════════════════════════════════════╝\n\n");
    
    // Pedir data inicial
    do {
        printf("Data INICIAL (DD MM AAAA): ");
        scanf("%d %d %d", &diaInicio, &mesInicio, &anoInicio);
        if (!validaData(diaInicio, mesInicio, anoInicio)) {
            printf("  Data inválida! Tente novamente.\n\n");
        }
    } while (!validaData(diaInicio, mesInicio, anoInicio));
    
    // Pedir data final
    do {
        printf("Data FINAL (DD MM AAAA): ");
        scanf("%d %d %d", &diaFim, &mesFim, &anoFim);
        if (!validaData(diaFim, mesFim, anoFim)) {
            printf("  Data inválida! Tente novamente.\n\n");
        }
        
        // Validar se a data final é posterior à inicial
        if (compararDatas(diaFim, mesFim, anoFim, diaInicio, mesInicio, anoInicio) < 0) {
            printf("  Data final deve ser posterior à inicial!\n\n");
        }
    } while (!validaData(diaFim, mesFim, anoFim) ||
             compararDatas(diaFim, mesFim, anoFim, diaInicio, mesInicio, anoInicio) < 0);
    
    // Array para armazenar dados (máximo 365 dias)
    DadosDiario dados[365];
    int numDias = 0;
    
    // Ler ficheiro e processar
    FILE *f = fopen(ficheiroEstacionamentos, "r");
    if (f == NULL) {
        printf("\n  Erro ao abrir ficheiro!\n");
        return;
    }
    
    // Carregar tarifas
    Tarifa tarifas[MAX_TARIFAS];
    int numTarifas = 0;
    lertarifas(tarifas, &numTarifas);
    
    estacionamento E;
    float preco;
    
    // Ler todos os registos
    while (fscanf(f, "%d %s %d %d %d %d %d %s %d %d %d %d %d %f",
                  &E.numE, E.matricula,
                  &E.anoE, &E.mesE, &E.diaE, &E.horaE, &E.minE,
                  E.lugar,
                  &E.anoS, &E.mesS, &E.diaS, &E.horaS, &E.minS,
                  &preco) == 14) {
        
        // Só considerar registos com saída (anoS != 0)
        if (E.anoS != 0) {
            // Verificar se data de saída está no intervalo
            if (dataNoIntervalo(E.diaS, E.mesS, E.anoS,
                               diaInicio, mesInicio, anoInicio,
                               diaFim, mesFim, anoFim)) {
                
                // Procurar se o dia já existe na tabela
                int encontrado = 0;
                for (int i = 0; i < numDias; i++) {
                    if (dados[i].dia == E.diaS && dados[i].mes == E.mesS) {
                        dados[i].totalSaidas++;
                        dados[i].totalValor += preco;
                        encontrado = 1;
                        break;
                    }
                }
                
                // Se não encontrou, adicionar novo dia
                if (!encontrado && numDias < 365) {
                    dados[numDias].dia = E.diaS;
                    dados[numDias].mes = E.mesS;
                    dados[numDias].totalSaidas = 1;
                    dados[numDias].totalValor = preco;
                    numDias++;
                }
            }
        }
    }
    fclose(f);
    
    if (numDias == 0) {
        printf("\n   Não há dados para o intervalo especificado!\n");
        printf("Pressione ENTER para continuar...");
        getchar();
        getchar();
        return;
    }
    
    // Ordenar dados por data
    for (int i = 0; i < numDias - 1; i++) {
        for (int j = 0; j < numDias - i - 1; j++) {
            if (compararDatas(dados[j].dia, dados[j].mes, anoInicio,
                             dados[j+1].dia, dados[j+1].mes, anoInicio) > 0) {
                DadosDiario temp = dados[j];
                dados[j] = dados[j+1];
                dados[j+1] = temp;
            }
        }
    }
    
    // Exibir tabela dinâmica
    system("cls");
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════╗\n");
    printf("║          📊 TABELA DINÂMICA - SAÍDAS POR DATA            ║\n");
    printf("║  Período: %02d/%02d/%d a %02d/%02d/%d                      ║\n",
           diaInicio, mesInicio, anoInicio, diaFim, mesFim, anoFim);
    printf("╚═══════════════════════════════════════════════════════════╝\n\n");
    
    printf("┌──────────────┬───────────────┬──────────────────┐\n");
    printf("│     DATA     │   SAÍDAS      │   VALOR TOTAL    │\n");
    printf("├──────────────┼───────────────┼──────────────────┤\n");
    
    float totalGeralValor = 0.0;
    int totalGeralSaidas = 0;
    
    for (int i = 0; i < numDias; i++) {
        printf("│ %02d/%02d/%d   │      %4d      │     %.2f €       │\n",
               dados[i].dia, dados[i].mes, anoInicio,
               dados[i].totalSaidas,
               dados[i].totalValor);
        
        totalGeralSaidas += dados[i].totalSaidas;
        totalGeralValor += dados[i].totalValor;
    }
    
    printf("├──────────────┼───────────────┼──────────────────┤\n");
    printf("│   TOTAL      │      %4d      │     %.2f €       │\n",
           totalGeralSaidas, totalGeralValor);
    printf("└──────────────┴───────────────┴──────────────────┘\n");
    
    // Média
    float media = (numDias > 0) ? (totalGeralValor / numDias) : 0.0;
    printf("\n  Valor médio por dia: %.2f €\n", media);
    printf("  Total de dias com saídas: %d\n", numDias);
    printf("  Total de saídas: %d\n\n", totalGeralSaidas);
    
    // Opção para gravar em ficheiro
    char opcao;
    printf("╔═══════════════════════════════════════════════════════════╗\n");
    printf("║  [G] Gravar em TXT    [C] Gravar em CSV    [0] Voltar   ║\n");
    printf("╚═══════════════════════════════════════════════════════════╝\n");
    printf("\nOpção: ");
    scanf(" %c", &opcao);
    
    if (opcao == 'G' || opcao == 'g') {
        char nomeArq[100];
        printf("\nNome do ficheiro (sem extensão): ");
        scanf("%s", nomeArq);
        strcat(nomeArq, ".txt");
        
        FILE *fout = fopen(nomeArq, "w");
        if (fout == NULL) {
            printf(" Erro ao criar ficheiro!\n");
            return;
        }
        
        fprintf(fout, "TABELA DINÂMICA - SAÍDAS POR DATA\n");
        fprintf(fout, "Período: %02d/%02d/%d a %02d/%02d/%d\n\n",
                diaInicio, mesInicio, anoInicio, diaFim, mesFim, anoFim);
        fprintf(fout, "DATA         | SAÍDAS    | VALOR TOTAL\n");
        fprintf(fout, "─────────────┼───────────┼─────────────\n");
        
        for (int i = 0; i < numDias; i++) {
            fprintf(fout, "%02d/%02d/%d   |    %4d   |   %.2f €\n",
                   dados[i].dia, dados[i].mes, anoInicio,
                   dados[i].totalSaidas,
                   dados[i].totalValor);
        }
        
        fprintf(fout, "─────────────┼───────────┼─────────────\n");
        fprintf(fout, "TOTAL        |    %4d   |   %.2f €\n\n",
                totalGeralSaidas, totalGeralValor);
        fprintf(fout, "Valor médio por dia: %.2f €\n", media);
        
        fclose(fout);
        printf("\n Ficheiro '%s' gravado com sucesso!\n", nomeArq);
    }
    else if (opcao == 'C' || opcao == 'c') {
        char nomeArq[100];
        char separador;
        
        printf("\nNome do ficheiro (sem extensão): ");
        scanf("%s", nomeArq);
        strcat(nomeArq, ".csv");
        
        printf("Separador (vírgula ou ponto-e-vírgula)? (,;): ");
        scanf(" %c", &separador);
        
        FILE *fout = fopen(nomeArq, "w");
        if (fout == NULL) {
            printf(" Erro ao criar ficheiro!\n");
            return;
        }
        
        fprintf(fout, "Data%cSaidas%cValor_Total\n", separador, separador);
        
        for (int i = 0; i < numDias; i++) {
            fprintf(fout, "%02d/%02d/%d%c%d%c%.2f\n",
                   dados[i].dia, dados[i].mes, anoInicio,
                   separador,
                   dados[i].totalSaidas,
                   separador,
                   dados[i].totalValor);
        }
        
        fclose(fout);
        printf("\n Ficheiro '%s' gravado com sucesso!\n", nomeArq);
    }
    
    printf("\nPressione ENTER para continuar...");
    getchar();
    getchar();
}
