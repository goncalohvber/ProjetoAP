#include "Tarifas.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

int modificatarifa(Tarifa tarifas[], int *numtarifas) {
    float valortemp;
    char Codigoescolhido[5];
    printf("Qual o código associado à tarifa que quer modificar (CT1, CT2,...)?\n");
    printf("Código: ");
    scanf("%s", Codigoescolhido);
    for(int i=0;i<*numtarifas; i++) {
        if(strcmp(tarifas[i].codigo, Codigoescolhido) == 0){
            printf("Valor atual do %s: %.2f\n", Codigoescolhido, tarifas[i].valor);
            do{
                printf("Novo valor: ");
                scanf("%f", &valortemp);
                if (valortemp<=0) {
                    printf("O novo valor deve ser maior que 0.00");
                }
            }while(valortemp<=0);
            tarifas[i].valor = valortemp;
            if (guardartarifas(tarifas, *numtarifas)) {
                printf("Valor modificado com sucesso.");
                return 1;
            }
        }
    }
    printf("O código %s não existe no tarifário.", Codigoescolhido);
    return 0;
}


void mostrarTabela(Tarifa tarifas[], int n) {
    printf("\n------------ VALORES DEFINIDOS ------------\n");
    for (int i = 0; i < n; i++) {
        printf("%s\tValor: %.2f €\t(%02d:%02d - %02d:%02d)\t%c\n",
               tarifas[i].codigo,
               tarifas[i].valor,
               tarifas[i].horaInf, tarifas[i].minInf,
               tarifas[i].horaSup, tarifas[i].minSup,
               tarifas[i].tipo);
    }
    printf("------------------------------------------\n");}

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
            printf("⚠️  Linha mal formatada: '%s'\n", linha);
            continue;
        }
        
        if (tipo != 'H' && tipo != 'D') {
            printf("⚠️  Tipo '%c' inválido\n", tipo);
            continue;
        }
        
        if (valor <= 0) {
            printf("⚠️  Valor %.2f inválido\n", valor);
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

float ObterPreco(Tarifa tarifas[], int numTarifas, char *codigoProcurado) { //Função para retornar o valor de qualquer código
    for (int i = 0; i < numTarifas; i++) {
        if (strcmp(tarifas[i].codigo, codigoProcurado) == 0) {
            return tarifas[i].valor;
        }
    }
    return 0.0;
}

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
