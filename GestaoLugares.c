#include "GestaoLugares.h"
#include "Estacionamento.h"
#include "Tarifas.h"
#include "validacoes.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define FICHEIRO_INDISPONIVEIS "lugares_indisponiveis.txt"

// Função: Exibe todos os dados de um registo de estacionamento
void mostrarDetalhesEstacionamento(estacionamento E) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════╗\n");
    printf("║            DETALHES DO ESTACIONAMENTO                     ║\n");
    printf("╠═══════════════════════════════════════════════════════════╣\n");
    printf("║                                                           ║\n");
    printf("║  Nº Entrada:  %-6d                                        ║\n", E.numE);
    printf("║  Matrícula:   %-10s                                       ║\n", E.matricula);
    printf("║  Lugar:       %-5s                                        ║\n", E.lugar);
    printf("║                                                           ║\n");
    printf("║  ───────────────────────────────────────────────────────  ║\n");
    printf("║                                                           ║\n");
    printf("║    ENTRADA:                                               ║\n");
    printf("║     Data: %02d/%02d/%d                                    ║\n", E.diaE, E.mesE, E.anoE);
    printf("║     Hora: %02d:%02d                                       ║\n",E.horaE, E.minE);
    printf("║                                                           ║\n");
    if (E.anoS == 0) {
        printf("║   ESTADO: AINDA NO PARQUE                                   ║\n");
    } else {
        printf("║   SAÍDA:                                                    ║\n");
        printf("║     Data: %02d/%02d/%d                                      ║\n",
               E.diaS, E.mesS, E.anoS);
        printf("║     Hora: %02d:%02d                                         ║\n",
               E.horaS, E.minS);
        printf("║                                                             ║\n");
        printf("║   Valor Pago: %.2f €                                        ║\n", E.valorPago);
    }
    printf("║                                                           ║\n");
    printf("╚═══════════════════════════════════════════════════════════╝\n");
}

// Função: Permite ao utilizador pesquisar um registo específico através do ID
void consultarRegisto(char *ficheiroEstacionamentos) {
    int numProcurado;
    int encontrado = 0;

    printf("\n╔═══════════════════════════════════════════════════════════╗\n");
    printf("║                 CONSULTAR REGISTO                         ║\n");
    printf("╚═══════════════════════════════════════════════════════════╝\n\n");

    // Solicita a entrada do utilizador
    printf("Digite o número de entrada: ");
    scanf("%d", &numProcurado);

    // Abre o ficheiro base para leitura
    FILE *f = fopen(ficheiroEstacionamentos, "r");
    if (f == NULL) {
        printf("Erro ao abrir ficheiro!\n");
        return;
    }
    estacionamento E;
    // Lê o ficheiro registo a registo (formato de 13 campos)
    while (fscanf(f, "%d %s %d %d %d %d %d %s %d %d %d %d %d",
                  &E.numE, E.matricula,
                  &E.anoE, &E.mesE, &E.diaE, &E.horaE, &E.minE,
                  E.lugar,
                  &E.anoS, &E.mesS, &E.diaS, &E.horaS, &E.minS) == 13) {

        // Verifica se o ID lido corresponde ao que o utilizador procura
        if (E.numE == numProcurado) {
            encontrado = 1;

            // Se o veículo já tiver saído (anoS != 0), calculamos o preço pago
            if (E.anoS != 0) {
                Tarifa tarifas[MAX_TARIFAS];
                int numTarifas = 0;

                // Carrega as tarifas atuais para garantir que o cálculo do preço é exato
                if (lertarifas(tarifas, &numTarifas)) {
                    E.valorPago = CalcularPreco(E.diaE, E.mesE, E.anoE, E.horaE, E.minE,
                                               E.diaS, E.mesS, E.anoS, E.horaS, E.minS,
                                               tarifas, numTarifas);
                }
            } else {
                // Se ainda está no parque, o valor pago é resetado para exibição
                E.valorPago = 0.0;
            }
            mostrarDetalhesEstacionamento(E);
            break;
        }
    }
    fclose(f);
    if (!encontrado) {
        printf("\n Registo nº %d não encontrado!\n", numProcurado);
    }
}

// Função: Permite editar campos de um registo já existente
int alterarRegisto(char *ficheiroEstacionamentos, Confparque config) {
    int numProcurado;
    int encontrado = 0;

    printf("\n╔═══════════════════════════════════════════════════════════╗\n");
    printf("║                   ALTERAR REGISTO                         ║\n");
    printf("╚═══════════════════════════════════════════════════════════╝\n\n");

    printf("Digite o número de entrada a alterar: ");
    scanf("%d", &numProcurado);

    // Ler todos os registos
    FILE *f = fopen(ficheiroEstacionamentos, "r");
    if (f == NULL) {
        printf("Erro ao abrir ficheiro!\n");
        return 0;
    }
    estacionamento registos[MAX_REG_EST];
    int numRegistos = 0;
    while (fscanf(f, "%d %s %d %d %d %d %d %s %d %d %d %d %d",
                  &registos[numRegistos].numE,
                  registos[numRegistos].matricula,
                  &registos[numRegistos].anoE, &registos[numRegistos].mesE,
                  &registos[numRegistos].diaE, &registos[numRegistos].horaE,
                  &registos[numRegistos].minE,
                  registos[numRegistos].lugar,
                  &registos[numRegistos].anoS, &registos[numRegistos].mesS,
                  &registos[numRegistos].diaS, &registos[numRegistos].horaS,
                  &registos[numRegistos].minS) == 13) {

        if (registos[numRegistos].numE == numProcurado) {
            encontrado = 1;
        }
        numRegistos++;
    }
    fclose(f);

    if (!encontrado) {
        printf("\n Registo nº %d não encontrado!\n", numProcurado);
        return 0;
    }

    // Encontrar o registo
    int indice = -1;
    for (int i = 0; i < numRegistos; i++) {
        if (registos[i].numE == numProcurado) {
            indice = i;
            break;
        }
    }

    // Mostrar registo atual
    printf("\n Registo atual:\n");
    mostrarDetalhesEstacionamento(registos[indice]);

    // Menu de alteração
    int opcao;
    printf("\n╔═══════════════════════════════════════╗\n");
    printf("║  O que deseja alterar?                ║\n");
    printf("╠═══════════════════════════════════════╣\n");
    printf("║  1. Matrícula                         ║\n");
    printf("║  2. Data/Hora de Entrada              ║\n");
    printf("║  3. Lugar                             ║\n");
    printf("║  4. Data/Hora de Saída                ║\n");
    printf("║  0. Cancelar                          ║\n");
    printf("╚═══════════════════════════════════════╝\n");
    printf("Opção: ");
    scanf("%d", &opcao);

    switch (opcao) {
        case 1: {
            char novaMatricula[MAX_MATRICULA];
            do {
                printf("Nova matrícula (XX-XX-XX): ");
                scanf("%s", novaMatricula);
                if (!validamatricula(novaMatricula)) {
                    printf(" Matrícula inválida!\n");
                }
            } while (!validamatricula(novaMatricula));
            strcpy(registos[indice].matricula, novaMatricula);
            break;
        }

        case 2: {
            int dia, mes, ano, hora, min;
            do {
                printf("Nova data de entrada (DD MM AAAA): ");
                scanf("%d %d %d", &dia, &mes, &ano);
                if (!validaData(dia, mes, ano)) {
                    printf(" Data inválida!\n");
                }
            } while (!validaData(dia, mes, ano));

            do {
                printf("Nova hora de entrada (HH MM): ");
                scanf("%d %d", &hora, &min);
                if (hora < 0 || hora > 23 || min < 0 || min > 59) {
                    printf(" Hora inválida!\n");
                }
            } while (hora < 0 || hora > 23 || min < 0 || min > 59);

            registos[indice].diaE = dia;
            registos[indice].mesE = mes;
            registos[indice].anoE = ano;
            registos[indice].horaE = hora;
            registos[indice].minE = min;
            break;
        }

        case 3: {
            char novoLugar[MAX_LUGAR];
            char letraMaxFila = 'A' + (config.numfilas - 1);
            do {
                printf("Novo lugar (ex: 1A05): ");
                scanf("%s", novoLugar);
                if (!validaLugar(novoLugar, config.numpisos, letraMaxFila, config.numlugares)) {
                    printf(" Lugar inválido!\n");
                }
            } while (!validaLugar(novoLugar, config.numpisos, letraMaxFila, config.numlugares));
            strcpy(registos[indice].lugar, novoLugar);
            break;
        }

        case 4: {
            if (registos[indice].anoS == 0) {
                printf(" Este veículo ainda não saiu! Use 'Registar Saída' primeiro.\n");
                return 0;
            }
            int dia, mes, ano, hora, min;
            do {
                printf("Nova data de saída (DD MM AAAA): ");
                scanf("%d %d %d", &dia, &mes, &ano);
                if (!validaData(dia, mes, ano)) {
                    printf(" Data inválida!\n");
                    continue;
                }
                printf("Nova hora de saída (HH MM): ");
                scanf("%d %d", &hora, &min);
                if (!validaEantesS(registos[indice].diaE, registos[indice].mesE,
                                  registos[indice].anoE, registos[indice].horaE,
                                  registos[indice].minE, dia, mes, ano, hora, min)) {
                    printf(" Saída deve ser posterior à entrada!\n");
                    continue;
                }
                break;
            } while (1);
            registos[indice].diaS = dia;
            registos[indice].mesS = mes;
            registos[indice].anoS = ano;
            registos[indice].horaS = hora;
            registos[indice].minS = min;
            break;
        }

        case 0:
            printf(" Alteração cancelada.\n");
            return 0;

        default:
            printf(" Opção inválida!\n");
            return 0;
    }

    // Gravar ficheiro atualizado
    f = fopen(ficheiroEstacionamentos, "w");
    if (f == NULL) {
        printf(" Erro ao gravar ficheiro!\n");
        return 0;
    }
    for (int i = 0; i < numRegistos; i++) {
        fprintf(f, "%d\t%s\t%d\t%d\t%d\t%d\t%d\t%s\t%d\t%d\t%d\t%d\t%d\n",
                registos[i].numE, registos[i].matricula,
                registos[i].anoE, registos[i].mesE, registos[i].diaE,
                registos[i].horaE, registos[i].minE,
                registos[i].lugar,
                registos[i].anoS, registos[i].mesS, registos[i].diaS,
                registos[i].horaS, registos[i].minS);
    }
    fclose(f);
    printf("\n Registo alterado com sucesso!\n");
    printf("\n Novo registo:\n");
    mostrarDetalhesEstacionamento(registos[indice]);
    return 1;
}

// Função: Remove permanentemente um registo do ficheiro de estacionamentos.
int eliminarRegisto(char *ficheiroEstacionamentos) {
    int numProcurado;
    int encontrado = 0;

    printf("\n╔═══════════════════════════════════════════════════════════╗\n");
    printf("║                   ELIMINAR REGISTO                        ║\n");
    printf("╚═══════════════════════════════════════════════════════════╝\n\n");

    printf("Digite o número de entrada a eliminar: ");
    scanf("%d", &numProcurado);

    // Ler todos os registos
    FILE *f = fopen(ficheiroEstacionamentos, "r");
    if (f == NULL) {
        printf(" Erro ao abrir ficheiro!\n");
        return 0;
    }
    estacionamento registos[MAX_REG_EST];
    int numRegistos = 0;
    while (fscanf(f, "%d %s %d %d %d %d %d %s %d %d %d %d %d",
                  &registos[numRegistos].numE,
                  registos[numRegistos].matricula,
                  &registos[numRegistos].anoE, &registos[numRegistos].mesE,
                  &registos[numRegistos].diaE, &registos[numRegistos].horaE,
                  &registos[numRegistos].minE,
                  registos[numRegistos].lugar,
                  &registos[numRegistos].anoS, &registos[numRegistos].mesS,
                  &registos[numRegistos].diaS, &registos[numRegistos].horaS,
                  &registos[numRegistos].minS) == 13) {
        if (registos[numRegistos].numE == numProcurado) {
            encontrado = 1;
        }
        numRegistos++;
    }
    fclose(f);
    if (!encontrado) {
        printf("\n Registo nº %d não encontrado!\n", numProcurado);
        return 0;
    }
    
    // Mostrar registo a eliminar
    for (int i = 0; i < numRegistos; i++) {
        if (registos[i].numE == numProcurado) {
            mostrarDetalhesEstacionamento(registos[i]);
            break;
        }
    }

    // Confirmar eliminação
    char confirmacao;
    printf("\n  TEM A CERTEZA que deseja eliminar este registo? (s/n): ");
    scanf(" %c", &confirmacao);

    if (confirmacao != 's' && confirmacao != 'S') {
        printf(" Eliminação cancelada.\n");
        return 0;
    }

    // Gravar ficheiro sem o registo eliminado
    f = fopen(ficheiroEstacionamentos, "w");
    if (f == NULL) {
        printf(" Erro ao gravar ficheiro!\n");
        return 0;
    }
    for (int i = 0; i < numRegistos; i++) {
        if (registos[i].numE != numProcurado) {
            fprintf(f, "%d\t%s\t%d\t%d\t%d\t%d\t%d\t%s\t%d\t%d\t%d\t%d\t%d\n",
                    registos[i].numE, registos[i].matricula,
                    registos[i].anoE, registos[i].mesE, registos[i].diaE,
                    registos[i].horaE, registos[i].minE,
                    registos[i].lugar,
                    registos[i].anoS, registos[i].mesS, registos[i].diaS,
                    registos[i].horaS, registos[i].minS);
        }
    }
    fclose(f);
    printf("\n Registo eliminado com sucesso!\n");
    return 1;
}

// Função: Lê o ficheiro de lugares indisponíveis e atualiza a matriz do mapa
void carregarLugaresIndisponiveis(Lugar mapa[][MAX_FILAS][MAX_LUGARES],
                                  Confparque config) {
    FILE *f = fopen(FICHEIRO_INDISPONIVEIS, "r");
    if (f == NULL) {
        return;
    }

    char lugar[MAX_LUGAR];
    char motivo;

    // Lê o ficheiro enquanto encontrar pares de dados (Lugar e Motivo)
    while (fscanf(f, "%s %c", lugar, &motivo) == 2) {
        int piso, fila, numero;
        ProcessarLugar(lugar, &piso, &fila, &numero);
        piso--;
        numero--;

        // Garante que o lugar lido existe na configuração atual do parque
        if (piso >= 0 && piso < config.numpisos &&
            fila >= 0 && fila < config.numfilas &&
            numero >= 0 && numero < config.numlugares) {
            mapa[piso][fila][numero].status = motivo;
            mapa[piso][fila][numero].motivo = motivo;
        }
    }

    fclose(f);
}

// Função: Percorre a matriz do mapa e grava num ficheiro todos os lugares que não estão livres
void guardarLugaresIndisponiveis(Lugar mapa[][MAX_FILAS][MAX_LUGARES],
                                 Confparque config) {
    FILE *f = fopen(FICHEIRO_INDISPONIVEIS, "w");
    if (f == NULL) {
        printf(" Erro ao guardar lugares indisponíveis!\n");
        return;
    }

    // Percorrer cada coordenada da matriz
    for (int p = 0; p < config.numpisos; p++) {
        for (int fil = 0; fil < config.numfilas; fil++) {
            for (int l = 0; l < config.numlugares; l++) {
                char status = mapa[p][fil][l].status;

                // Se é indisponível (i, o, r, m)
                if (status == 'i' || status == 'o' ||
                    status == 'r' || status == 'm') {
                    fprintf(f, "%d%c%02d %c\n",
                            p + 1, 'A' + fil, l + 1, status);
                }
            }
        }
    }
    fclose(f);
}

// Função: Interface para o utilizador marcar um lugar como indisponível no sistema
int marcarLugarIndisponivel(Confparque config) {
    char lugar[MAX_LUGAR];
    char motivo;
    char letraMaxFila = 'A' + (config.numfilas - 1);

    printf("\n╔═════════════════════════════════════════════════════════════╗\n");
    printf("║          MARCAR LUGAR COMO INDISPONÍVEL                     ║\n");
    printf("╚═════════════════════════════════════════════════════════════╝\n\n");

    // Obter e validar a sintaxe do lugar
    do {
        printf("Digite o lugar (ex: 1A05): ");
        scanf("%s", lugar);
       
        if (!validaLugar(lugar, config.numpisos, letraMaxFila, config.numlugares)) {
            printf("Lugar inválido!\n");
        }
    } while (!validaLugar(lugar, config.numpisos, letraMaxFila, config.numlugares));

    // Verificar se o lugar está ocupado
    FILE *f = fopen("estacionamentos.txt", "r");
    if (f != NULL) {
        estacionamento E;
        while (fscanf(f, "%d %s %d %d %d %d %d %s %d %d %d %d %d",
                      &E.numE, E.matricula,
                      &E.anoE, &E.mesE, &E.diaE, &E.horaE, &E.minE,
                      E.lugar,
                      &E.anoS, &E.mesS, &E.diaS, &E.horaS, &E.minS) == 13) {

            // Se o lugar coincide e o ano de saída é 0, o carro ainda está lá
            if (strcmp(E.lugar, lugar) == 0 && E.anoS == 0) {
                fclose(f);
                printf(" Este lugar está ocupado (matrícula: %s)!\n", E.matricula);
                printf("   Registe a saída primeiro.\n");
                return 0;
            }
        }
        fclose(f);
    }

    // Escolha do Motivo
    printf("\n╔═══════════════════════════════════════╗\n");
    printf("║  Motivo da indisponibilidade:         ║\n");
    printf("╠═══════════════════════════════════════╣\n");
    printf("║  i - Condições inadequadas            ║\n");
    printf("║  o - Objeto de obras                  ║\n");
    printf("║  r - Reservado                        ║\n");
    printf("║  m - Outros motivos                   ║\n");
    printf("╚═══════════════════════════════════════╝\n");
    printf("Motivo: ");
    scanf(" %c", &motivo);

    // Validação do motivo
    if (motivo != 'i' && motivo != 'o' && motivo != 'r' && motivo != 'm') {
        printf(" Motivo inválido!\n");
        return 0;
    }
    // Atualização do Estado
    Lugar mapa[MAX_PISOS][MAX_FILAS][MAX_LUGARES];

    // Prepara o mapa em memória
    InicializarMapa(mapa, config);
    carregarLugaresIndisponiveis(mapa, config);

    int piso, fila, numero;
    ProcessarLugar(lugar, &piso, &fila, &numero);
    piso--;
    numero--;

    // Aplica a alteração
    mapa[piso][fila][numero].status = motivo;
    mapa[piso][fila][numero].motivo = motivo;

    guardarLugaresIndisponiveis(mapa, config);
    printf("\n Lugar %s marcado como indisponível (motivo: %c)\n", lugar, motivo);
    return 1;
}

// Função: Remove a marcação de indisponibilidade de um lugar
int tornarLugarDisponivel(Confparque config) {
    char lugar[MAX_LUGAR];
    char letraMaxFila = 'A' + (config.numfilas - 1);

    printf("\n╔═══════════════════════════════════════════════════════════╗\n");
    printf("║          TORNAR LUGAR DISPONÍVEL                        ║\n");
    printf("╚═══════════════════════════════════════════════════════════╝\n\n");

    // Obter e validar o identificador do lugar
    do {
        printf("Digite o lugar (ex: 1A05): ");
        scanf("%s", lugar);

        if (!validaLugar(lugar, config.numpisos, letraMaxFila, config.numlugares)) {
            printf("❌ Lugar inválido!\n");
        }
    } while (!validaLugar(lugar, config.numpisos, letraMaxFila, config.numlugares));

    // ETAPA 2: Carregar o mapa atual de indisponíveis 
    Lugar mapa[MAX_PISOS][MAX_FILAS][MAX_LUGARES];
    InicializarMapa(mapa, config);
    carregarLugaresIndisponiveis(mapa, config);

    // Converte a string para coordenadas da matriz
    int piso, fila, numero;
    ProcessarLugar(lugar, &piso, &fila, &numero);
    piso--;
    numero--;

    char statusAtual = mapa[piso][fila][numero].status;

    // Verificar se o lugar realmente precisa de ser libertado
    if (statusAtual != 'i' && statusAtual != 'o' &&
        statusAtual != 'r' && statusAtual != 'm') {
        printf("Este lugar já está disponível!\n");
        return 0;
    }

    // Confirmação e Alteração
    printf("Lugar %s está marcado como: %c\n", lugar, statusAtual);
    char confirmacao;
    printf("Deseja torná-lo disponível? (s/n): ");
    scanf(" %c", &confirmacao);
    if (confirmacao != 's' && confirmacao != 'S') {
        printf("Operação cancelada.\n");
        return 0;
    }
    mapa[piso][fila][numero].status = '-';
    mapa[piso][fila][numero].motivo = '\0';

    // Guardar
    guardarLugaresIndisponiveis(mapa, config);
    printf("\n Lugar %s agora está disponível!\n", lugar);
    return 1;
}

// Função: Menu dedicado à administração e manutenção do parque
void menuGestaoLugares(Confparque config) {
    int opcao;
    if (!lerconfig(&config)) {
            printf("Erro: Não foi possível carregar a configuração do parque!\n");
            return;
        }
    do {
        printf("\n");
        printf("╔═══════════════════════════════════════════════════════════╗\n");
        printf("║                  GESTÃO DE LUGARES                        ║\n");
        printf("╠═══════════════════════════════════════════════════════════╣\n");
        printf("║                                                           ║\n");
        printf("║  1.  Consultar registo                                    ║\n");
        printf("║  2.  Alterar registo                                      ║\n");
        printf("║  3.  Eliminar registo                                     ║\n");
        printf("║  4.  Tornar lugar indisponível/motivo                     ║\n");
        printf("║  5.  Tornar lugar disponível                              ║\n");
        printf("║                                                           ║\n");
        printf("║  0.   Voltar ao menu principal                            ║\n");
        printf("║                                                           ║\n");
        printf("╚═══════════════════════════════════════════════════════════╝\n");
        printf("\nEscolha uma opção: ");
        scanf("%d", &opcao);

        switch (opcao) {
            case 1:
                consultarRegisto("estacionamentos.txt");
                printf("\nPressione ENTER para continuar...");
                getchar();
                getchar();
                break;

            case 2:
                if (alterarRegisto("estacionamentos.txt", config)) {
                    atualizarValidacao(config);
                }
                printf("\nPressione ENTER para continuar...");
                getchar();
                getchar();
                break;

            case 3:
                if (eliminarRegisto("estacionamentos.txt")) {
                    atualizarValidacao(config);
                }
                printf("\nPressione ENTER para continuar...");
                getchar();
                getchar();
                break;

            case 4:
                marcarLugarIndisponivel(config);
                printf("\nPressione ENTER para continuar...");
                getchar();
                getchar();
                break;

            case 5:
                tornarLugarDisponivel(config);
                printf("\nPressione ENTER para continuar...");
                getchar();
                getchar();
                break;

            case 0:
                printf("Voltando ao menu principal...\n");
                break;

            default:
                printf(" Opção inválida!\n");
                printf("\nPressione ENTER para continuar...");
                getchar();
                getchar();
        }

    } while (opcao != 0);
}
