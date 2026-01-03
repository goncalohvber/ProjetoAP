#include "Tarifas.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Guarda todas as tarifas no ficheiro "Tarifario.txt"
// 
// Parâmetros:
//   - tarifas[]: array com as tarifas a guardar
//   - numtarifas: número de tarifas no array
// 
// 
// Formato do ficheiro: tipo codigo horaInf:minInf horaSup:minSup valor
int guardartarifas(Tarifa tarifas[], int numtarifas){
    FILE *f = fopen("Tarifario.txt", "w");
    if(f==NULL){
        return 0;
    }
    for(int i=0 ; i<numtarifas; i++){
        fprintf(f, "%c\t%s\t%02d:%02d\t%02d:%02d\t%.2f\n", tarifas[i].tipo, tarifas[i].codigo, tarifas[i].horaInf, tarifas[i].minInf, tarifas[i].horaSup, tarifas[i].minSup,tarifas[i].valor);
    }
    fclose(f);
    return 1;
   
}

// Permite ao utilizador modificar o valor de uma tarifa existente.
// 
//   1. Mostra os códigos disponíveis (CT1, CT2, CT3, CT4)
//   2. Pede ao utilizador para escolher um código
//   3. Mostra os dados atuais da tarifa
//   4. Pede o novo valor (tem de ser > 0)
//   5. Atualiza e guarda no ficheiro
// 
// Parâmetros:
//   - tarifas[]: array de tarifas
//   - numtarifas: ponteiro para o número de tarifas
int modificatarifa(Tarifa tarifas[], int *numtarifas) {
    float valortemp;
    char Codigoescolhido[10];
    int encontrado = 0;
    
    printf("\n╔════════════════════════════════════════╗\n");
    printf("║      MODIFICAR TARIFA                ║\n");
    printf("╚════════════════════════════════════════╝\n\n");
    
    // Limpar buffer
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
    
    printf("Códigos disponíveis: ");
    for (int i = 0; i < *numtarifas; i++) {
        printf("%s", tarifas[i].codigo);
        if (i < *numtarifas - 1) printf(", ");
    }
    printf("\n\n");
    
    printf("Qual o código da tarifa que quer modificar? ");
    scanf("%s", Codigoescolhido);
    
    for(int i = 0; i < *numtarifas; i++) {
        if(strcmp(tarifas[i].codigo, Codigoescolhido) == 0) {
            encontrado = 1;
            
            printf("\n┌────────────────────────────────────┐\n");
            printf("│ TARIFA ATUAL: %s                   \n", Codigoescolhido);
            printf("├────────────────────────────────────┤\n");
            printf("│ Tipo: %c                           \n", tarifas[i].tipo);
            printf("│ Horário: %02d:%02d - %02d:%02d         \n",
                   tarifas[i].horaInf, tarifas[i].minInf,
                   tarifas[i].horaSup, tarifas[i].minSup);
            printf("│ Valor: %.2f €                      \n", tarifas[i].valor);
            printf("└────────────────────────────────────┘\n\n");
            
            do {
                printf("Novo valor (€): ");
                if (scanf("%f", &valortemp) != 1) {
                    printf(" Entrada inválida! Use números.\n");
                    while ((c = getchar()) != '\n' && c != EOF);
                    valortemp = -1;
                    continue;
                }
                
                if (valortemp <= 0) {
                    printf(" O valor deve ser maior que 0.00 €\n\n");
                }
            } while(valortemp <= 0);
            
            tarifas[i].valor = valortemp;
            
            if (guardartarifas(tarifas, *numtarifas)) {
                printf("\n Valor de %s alterado para %.2f €\n",
                       Codigoescolhido, valortemp);
                return 1;
            } else {
                printf("\n Erro ao guardar tarifa!\n");
                return 0;
            }
        }
    }
    
    if (!encontrado) {
        printf("\n O código '%s' não existe no tarifário!\n", Codigoescolhido);
    }
    
    return 0;
}

// Apresenta todas as tarifas numa tabela formatada e visual.
// 
// A tabela inclui:
//   - Código da tarifa (CT1, CT2, CT3, CT4)
//   - Valor em euros
//   - Horário (hora início - hora fim)
//   - Tipo (H = Horário, D = Diário)
//   - Descrição legível (ex: "Horário Diurno", "Mudança de Dia")
// 
// Parâmetros:
//   - tarifas[]: array de tarifas a mostrar
//   - n: número de tarifas no array
void mostrarTabela(Tarifa tarifas[], int n) {
    printf("\n┌─────────────────────────────────────────────────────────────┐\n");
    printf("│                TARIFÁRIO ATUAL                             │\n");
    printf("├────────┬──────────┬───────────────┬──────┬─────────────────┤\n");
    printf("│ Código │  Valor   │   Horário     │ Tipo │   Descrição     │\n");
    printf("├────────┼──────────┼───────────────┼──────┼─────────────────┤\n");
    
    for (int i = 0; i < n; i++) {
        char descricao[20];
        
        if (tarifas[i].tipo == 'H') {
            if (strcmp(tarifas[i].codigo, "CT1") == 0) {
                strcpy(descricao, "Horário Diurno");
            } else if (strcmp(tarifas[i].codigo, "CT2") == 0) {
                strcpy(descricao, "Horário Noturno");
            } else {
                strcpy(descricao, "Horário");
            }
        } else if (tarifas[i].tipo == 'D') {
            if (strcmp(tarifas[i].codigo, "CT3") == 0) {
                strcpy(descricao, "Mudança de Dia");
            } else if (strcmp(tarifas[i].codigo, "CT4") == 0) {
                strcpy(descricao, "Dia Completo");
            } else {
                strcpy(descricao, "Diário");
            }
        }
        
        printf("│  %4s  │ %6.2f € │ %02d:%02d - %02d:%02d │  %c   │ %-15s │\n",
               tarifas[i].codigo,
               tarifas[i].valor,
               tarifas[i].horaInf, tarifas[i].minInf,
               tarifas[i].horaSup, tarifas[i].minSup,
               tarifas[i].tipo,
               descricao);
    }
    
    printf("└────────┴──────────┴───────────────┴──────┴─────────────────┘\n");
}

// Carrega as tarifas do ficheiro "Tarifario.txt".
// Se o ficheiro não existir, cria-o com valores padrão:
//   - CT1: Diurno (08:00-22:00) a 1.50€/hora
//   - CT2: Noturno (22:00-08:00) a 0.75€/hora
//   - CT3: Mudança de Dia a 5.00€
//   - CT4: Dia Completo a 10.00€
// 
// Validações:
//   - Ignora linhas vazias ou mal formatadas
//   - Tipo tem de ser 'H' ou 'D'
//   - Valor tem de ser > 0
// 
// Parâmetros:
//   - tarifas[]: array onde guardar as tarifas lidas
//   - numtarifas: ponteiro que recebe o número de tarifas carregadas
int lertarifas(Tarifa tarifas[], int *numtarifas) {
    // Validar parâmetros
    if (tarifas == NULL || numtarifas == NULL) {
        return 0;
    }
    
    // Só mostrar erro se o ficheiro NÃO existir
    FILE *f = fopen("Tarifario.txt", "r");
    
    if (f == NULL) {
        printf("  Tarifario.txt não encontrado. A criar ficheiro...\n");
        
        // Criar ficheiro com tarifas padrão
        f = fopen("Tarifario.txt", "w");
        if (f != NULL) {
            fprintf(f, "H\tCT1\t08:00\t22:00\t1.50\n");
            fprintf(f, "H\tCT2\t22:00\t08:00\t0.75\n");
            fprintf(f, "D\tCT3\t00:00\t23:59\t5.00\n");
            fprintf(f, "D\tCT4\t00:00\t23:59\t10.00\n");
            fclose(f);
            printf(" Tarifario.txt criado com valores padrão.\n");
        } else {
            printf(" Erro ao criar Tarifario.txt!\n");
            return 0;
        }
        
        // Reabrir para leitura
        f = fopen("Tarifario.txt", "r");
        if (f == NULL) {
            printf(" Não foi possível abrir o ficheiro após criação.\n");
            return 0;
        }
    }

    *numtarifas = 0;
    char linha[200];
    
    // Ler linha por linha
    while (fgets(linha, sizeof(linha), f) != NULL && *numtarifas < MAX_TARIFAS) {
        // Remover \n do final
        linha[strcspn(linha, "\n")] = 0;
        
        // Ignorar linhas vazias
        if (strlen(linha) == 0) {
            continue;
        }
        
        // Variaveis temporarias
        char tipo;
        char codigo[10];
        int horaInf, minInf, horaSup, minSup;
        float valor;
        int resultado = sscanf(linha, " %c %s %d:%d %d:%d %f",
                               &tipo, codigo, &horaInf, &minInf, &horaSup, &minSup, &valor);
        
        if (resultado != 7) {
            printf("   Linha mal formatada: '%s'\n", linha);
            continue;
        }
        
        if (tipo != 'H' && tipo != 'D') {
            printf("   Tipo '%c' inválido\n", tipo);
            continue;
        }
        
        if (valor <= 0) {
            printf("   Valor %.2f inválido\n", valor);
            continue;
        }
        
        tarifas[*numtarifas].tipo = tipo;
        strcpy(tarifas[*numtarifas].codigo, codigo);
        tarifas[*numtarifas].horaInf = horaInf;
        tarifas[*numtarifas].minInf = minInf;
        tarifas[*numtarifas].horaSup = horaSup;
        tarifas[*numtarifas].minSup = minSup;
        tarifas[*numtarifas].valor = valor;
        
        (*numtarifas)++;
    }
    
    fclose(f);
    
    if (*numtarifas == 0) {
        printf(" Nenhuma tarifa válida foi carregada\n");
        return 0;
    }
    return 1;
}

// Procura uma tarifa pelo código e retorna o seu valor.
// 
// Parâmetros:
//   - tarifas[]: array de tarifas
//   - numTarifas: número de tarifas no array
//   - codigoProcurado: código da tarifa (ex: "CT1", "CT2")
float ObterPreco(Tarifa tarifas[], int numTarifas, char *codigoProcurado) { //Função para retornar o valor de qualquer código
    for (int i = 0; i < numTarifas; i++) {
        if (strcmp(tarifas[i].codigo, codigoProcurado) == 0) {
            return tarifas[i].valor;
        }
    }
    return 0.0;
}

// Procura uma tarifa pelo código e retorna a sua hora de início.
int ObterHoraInicio(Tarifa tarifas[], int numTarifas, char *codigoProcurado) { // Retorna a hora de início de uma tarifa de qualquer codigo
    for (int i = 0; i < numTarifas; i++) {
        if (strcmp(tarifas[i].codigo, codigoProcurado) == 0) {
            return tarifas[i].horaInf;
        }
    }
    return 8; // Valor por defeito se falhar (8:00)
}


// Função auxiliar para verificar se é horário diurno (nao fazemos o noturno porque basta usar um else)
int EHorarioDiurno(int hora, int hInicioDiurno, int hFimDiurno) {
    return (hora >= hInicioDiurno && hora < hFimDiurno);
}

// Calcula o preço total do estacionamento entre duas datas/horas.
// 
// O cálculo considera:
//   1. DIÁRIAS (CT3 e CT4):
//      - Se passou a meia-noite: adiciona CT3 (mudança de dia)
//      - Se ficou mais de 2 dias: adiciona CT4 por cada dia extra
// 
//   2. TARIFAS HORÁRIAS (CT1 e CT2):
//      - Calcula minuto a minuto
//      - Aplica CT1 (diurno) ou CT2 (noturno) conforme a hora
//      - Divide o preço por 60 para obter o custo por minuto
float CalcularPreco(int dE, int mE, int aE, int hE, int minE,
                    int dS, int mS, int aS, int hS, int minS,
                    Tarifa tarifas[], int numTarifas)
{
    float totalPagar = 0.0;

    float precoCT1 = ObterPreco(tarifas, numTarifas, "CT1"); // Diurno
    float precoCT2 = ObterPreco(tarifas, numTarifas, "CT2"); // Noturno
    float precoCT3 = ObterPreco(tarifas, numTarifas, "CT3"); // Mudança Dia
    float precoCT4 = ObterPreco(tarifas, numTarifas, "CT4"); // Mais de dois dia
    
    // Descobrir a que horas começa o dia (8h no CT1)
    int inicioDia = ObterHoraInicio(tarifas, numTarifas, "CT1");
    // Descobrir a que horas acaba o dia ( 22h no CT1)
    int fimDia = ObterHoraInicio(tarifas, numTarifas, "CT2");
    // (Nota: Se CT1 é 08:00-22:00, o fimDia deve ser 22)

    // Calcular Diferença de Dias
    int diferencaDias = dS - dE;

    // --- CÁLCULO DAS DIÁRIAS (CT3 e CT4) ---
    if (diferencaDias > 0) {
        // Se ficou mais de 1 dia (dias "do meio" completos)
        if (diferencaDias > 1) {
            totalPagar += (diferencaDias - 1) * precoCT4;
        }
        // Taxa de mudança de dia (pernoita)
        totalPagar += precoCT3;
    }

    // --- CÁLCULO DAS HORAS (CT1 e CT2) ---
    // Convertemos tudo para minutos absolutos do dia (0 a 1440)
    long minutosEntrada = (hE * 60) + minE;
    long minutosSaida = (hS * 60) + minS;

    if (diferencaDias == 0) {
        // MESMO DIA: Diferença simples
        long duracao = minutosSaida - minutosEntrada;
        
        for (int i = 0; i < duracao; i++) {
            int minutoAtualDoDia = minutosEntrada + i;
            int horaAtual = (minutoAtualDoDia / 60); // Divisão inteira dá a hora (0-23)
            
            if (EHorarioDiurno(horaAtual, inicioDia, fimDia)) {
                totalPagar += (precoCT1 / 60.0); // Preço por minuto diurno
            } else {
                totalPagar += (precoCT2 / 60.0); // Preço por minuto noturno
            }
        }
    }
    else {
        // DIAS DIFERENTES:
        long minAteMeiaNoite = (24 * 60) - minutosEntrada;
        for (int i = 0; i < minAteMeiaNoite; i++) {
            int horaAtual = ((minutosEntrada + i) / 60);
            if (EHorarioDiurno(horaAtual, inicioDia, fimDia)) totalPagar += (precoCT1 / 60.0);
            else totalPagar += (precoCT2 / 60.0);
        }

        // B) Desde a meia-noite (00:00) até à saída
        long minDesdeMeiaNoite = minutosSaida;
        for (int i = 0; i < minDesdeMeiaNoite; i++) {
            int horaAtual = (i / 60);
            if (EHorarioDiurno(horaAtual, inicioDia, fimDia)) totalPagar += (precoCT1 / 60.0);
            else totalPagar += (precoCT2 / 60.0);
        }
    }

    return totalPagar;
}
