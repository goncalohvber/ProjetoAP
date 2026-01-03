#include "Estacionamento.h"
#include "Util.h"
#include "Instalacao.h"
#include "Listagens.h"
#include <stdio.h>
#include <time.h>
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

// Verifica se um ano é bissexto.
// Regras:
//   - Ano divisível por 4 E NÃO divisível por 100 = bissexto
//   - OU ano divisível por 400 = bissexto
// 
// Parâmetros:
//   - ano: ano a verificar
int ABissexto(int ano){
    if ((ano % 4 == 0 && ano % 100 != 0) || (ano % 400 == 0)) {
            return 1;
        }
        return 0;
    }

// Valida se uma data (dia/mês/ano) é válida.
// 
// Validações:
//   - Ano entre 0 e 2100
//   - Mês entre 1 e 12
//   - Dia válido para o mês (considera fevereiro em anos bissextos)
//   - Meses com 30 dias: abril, junho, setembro, novembro
//   - Meses com 31 dias: janeiro, março, maio, julho, agosto, outubro, dezembro
//   - Fevereiro: 28 dias (29 em anos bissextos)
// 
// Parâmetros:
//   - dia: dia do mês (1-31)
//   - mes: mês do ano (1-12)
//   - ano: ano (0-2100)
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

// Valida o formato de uma matrícula portuguesa.
// Formato esperado: AA-00-AA
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

// Valida se um código de lugar do parque é válido.
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

// Verifica se a data/hora de Entrada é ANTES da data/hora de Saída.
// Garante que um veículo não pode sair antes de entrar.
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

// Mostra uma mensagem ao utilizador e aguarda Enter para continuar.
void mostrarMensagem(char *mens){
    printf("\n%s", mens);
    printf("\nDigite <Enter> para continuar... ");
    getchar();
    getchar();
}



// Compara duas datas/horas completas.
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

// Limpa o ecrã do terminal/consola.
void limparTela(void) {
    #ifdef _WIN32 // Se for Windows
        system("cls");
    #else // Para Linux, macOS...
        system("clear");
    #endif
}

// Obtém a data e hora atual do sistema.
// Usa a biblioteca <time.h> para aceder ao relógio do sistema.
void obterDataHoraAtual(int *dia, int *mes, int *ano, int *hora, int *min) {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    
    *dia = tm.tm_mday;
    *mes = tm.tm_mon + 1;  // tm_mon vai de 0-11, por isso +1
    *ano = tm.tm_year + 1900;  // tm_year conta desde 1900
    *hora = tm.tm_hour;
    *min = tm.tm_min;
}

// Calcula as estatísticas de ocupação do parque.
// 
// Calcula:
//   - Total de lugares: pisos × filas × lugares
//   - Lugares ocupados: veículos que ainda não saíram (anoS = 0)
//   - Lugares indisponíveis: lidos de "lugares_indisponiveis.txt"
//   - Lugares disponíveis: total - ocupados - indisponíveis
// 
// Lê ficheiros:
//   - "estacionamentos.txt": para contar ocupação
//   - "lugares_indisponiveis.txt": para contar indisponíveis
EstatisticasParque obterEstatisticasParque(Confparque config) {
    EstatisticasParque stats;
    
    // Calcular total de lugares
    stats.total = config.numpisos * config.numfilas * config.numlugares;
    stats.ocupados = 0;
    stats.indisponiveis = 0;
    
    // Contar lugares ocupados (veículos que ainda não saíram)
    FILE *f = fopen("estacionamentos.txt", "r");
    if (f != NULL) {
        estacionamento E;
        while (fscanf(f, "%d %s %d %d %d %d %d %s %d %d %d %d %d",
                      &E.numE, E.matricula,
                      &E.anoE, &E.mesE, &E.diaE, &E.horaE, &E.minE,
                      E.lugar,
                      &E.anoS, &E.mesS, &E.diaS, &E.horaS, &E.minS) == 13) {
            
            // Se ainda não saiu (anoS = 0), está ocupado
            if (E.anoS == 0) {
                stats.ocupados++;
            }
        }
        fclose(f);
    }
    
    // Contar lugares indisponíveis
    f = fopen("lugares_indisponiveis.txt", "r");
    if (f != NULL) {
        char lugar[10];
        char motivo;
        while (fscanf(f, "%s %c", lugar, &motivo) == 2) {
            stats.indisponiveis++;
        }
        fclose(f);
    }
    
    // Calcular disponíveis
    stats.disponiveis = stats.total - stats.ocupados - stats.indisponiveis;
    
    return stats;
}

// Menu interativo para gerir as tarifas do parque.
void menuGestaoTarifas(void) {
    Tarifa tarifas[MAX_TARIFAS];
    int numTarifas = 0;
    int opcao;
    
    // Carregar tarifas
    if (!lertarifas(tarifas, &numTarifas)) {
        printf("\n❌ Erro ao carregar tarifas!\n");
        mostrarMensagem("Prima Enter para continuar...");
        return;
    }
    
    do {
        system("cls");
        printf("\n");
        printf("\t╔════════════════════════════════════════════════════╗\n");
        printf("\t║             GESTÃO DE TARIFAS                      ║\n");
        printf("\t╚════════════════════════════════════════════════════╝\n\n");
        
        // Mostrar tabela de tarifas atual
        mostrarTabela(tarifas, numTarifas);
        
        printf("\n");
        printf("\t╔════════════════════════════════════════════════════╗\n");
        printf("\t║                                                    ║\n");
        printf("\t║  1.    Modificar Tarifa                            ║\n");
        printf("\t║  2.    Recarregar Tarifas                          ║\n");
        printf("\t║  3.    Informação sobre Tarifas                    ║\n");
        printf("\t║                                                    ║\n");
        printf("\t║  0.    Voltar                                      ║\n");
        printf("\t║                                                    ║\n");
        printf("\t╚════════════════════════════════════════════════════╝\n");
        printf("\n\t>> Escolha uma opção: ");
        scanf("%d", &opcao);
        
        switch (opcao) {
            case 1: {
                // Modificar tarifa
                printf("\n");
                if (modificatarifa(tarifas, &numTarifas)) {
                    printf("\n✅ Tarifa modificada com sucesso!\n");
                    // Recarregar para mostrar valores atualizados
                    lertarifas(tarifas, &numTarifas);
                } else {
                    printf("\n❌ Erro ao modificar tarifa!\n");
                }
                mostrarMensagem("\nPrima Enter para continuar...");
                break;
            }
            
            case 2: {
                // Recarregar tarifas
                if (lertarifas(tarifas, &numTarifas)) {
                    printf("\n✅ Tarifas recarregadas com sucesso!\n");
                } else {
                    printf("\n❌ Erro ao recarregar tarifas!\n");
                }
                mostrarMensagem("Prima Enter para continuar...");
                break;
            }
            
            case 3: {
                // Informação sobre tarifas
                system("cls");
                printf("\n");
                printf("╔══════════════════════════════════════════════════════════════╗\n");
                printf("║                  INFORMAÇÃO SOBRE TARIFAS                    ║\n");
                printf("╠══════════════════════════════════════════════════════════════╣\n");
                printf("║                                                              ║\n");
                printf("║  TIPOS DE TARIFA:                                            ║\n");
                printf("║  ──────────────────────────────────────────────────────────  ║\n");
                printf("║                                                              ║\n");
                printf("║     TARIFAS HORÁRIAS (Tipo: H)                               ║\n");
                printf("║     CT1 - Horário Diurno (ex: 08:00 - 22:00)                ║\n");
                printf("║     CT2 - Horário Noturno (ex: 22:00 - 08:00)               ║\n");
                printf("║     → Cobrado por minuto dentro do intervalo                ║\n");
                printf("║                                                              ║\n");
                printf("║     TARIFAS DIÁRIAS (Tipo: D)                                ║\n");
                printf("║     CT3 - Taxa de Mudança de Dia (pernoita)                 ║\n");
                printf("║     CT4 - Taxa por Dia Completo (2+ dias)                   ║\n");
                printf("║     → Aplicado quando o veículo fica mais de 24h            ║\n");
                printf("║                                                              ║\n");
                printf("║  EXEMPLO DE CÁLCULO:                                         ║\n");
                printf("║  ──────────────────────────────────────────────────────────  ║\n");
                printf("║  Entrada:  10/01/2026 às 10:00                              ║\n");
                printf("║  Saída:    12/01/2026 às 14:00                              ║\n");
                printf("║                                                              ║\n");
                printf("║  • Dia 10: 10:00-22:00 (12h diurnas CT1)                    ║\n");
                printf("║           22:00-24:00 (2h noturnas CT2)                     ║\n");
                printf("║  • Pernoita: 1x CT3                                          ║\n");
                printf("║  • Dia 11 completo: 1x CT4                                   ║\n");
                printf("║  • Dia 12: 00:00-08:00 (8h noturnas CT2)                    ║\n");
                printf("║           08:00-14:00 (6h diurnas CT1)                      ║\n");
                printf("║                                                              ║\n");
                printf("╚══════════════════════════════════════════════════════════════╝\n");
                mostrarMensagem("\nPrima Enter para continuar...");
                break;
            }
            
            case 0:
                printf("\n    Voltando ao menu de configurações...\n");
                break;
            
            default:
                printf("\n   Opção inválida!\n");
                mostrarMensagem("Prima Enter para continuar...");
        }
        
    } while (opcao != 0);
}


void menuConfiguracoes(Confparque config) {
    int opcao;
    
    do {
        system("clear");
        printf("\n");
        printf("\t╔════════════════════════════════════════╗\n");
        printf("\t║            MENU DE CONFIGURAÇÕES       ║\n");
        printf("\t╠════════════════════════════════════════╣\n");
        printf("\t║                                        ║\n");
        printf("\t║  1.    Reconfigurar Parque             ║\n");
        printf("\t║  2.    Gestão de Tarifas               ║\n");
        printf("\t║                                        ║\n");
        printf("\t║  0.    Voltar ao menu principal        ║\n");
        printf("\t║                                        ║\n");
        printf("\t╚════════════════════════════════════════╝\n");
        printf("\n\t>> Escolha uma opção: ");
        scanf("%d", &opcao);
        
        switch (opcao) {
            case 1: {
                // Reconfigurar Parque
                char confirmacao;
                printf("\n    ATENÇÃO: Reconfigurar o parque irá reiniciar todas as configurações!\n");
                printf("Tem a certeza que deseja continuar? (s/n): ");
                scanf(" %c", &confirmacao);
                
                if (confirmacao == 's' || confirmacao == 'S') {
                    remove("configfeita.txt");
                    system("clear");
                    personalizapp(config);
                    return; // Sair do menu de configurações
                } else {
                    printf("\n   Operação cancelada.\n");
                    mostrarMensagem("Prima Enter para continuar...");
                }
                break;
            }
            
            case 2: {
                // Gestão de Tarifas
                menuGestaoTarifas();
                break;
            }
            
            case 0:
                printf("\n    Voltando ao menu principal...\n");
                break;
            
            default:
                printf("\n   Opção inválida!\n");
                mostrarMensagem("Prima Enter para continuar...");
        }
        
    } while (opcao != 0);
}

