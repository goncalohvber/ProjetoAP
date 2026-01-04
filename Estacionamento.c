#include <stdio.h>
#include <stdlib.h>
#include "Estacionamento.h"
#include "Listagens.h"
#include "validacoes.h"
#include "Tarifas.h"
#include "Util.h"
#include "GestaoLugares.h"
int validaData(int dia, int mes, int ano);
int validamatricula(char *mat);
int validaLugar(char *lugar, int maxPisos, char maxFila, int maxLugares);
int validaEantesS(int diaE, int mesE, int anoE, int horaE, int minE, int diaS, int mesS, int anoS, int horaS, int minS);
int ComparaDatas(int d1, int m1, int a1, int h1, int min1, int d2, int m2, int a2, int h2, int min2);
float CalcularPreco(int dE, int mE, int aE, int hE, int minE, int dS, int mS, int aS, int hS, int minS, Tarifa tarifas[], int numTarifas);
int lertarifas(Tarifa tarifas[], int *numtarifas);

// Função: Limpar e validar ficheiro de estacionamentos
void LimpaFicheiroEstac(char *ficheirobase, char *ficheirovalido, char *ficheiroerros, Confparque config) {
    
    // Abertura dos ficheiros: leitura do original e escrita para válidos e erros
    FILE *f_og = fopen(ficheirobase, "r");
    FILE *f_val = fopen(ficheirovalido, "w");
    FILE *f_err = fopen(ficheiroerros, "w");
    
    // Verificação de segurança na abertura dos ficheiros
    if (f_og == NULL || f_val == NULL || f_err == NULL) {
        printf("Erro critico ao abrir ficheiros.\n");
        if (f_og != NULL) fclose(f_og);
        if (f_val != NULL) fclose(f_val);
        if (f_err != NULL) fclose(f_err);
        return;
    }

    estacionamento E;
    int novoID = 1; // Carregamento da tabela de preços (tarifas) para cálculo posterior

    // Carregamento da tabela de preços (tarifas) para cálculo posterior
    Tarifa tarifas[MAX_TARIFAS];
    int numTarifas = 0;
    if (!lertarifas(tarifas, &numTarifas)) {
        printf("ERRO: Não foi possível carregar as tarifas!\n");
        fclose(f_og);
        fclose(f_val);
        fclose(f_err);
        return;
    }
    
    // Array para rastrear matrículas no parque
    char matriculasNoParque[MAX_REG_EST][10];
    int numMatriculasNoParque = 0;

    // Lê cada linha do ficheiro base até ao fim
    while (fscanf(f_og, "%d %s %d %d %d %d %d %s %d %d %d %d %d",
                  &E.numE, E.matricula,
                  &E.anoE, &E.mesE, &E.diaE, &E.horaE, &E.minE,
                  E.lugar,
                  &E.anoS, &E.mesS, &E.diaS, &E.horaS, &E.minS) == 13)
    {
        // Define o limite de filas
        char letraConvertida = 'A' + (config.numfilas - 1);
        // Validações
        int entradaValida = validaData(E.diaE, E.mesE, E.anoE);
        int saidaValida = validaData(E.diaS, E.mesS, E.anoS);
        int matriculaValida = validamatricula(E.matricula);
        int lugarvalido = validaLugar(E.lugar, config.numpisos, letraConvertida, config.numlugares);
        int tempovalido = validaEantesS(E.diaE, E.mesE, E.anoE, E.horaE, E.minE,
                                        E.diaS, E.mesS, E.anoS, E.horaS, E.minS);
        
        // Verificar se matrícula já está no parque
        int matriculaDuplicada = 0;
        if (E.anoS == 0) {
            for (int i = 0; i < numMatriculasNoParque; i++) {
                if (strcmp(matriculasNoParque[i], E.matricula) == 0) {
                    matriculaDuplicada = 1;
                    break;
                }
            }
        }
        
        // Se passar em todos os testes, o registo é considerado válido
        if (entradaValida == 1 && saidaValida == 1 && matriculaValida == 1 &&
            lugarvalido == 1 && tempovalido == 1 && !matriculaDuplicada) {

            // Calcula o custo total com base no tempo de permanência
            float precoPagar = CalcularPreco(E.diaE, E.mesE, E.anoE, E.horaE, E.minE,
                                           E.diaS, E.mesS, E.anoS, E.horaS, E.minS,
                                           tarifas, numTarifas);

            // Escreve no ficheiro de válidos com o novo ID e o preço calculado
            fprintf(f_val, "%d %s %d %d %d %d %d %s %d %d %d %d %d %.2f\n",
                    novoID, E.matricula,
                    E.anoE, E.mesE, E.diaE, E.horaE, E.minE,
                    E.lugar,
                    E.anoS, E.mesS, E.diaS, E.horaS, E.minS,
                    precoPagar);
            
            // Regista a matrícula na lista de veículos ativos no parque
            if (E.anoS == 0 && numMatriculasNoParque < MAX_REG_EST) {
                strcpy(matriculasNoParque[numMatriculasNoParque], E.matricula);
                numMatriculasNoParque++;
            } 
            novoID++;
        }
        
        // Se falhar em qualquer validação, regista o motivo específico no ficheiro de erros    
        else if(entradaValida != 1) {
            fprintf(f_err, "[ERRO] Linha %d | Entrada: %d/%d/%d (Data de entrada não existe)\n",
                    E.numE, E.diaE, E.mesE, E.anoE);
        }
        else if(saidaValida != 1){
            fprintf(f_err, "[ERRO] Linha %d | Entrada: %d/%d/%d (Data de saída não existe)\n",
                    E.numE, E.diaS, E.mesS, E.anoS);
        }
        else if(matriculaValida != 1){
            fprintf(f_err, "[ERRO] Linha %d | Matricula: %s (Matricula não valida)\n",
                    E.numE, E.matricula);
        }
        else if(lugarvalido != 1){
            fprintf(f_err, "[ERRO] Linha %d | Lugar: %s (Lugar não compativel com o tamanho do estacionamento)\n",
                    E.numE, E.lugar);
        }
        else if(tempovalido != 1){
            fprintf(f_err, "[ERRO] Linha %d | A Data de entrada é posterior à Data de saida. (Impossível)\n",
                    E.numE);
        }
        else if(matriculaDuplicada){
            fprintf(f_err, "[ERRO] Linha %d | Matricula: %s (Veículo já está no parque - entrada duplicada)\n",
                    E.numE, E.matricula);
        }
    }
    fclose(f_og);
    fclose(f_val);
    fclose(f_err);
}


// Função: Mostrar ficheiro de estacionamento
void MostrarFicheiroEstacionamento(char *nomeFicheiro)
{
    // Tenta abrir o ficheiro
    FILE *f = fopen(nomeFicheiro, "r");

    // Verifica se o ficheiro existe ou se pode ser aberto
    if (f == NULL) {
        printf("Erro: Nao foi possivel abrir o ficheiro %s\n", nomeFicheiro);
        return;
    }
    estacionamento E;
    float precoPagar;
    
    printf("\n--- REGISTO DE ESTACIONAMENTOS ---\n");

    //Loop de leitura: o fscanf procura 14 campos específicos e continua enquanto o ficheiro devolver exatamente os 14 itens por linha
    while (fscanf(f, "%d %s %d %d %d %d %d %s %d %d %d %d %d %f",
                  &E.numE,
                  E.matricula,
                  &E.anoE, &E.mesE, &E.diaE, &E.horaE, &E.minE,
                  E.lugar,
                  &E.anoS, &E.mesS, &E.diaS, &E.horaS, &E.minS,
                  &precoPagar) == 14)
    {
        printf("Num: %-4d | Mat: %-8s | Lugar: %-4s | Ent: %02d/%02d/%d %02d:%02d | Sai: %02d/%02d/%d %02d:%02d | Preço: %.2f\n",
               E.numE,
               E.matricula,
               E.lugar,
               E.diaE, E.mesE, E.anoE, E.horaE, E.minE,
               E.diaS, E.mesS, E.anoS, E.horaS, E.minS,
               precoPagar);
    }
    fclose(f);
}

// Função: Gerar ficheiro de ocupação
void gerarficheiroocupacao(char *ficheirovalido, char *ficheiroocupacao,
                           int diaU, int mesU, int anoU, int horaU, int minU) {
    
    // Abre o ficheiro de registos validados para leitura e cria o ficheiro de ocupação
    FILE *f_val = fopen(ficheirovalido, "r");
    FILE *f_ocup = fopen(ficheiroocupacao, "w");

    // Verificação de erro na abertura dos ficheiros
    if (f_val == NULL || f_ocup == NULL) {
        printf("Erro ao abrir ficheiros para gerar ocupacao.\n");
        if (f_val) fclose(f_val);
        if (f_ocup) fclose(f_ocup);
        return;
    }
        
    estacionamento E;   // Contador para o novo ID sequencial no ficheiro de ocupação
    int novonum = 1;    // Variável auxiliar para ler o preço (que não será usado aqui)
    float preco;

    // Lê todos os registos do ficheiro de veículos validados
    while (fscanf(f_val, "%d %s %d %d %d %d %d %s %d %d %d %d %d %f",
                  &E.numE, E.matricula,
                  &E.anoE, &E.mesE, &E.diaE, &E.horaE, &E.minE,
                  E.lugar,
                  &E.anoS, &E.mesS, &E.diaS, &E.horaS, &E.minS,
                  &preco) == 14) {
        
        // Verificar se entrou antes ou na data/hora consultada
        int entrouAntesOuIgual = ComparaDatas(E.diaE, E.mesE, E.anoE, E.horaE, E.minE,
                                               diaU, mesU, anoU, horaU, minU) <= 0;
        
        // Se ainda não saiu (anoS = 0), considerar que está no parque
        int aindaNoParque = (E.anoS == 0);
        
        // Se saiu, verificar se foi depois da data consultada
        int saiuDepoisOuIgual = 0;
        if (!aindaNoParque) {
            saiuDepoisOuIgual = ComparaDatas(E.diaS, E.mesS, E.anoS, E.horaS, E.minS,
                                             diaU, mesU, anoU, horaU, minU) >= 0;
        }

        // Está ocupado se: entrou antes E (ainda não saiu OU saiu depois)
        if (entrouAntesOuIgual && (aindaNoParque || saiuDepoisOuIgual)) {
            fprintf(f_ocup, "%d %s %d %d %d %d %d %s %d %d %d %d %d\n",
                    novonum, E.matricula,
                    E.anoE, E.mesE, E.diaE, E.horaE, E.minE,
                    E.lugar,
                    E.anoS, E.mesS, E.diaS, E.horaS, E.minS);
            novonum++;
        }
    }
    fclose(f_val);
    fclose(f_ocup);
    printf("DEBUG: %d veículos ocupando lugares no momento consultado.\n", novonum - 1);
}

// Função: Verificar se carro já está no parque
int verificarCarroNoParque(char *matricula, char *ficheiroEstacionamentos) {
    // Abre o ficheiro de registos para leitura
    FILE *f = fopen(ficheiroEstacionamentos, "r");
    
    // Se o ficheiro não existir (ex: primeiro carro do dia), assume que não está no parque
    if (f == NULL) {
        return 0;
    }
    estacionamento E;
    
    // Lê o ficheiro linha a linha (esperando 13 campos, formato sem preço)
    while (fscanf(f, "%d %s %d %d %d %d %d %s %d %d %d %d %d",
                  &E.numE, E.matricula,
                  &E.anoE, &E.mesE, &E.diaE, &E.horaE, &E.minE,
                  E.lugar,
                  &E.anoS, &E.mesS, &E.diaS, &E.horaS, &E.minS) == 13)
    {
        // A matrícula lida é igual à matrícula que procuramos (strcmp == 0) e o ano de saída é 0 (indicando que o carro ainda não registou a saída)
        if (strcmp(E.matricula, matricula) == 0 && E.anoS == 0) {
            fclose(f); // Fecha o ficheiro antes de retornar para evitar fugas de memória
            return 1;  // Encontrou o carro ainda estacionado
        }
    }
    fclose(f);
    return 0;
}

// Função: Obter próximo número de entrada 
int obterProximoNumeroEntrada(char *ficheiroEstacionamentos) {

    // Tenta abrir o ficheiro para leitura
    FILE *f = fopen(ficheiroEstacionamentos, "r");

    // Se o ficheiro não existir, significa que este será o primeiro registo
    if (f == NULL) {
        return 1;
    }
    
    int ultimoNum = 0;
    estacionamento E;

    // Percorre todo o ficheiro lendo os registos, o fscanf tenta ler os 13 campos que compõem a estrutura no ficheiro base.
    while (fscanf(f, "%d %s %d %d %d %d %d %s %d %d %d %d %d",
                  &E.numE, E.matricula,
                  &E.anoE, &E.mesE, &E.diaE, &E.horaE, &E.minE,
                  E.lugar,
                  &E.anoS, &E.mesS, &E.diaS, &E.horaS, &E.minS) == 13) {

        // Atualiza ultimoNum sempre que encontrar um ID maior, garante que o ID final seja o maior de todos, mesmo que o ficheiro não esteja ordenado
        if (E.numE > ultimoNum) {
            ultimoNum = E.numE;
        }
    }
    fclose(f);
    
    // Retorna o maior número encontrado acrescido de uma unidade
    return ultimoNum + 1;
}

// Função: Obter próximo número validado
int obterProximoNumeroValidado(char *ficheiroValidado) {
    
    FILE *f = fopen(ficheiroValidado, "r");
    
    // Se o ficheiro não existe, este será o primeiro registo validado
    if (f == NULL) {
        return 1;
    }

    int ultimoNumValidado = 0;
    int numLido;
    char matricula[MAX_MATRICULA];
    int anoE, mesE, diaE, horaE, minE;
    char lugar[MAX_LUGAR];
    int anoS, mesS, diaS, horaS, minS;
    float preco;

    // Loop de leitura: nota que aqui o fscanf procura 14 valores. É essencial ler o float 'preco' no final, caso contrário o ponteiro  do ficheiro ficaria desalinhado para a próxima linha.
    while (fscanf(f, "%d %s %d %d %d %d %d %s %d %d %d %d %d %f",
                  &numLido, matricula,
                  &anoE, &mesE, &diaE, &horaE, &minE,
                  lugar,
                  &anoS, &mesS, &diaS, &horaS, &minS,
                  &preco) == 14)
    {
        // Verifica se o ID lido é o maior encontrado até agora
        if (numLido > ultimoNumValidado) {
            ultimoNumValidado = numLido;
        }
    }
    
    fclose(f);
    
    // Retorna o sucessor do maior número encontrado
    return ultimoNumValidado + 1;
}

// Função: Processar string de lugar, decompõe a string do lugar nos seus componentes numéricos.
void ProcessarLugarLocal(char *lugarStr, int *piso, int *fila, int *numero) {

    // Converte o primeiro caractere (ASCII) para o valor numérico real e subtrai '0' transforma o char '1' (valor 49 na tabela ASCII) no int 1.
    *piso = lugarStr[0] - '0';

    // Converte a letra da fila para um índice base zero (A=0, B=1...) e subtrai 'A' retira o valor base da letra, facilitando o uso em matrizes ou intervalos.
    *fila = lugarStr[1] - 'A';

    // Converte o restante da string (do terceiro caractere em diante) para um número inteiro e o operador '&' com o índice [2] passa o endereço onde começa o número na string.
    *numero = atoi(&lugarStr[2]);
}

// Função: Verificar se lugar está ocupado numa data
int lugarOcupadoNaData(char *lugar, int diaCheck, int mesCheck, int anoCheck,
                       int horaCheck, int minCheck, char *ficheiroEstacionamentos) {
    FILE *f = fopen(ficheiroEstacionamentos, "r");
    if (f == NULL) {
        return 0; // Se o ficheiro não existe, o lugar está tecnicamente livre
    }
    estacionamento E;
    while (fscanf(f, "%d %s %d %d %d %d %d %s %d %d %d %d %d",
                  &E.numE, E.matricula,
                  &E.anoE, &E.mesE, &E.diaE, &E.horaE, &E.minE,
                  E.lugar,
                  &E.anoS, &E.mesS, &E.diaS, &E.horaS, &E.minS) == 13)
    {
        // Se o registo lido não for sobre o lugar que estamos a verificar, ignora e passa ao próximo
        if (strcmp(E.lugar, lugar) != 0) {
            continue;
        }
        
        // Se ainda não saiu (anoS = 0), está ocupado de entrada até "infinito"
        if (E.anoS == 0) {
            int resultado = ComparaDatas(diaCheck, mesCheck, anoCheck, horaCheck, minCheck,
                                        E.diaE, E.mesE, E.anoE, E.horaE, E.minE);
            
            if (resultado >= 0) {
                fclose(f);
                return 1;
            }
        } else {
            // Verifica se a consulta é posterior ou igual à entrada
            int depoisEntrada = ComparaDatas(diaCheck, mesCheck, anoCheck, horaCheck, minCheck,
                                            E.diaE, E.mesE, E.anoE, E.horaE, E.minE) >= 0;
            
            // Verifica se a consulta é anterior ou igual à saída
            int antesSaida = ComparaDatas(diaCheck, mesCheck, anoCheck, horaCheck, minCheck,
                                         E.diaS, E.mesS, E.anoS, E.horaS, E.minS) <= 0;

            // Se estiver dentro do intervalo, o lugar estava ocupado nesse momento
            if (depoisEntrada && antesSaida) {
                fclose(f);
                return 1; // Ocupado
            }
        }
    }
    
    fclose(f);
    return 0;
}

// Função: Atribuir lugar automático
char* atribuirLugar(Confparque config, char *ficheiroEstacionamentos) {
    // Criar array para marcar lugares ocupados
    static char lugarAtribuido[10];
    int ocupados[MAX_PISOS][MAX_FILAS][MAX_LUGARES] = {0};
    
    // Ler ficheiro e marcar lugares ocupados
    FILE *f = fopen(ficheiroEstacionamentos, "r");
    if (f != NULL) {
        estacionamento E;
        
        // Ler apenas 13 campos (SEM preço)
        while (fscanf(f, "%d %s %d %d %d %d %d %s %d %d %d %d %d",
                      &E.numE, E.matricula,
                      &E.anoE, &E.mesE, &E.diaE, &E.horaE, &E.minE,
                      E.lugar,
                      &E.anoS, &E.mesS, &E.diaS, &E.horaS, &E.minS) == 13)
        {
            // Se ainda não saiu (anoS = 0), marcar como ocupado
            if (E.anoS == 0) {
                int piso, fila, numero;
                ProcessarLugar(E.lugar, &piso, &fila, &numero);
                ocupados[piso-1][fila][numero-1] = 1;
            }
        }
        fclose(f);
    }
    
    // Procurar primeiro lugar livre (ordem: piso -> fila -> número)
    for (int p = 0; p < config.numpisos; p++) {
        for (int f = 0; f < config.numfilas; f++) {
            for (int l = 0; l < config.numlugares; l++) {
                if (ocupados[p][f][l] == 0) {
                    
                    // Encontrou lugar livre
                    sprintf(lugarAtribuido, "%d%c%d",
                            p+1,           // Piso (1-5)
                            'A' + f,       // Fila (A-Z)
                            l+1);          // Número (1-50)
                    return lugarAtribuido;
                }
            }
        }
    }
    
    // Sem lugares livres
    return NULL;
}

// Função: Atribuir lugar numa data específica
char* atribuirLugarNaData(Confparque config, char *ficheiroEstacionamentos,
                          int diaEntrada, int mesEntrada, int anoEntrada,
                          int horaEntrada, int minEntrada) {
    
    static char lugarAtribuido[10];
    
    // Carregar lugares indisponíveis
    Lugar mapa[MAX_PISOS][MAX_FILAS][MAX_LUGARES];
    InicializarMapa(mapa, config);
    carregarLugaresIndisponiveis(mapa, config);
    
    // Procurar primeiro lugar livre NESSA DATA
    for (int p = 0; p < config.numpisos; p++) {
        for (int f = 0; f < config.numfilas; f++) {
            for (int l = 0; l < config.numlugares; l++) {
                sprintf(lugarAtribuido, "%d%c%02d", p+1, 'A' + f, l+1);
                
                // Verificar se está indisponível
                char statusIndisponivel = mapa[p][f][l].status;
                if (statusIndisponivel == 'i' || statusIndisponivel == 'o' ||
                    statusIndisponivel == 'r' || statusIndisponivel == 'm') {
                    continue; // Pular lugares indisponíveis
                }
                
                int ocupado = lugarOcupadoNaData(lugarAtribuido,
                                                 diaEntrada, mesEntrada, anoEntrada,
                                                 horaEntrada, minEntrada,
                                                 ficheiroEstacionamentos);
                
                if (!ocupado) {
                    return lugarAtribuido;
                }
            }
        }
    }
    
    return NULL;
}

// Função: Registar Entrada
int registarEntrada(Confparque config, char *ficheiroEstacionamentos) {
    estacionamento novoEstac;
    char matriculaTemp[10];
    int dia, mes, ano, hora, min;
    int carroJaNoParque = 0;
    
    printf("\n╔═══════════════════════════════════════════════════════════╗\n");
    printf("║               REGISTAR ENTRADA DE VEÍCULO                 ║\n");
    printf("╚═══════════════════════════════════════════════════════════╝\n\n");
    
    // Pedir data e hora
    do {
        printf(" Data de entrada (DD MM AAAA): ");
        
        // Limpar o buffer antes de ler
        int resultado = scanf("%d %d %d", &dia, &mes, &ano);
        
        // Se scanf falhou (retornou menos de 3 valores)
        if (resultado != 3) {
            printf(" Entrada inválida! Digite apenas números.\n");
            
            // Limpar o buffer
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
            
            continue; // Voltar ao início do loop
        }
        
        if (!validaData(dia, mes, ano)) {
            printf(" Data inválida! Tente novamente.\n");
        }
    } while (!validaData(dia, mes, ano));
    
    do {
        printf(" Hora de entrada (HH MM): ");
        
        int resultado = scanf("%d %d", &hora, &min);
        
        if (resultado != 2) {
            printf(" Entrada inválida! Digite apenas números.\n\n");
            
            // Limpar o buffer
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
            
            continue;
        }
        
        if (hora < 0 || hora > 23 || min < 0 || min > 59) {
            printf(" Hora inválida! Tente novamente.\n\n");
        }
    } while (hora < 0 || hora > 23 || min < 0 || min > 59);
    
    // Pedir matrícula e verificar se já está no parque nessa data
    do {
        printf(" Matrícula do veículo (XX-XX-XX): ");
        scanf("%s", matriculaTemp);
        
        if (!validamatricula(matriculaTemp)) {
            printf(" Matrícula inválida! Formato correto: XX-XX-XX\n\n");
            continue;
        }
        
        FILE *f = fopen(ficheiroEstacionamentos, "r");
        carroJaNoParque = 0;
        
        if (f != NULL) {
            estacionamento E;
            
            while (fscanf(f, "%d %s %d %d %d %d %d %s %d %d %d %d %d",
                          &E.numE, E.matricula,
                          &E.anoE, &E.mesE, &E.diaE, &E.horaE, &E.minE,
                          E.lugar,
                          &E.anoS, &E.mesS, &E.diaS, &E.horaS, &E.minS) == 13)
            {
                if (strcmp(E.matricula, matriculaTemp) == 0) {
                    if (E.anoS == 0) {
                        int dataNovaDepoisExistente = ComparaDatas(dia, mes, ano, hora, min,
                                                                   E.diaE, E.mesE, E.anoE, E.horaE, E.minE) >= 0;
                        if (dataNovaDepoisExistente) {
                            carroJaNoParque = 1;
                            break;
                        }
                    } else {
                        int novaEntradaDurantePeriodo =
                            (ComparaDatas(dia, mes, ano, hora, min, E.diaE, E.mesE, E.anoE, E.horaE, E.minE) >= 0) &&
                            (ComparaDatas(dia, mes, ano, hora, min, E.diaS, E.mesS, E.anoS, E.horaS, E.minS) <= 0);
                        
                        if (novaEntradaDurantePeriodo) {
                            carroJaNoParque = 1;
                            break;
                        }
                    }
                }
            }
            fclose(f);
        }
        
        if (carroJaNoParque) {
            printf(" Erro: O veículo %s já estará no parque nessa data/hora!\n", matriculaTemp);
            printf(" Escolha outra data ou matrícula.\n\n");
        }
        
    } while (!validamatricula(matriculaTemp) || carroJaNoParque);
    
    // Atribuir lugar para essa data
    char *lugarAtribuido = atribuirLugarNaData(config, "estacionamentos.txt",
                                               dia, mes, ano, hora, min);
    
    if (lugarAtribuido == NULL) {
        printf("\n ERRO: Não há lugares disponíveis no parque nessa data!\n");
        return 0;
    }
    
    // Obter números
    novoEstac.numE = obterProximoNumeroEntrada("estacionamentos.txt");
    novoEstac.numValidado = obterProximoNumeroValidado("estacionamentos_validos.txt");
    
    // Preencher estrutura
    strcpy(novoEstac.matricula, matriculaTemp);
    novoEstac.anoE = ano;
    novoEstac.mesE = mes;
    novoEstac.diaE = dia;
    novoEstac.horaE = hora;
    novoEstac.minE = min;
    strcpy(novoEstac.lugar, lugarAtribuido);
    
    novoEstac.anoS = 0;
    novoEstac.mesS = 0;
    novoEstac.diaS = 0;
    novoEstac.horaS = 0;
    novoEstac.minS = 0;
    
    // Gravar no ficheiro base
    FILE *f = fopen("estacionamentos.txt", "a");
    
    if (f == NULL) {
        printf("ERRO: Não foi possível abrir o ficheiro!\n");
        return 0;
    }
    
    fprintf(f, "%d\t%s\t%d\t%d\t%d\t%d\t%d\t%s\t%d\t%d\t%d\t%d\t%d\n",
            novoEstac.numE,
            novoEstac.matricula,
            novoEstac.anoE, novoEstac.mesE, novoEstac.diaE,
            novoEstac.horaE, novoEstac.minE,
            novoEstac.lugar,
            novoEstac.anoS, novoEstac.mesS, novoEstac.diaS,
            novoEstac.horaS, novoEstac.minS);
    
    fclose(f);
    
    // Gravar no ficheiro validado
    f = fopen("estacionamentos_validos.txt", "a");
    
    if (f == NULL) {
        printf("ERRO: Não foi possível abrir o ficheiro validado!\n");
        return 0;
    }
    
    fprintf(f, "%d %s %d %d %d %d %d %s %d %d %d %d %d %.2f\n",
            novoEstac.numValidado,
            novoEstac.matricula,
            novoEstac.anoE, novoEstac.mesE, novoEstac.diaE,
            novoEstac.horaE, novoEstac.minE,
            novoEstac.lugar,
            novoEstac.anoS, novoEstac.mesS, novoEstac.diaS,
            novoEstac.horaS, novoEstac.minS,
            0.00);
    
    fclose(f);
    
    // Mostrar ticket
    printf("\n  Entrada registada com sucesso!\n");
    mostrarTicket(novoEstac);
    
    return 1;
}

// FUNÇÕES DO MAPA DE OCUPAÇÃO

// Função: Inicializa a matriz tridimensional que representa o parque de estacionamento
void InicializarMapa(Lugar mapa[][MAX_FILAS][MAX_LUGARES], Confparque config) {

    // Percorre cada nível (piso) do parque
    for (int piso = 0; piso < config.numpisos; piso++) {

        // Em cada piso, percorre as filas (A, B, C...)
        for (int fila = 0; fila < config.numfilas; fila++) {

            // Em cada fila, percorre os lugares individuais (1, 2, 3...)
            for (int lugar = 0; lugar < config.numlugares; lugar++) {

                // Define o estado visual como livre (usando o caractere hífen)
                mapa[piso][fila][lugar].status = '-';

                // Limpa a string da matrícula para garantir que não contenha dados residuais
                strcpy(mapa[piso][fila][lugar].matricula, "");
            }
        }
    }
}

// Função: Descodifica uma string de localização (ex: "1A05") em índices numéricos.
void ProcessarLugar(char *lugarStr, int *piso, int *fila, int *numero) {

    // Converte o caractere do piso para inteiro (ex: '1' vira 1)
    *piso = lugarStr[0] - '0';
    
    // Converte a letra da fila para um índice numérico (ex: 'A' vira 0, 'B' vira 1)
    *fila = lugarStr[1] - 'A';

    // Converte o restante da string em número inteiro usando atoi
    *numero = atoi(&lugarStr[2]);
}

// Função: Lê o ficheiro de ocupação e preenche a matriz do mapa com os veículos presentes
void PreencherMapaComOcupacoes(Lugar mapa[][MAX_FILAS][MAX_LUGARES], char *ficheiroOcupacao) {
    FILE *f = fopen(ficheiroOcupacao, "r");

    // Tratamento de erro caso o ficheiro não exista (parque vazio ou erro de geração)
    if (f == NULL) {
        printf("  Aviso: Ficheiro de ocupação não encontrado ou vazio.\n");
        return;
    }
    
    estacionamento E;
    int ocupados = 0;

    // Lê cada registo do ficheiro de ocupação (formato de 13 campos)
    while (fscanf(f, "%d %s %d %d %d %d %d %s %d %d %d %d %d",
                  &E.numE, E.matricula,
                  &E.anoE, &E.mesE, &E.diaE, &E.horaE, &E.minE,
                  E.lugar,
                  &E.anoS, &E.mesS, &E.diaS, &E.horaS, &E.minS) == 13)
    {
        int piso, fila, numero;

        // Converte a string para coordenadas numéricas
        ProcessarLugar(E.lugar, &piso, &fila, &numero);

        // Se o utilizador introduziu Piso 1, na matriz C isso corresponde ao índice 0. O mesmo acontece para o número do lugar.
        piso--;
        numero--;

        // Verificação de segurança: garante que as coordenadas estão dentro dos limites da matriz
        if (piso >= 0 && piso < MAX_PISOS &&
            fila >= 0 && fila < MAX_FILAS &&
            numero >= 0 && numero < MAX_LUGARES) {
            
            // Obtém o estado atual do lugar para evitar sobrepor lugares especiais
            char statusAtual = mapa[piso][fila][numero].status;

            // Marca com 'X' (Ocupado) se o lugar não estiver marcado como indisponível
            if (statusAtual != 'i' && statusAtual != 'o' &&
                statusAtual != 'r' && statusAtual != 'm') {
                mapa[piso][fila][numero].status = 'X';
                strcpy(mapa[piso][fila][numero].matricula, E.matricula);
                ocupados++;
            }
        }
    }
    
    fclose(f);
    printf("Total de lugares ocupados: %d\n\n", ocupados);
}

void DesenharMapa(Lugar mapa[][MAX_FILAS][MAX_LUGARES], Confparque config) {
    
    for (int piso = 0; piso < config.numpisos; piso++) {
        printf("\n╔═══════════════════════════════════════════════════════════╗\n");
        printf("║                      PISO %d                               ║\n", piso + 1);
        printf("╚═══════════════════════════════════════════════════════════╝\n\n");
        
        // Cabeçalho com números dos lugares
        printf("    ");
        for (int lugar = 0; lugar < config.numlugares; lugar++) {
            printf("%2d ", lugar + 1);
        }
        printf("\n");
        
        printf("    ");
        for (int lugar = 0; lugar < config.numlugares; lugar++) {
            printf("───");
        }
        printf("\n");
        
        // Cada fila
        for (int fila = 0; fila < config.numfilas; fila++) {
            char letraFila = 'A' + fila;
            printf(" %c │ ", letraFila);
            
            for (int lugar = 0; lugar < config.numlugares; lugar++) {
                char status = mapa[piso][fila][lugar].status;
                printf("%c  ", status);
            }
            printf("\n");
        }
        printf("\n");
    }
    
    // Legenda atualizada
    printf("╔═══════════════════════════════════════════════════════════╗\n");
    printf("║                        LEGENDA                            ║\n");
    printf("╠═══════════════════════════════════════════════════════════╣\n");
    printf("║  -  = Lugar Livre                                         ║\n");
    printf("║  X  = Lugar Ocupado                                       ║\n");
    printf("║  i  = Condições inadequadas                               ║\n");
    printf("║  o  = Objeto de obras                                     ║\n");
    printf("║  r  = Reservado                                           ║\n");
    printf("║  m  = Outros motivos                                      ║\n");
    printf("╚═══════════════════════════════════════════════════════════╝\n");
}

// Função principal que junta tudo
void MostrarMapaOcupacao_ComMapa(Confparque config, char *ficheiroOcupacao,
                                 Lugar mapa[][MAX_FILAS][MAX_LUGARES]) {
    
    
    printf("\n MAPA DE OCUPAÇÃO DO PARQUE\n");
    printf("═══════════════════════════════════════════\n");
    
    // Inicializar tudo como livre
    InicializarMapa(mapa, config);
    
    //  Carregar lugares indisponíveis primeiro
    carregarLugaresIndisponiveis(mapa, config);
    
    // Ler ficheiro e marcar ocupados (sem sobrescrever indisponíveis)
    PreencherMapaComOcupacoes(mapa, ficheiroOcupacao);
    
    // Desenhar o mapa
    DesenharMapa(mapa, config);
    
    // Estatísticas
    int totalLugares = config.numpisos * config.numfilas * config.numlugares;
    int ocupados = 0;
    int indisponiveis = 0;
    
    for (int p = 0; p < config.numpisos; p++) {
        for (int f = 0; f < config.numfilas; f++) {
            for (int l = 0; l < config.numlugares; l++) {
                char status = mapa[p][f][l].status;
                if (status == 'X') {
                    ocupados++;
                } else if (status == 'i' || status == 'o' ||
                          status == 'r' || status == 'm') {
                    indisponiveis++;
                }
            }
        }
    }
    
    int livres = totalLugares - ocupados - indisponiveis;
    float percentagem = (ocupados * 100.0) / totalLugares;
    
    printf("\n ESTATÍSTICAS:\n");
    printf("   Total de lugares: %d\n", totalLugares);
    printf("   Lugares ocupados: %d\n", ocupados);
    printf("   Lugares indisponíveis: %d\n", indisponiveis);
    printf("   Lugares livres: %d\n", livres);
    printf("   Taxa de ocupação: %.1f%%\n", percentagem);
}


void mostrarTicket(estacionamento E) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════╗\n");
    printf("║                 TICKET DE ESTACIONAMENTO                  ║\n");
    printf("╠═══════════════════════════════════════════════════════════╣\n");
    printf("║                                                           ║\n");
    printf("║  Nº Entrada: %-6d                                         ║\n", E.numValidado);
    printf("║  Matrícula:  %-10s                                        ║\n", E.matricula);
    printf("║                                                           ║\n");
    printf("║  Lugar Atribuído: %-5s                                    ║\n", E.lugar);
    printf("║                                                           ║\n");
    printf("║  Data Entrada: %02d/%02d/%d                               ║\n", E.diaE, E.mesE, E.anoE);
    printf("║  Hora Entrada: %02d:%02d                                  ║\n", E.horaE, E.minE);
    printf("║                                                           ║\n");
    printf("║  Guarde este ticket para efetuar o pagamento!             ║\n");
    printf("║                                                           ║\n");
    printf("╚═══════════════════════════════════════════════════════════╝\n");
}


int registarSaida(Confparque config, char *ficheiroEstacionamentos) {
    char matriculaProcurada[10];
    int dia, mes, ano, hora, min;
    int encontrado = 0;
    
    printf("\n╔═══════════════════════════════════════════════════════════╗\n");
    printf("║                 REGISTAR SAÍDA DE VEÍCULO                 ║\n");
    printf("╚═══════════════════════════════════════════════════════════╝\n\n");
    
    // Pedir matrícula 
    do {
        printf(" Matrícula do veículo (XX-XX-XX): ");
        scanf("%s", matriculaProcurada);
        
        if (!validamatricula(matriculaProcurada)) {
            printf("Matrícula inválida! Formato correto: XX-XX-XX\n\n");
        }
    } while (!validamatricula(matriculaProcurada));
    
    // Verificar se está no parque 
    FILE *f = fopen("estacionamentos.txt", "r");
    if (f == NULL) {
        printf("Erro: Não foi possível abrir o ficheiro!\n");
        return 0;
    }
    estacionamento E;
    while (fscanf(f, "%d %s %d %d %d %d %d %s %d %d %d %d %d",
                  &E.numE, E.matricula,
                  &E.anoE, &E.mesE, &E.diaE, &E.horaE, &E.minE,
                  E.lugar,
                  &E.anoS, &E.mesS, &E.diaS, &E.horaS, &E.minS) == 13)
    {
        if (strcmp(E.matricula, matriculaProcurada) == 0 && E.anoS == 0) {
            encontrado = 1;
            break;
        }
    }
    fclose(f);
    
    if (!encontrado) {
        printf("\n Erro: O veículo %s não se encontra no parque!\n", matriculaProcurada);
        printf("   Verifique se a matrícula está correta.\n");
        return 0;
    }
    
    // Ler do ficheiro validado
    int numValidadoEncontrado = 0;
    FILE *f_val = fopen("estacionamentos_validos.txt", "r");
    if (f_val != NULL) {
        estacionamento EVal;
        float precoTemp;
        
        while (fscanf(f_val, "%d %s %d %d %d %d %d %s %d %d %d %d %d %f",
                      &EVal.numValidado, EVal.matricula,
                      &EVal.anoE, &EVal.mesE, &EVal.diaE, &EVal.horaE, &EVal.minE,
                      EVal.lugar,
                      &EVal.anoS, &EVal.mesS, &EVal.diaS, &EVal.horaS, &EVal.minS,
                      &precoTemp) == 14)
        {
            if (strcmp(EVal.matricula, matriculaProcurada) == 0 && EVal.anoS == 0) {
                E.numValidado = EVal.numValidado;
                numValidadoEncontrado = 1;
                break;
            }
        }
        fclose(f_val);
    }
    
    // Se não encontrou no validado, usar o próximo número
    if (!numValidadoEncontrado) {
        E.numValidado = obterProximoNumeroValidado("estacionamentos_validos.txt");
    }
    
    // ========== PASSO 3: MOSTRAR INFO DA ENTRADA ==========
    printf("\n Veículo encontrado no parque!\n");
    printf("┌─────────────────────────────────────────────┐\n");
    printf("│ INFORMAÇÃO DO ESTACIONAMENTO                │\n");
    printf("├─────────────────────────────────────────────┤\n");
    printf("│ Nº Entrada: %-6d                            │\n", E.numValidado);
    printf("│ Matrícula:  %-10s                           │\n", E.matricula);
    printf("│ Lugar:      %-5s                            │\n", E.lugar);
    printf("│ Entrada:    %02d/%02d/%d às %02d:%02d       │\n", E.diaE, E.mesE, E.anoE, E.horaE, E.minE);
    printf("└─────────────────────────────────────────────┘\n\n");
    
    // Pedir data e hora de saída 
    do {
        printf("Data de saída (DD MM AAAA): ");
        scanf("%d %d %d", &dia, &mes, &ano);
        if (!validaData(dia, mes, ano)) {
            printf("Data inválida! Tente novamente.\n\n");
        }
    } while (!validaData(dia, mes, ano));
    
    do {
        printf("Hora de saída (HH MM): ");
        scanf("%d %d", &hora, &min);
        
        if (hora < 0 || hora > 23 || min < 0 || min > 59) {
            printf("Hora inválida! Tente novamente.\n\n");
        }
    } while (hora < 0 || hora > 23 || min < 0 || min > 59);
    
    // Validar que saída é depois da entrada
    if (!validaEantesS(E.diaE, E.mesE, E.anoE, E.horaE, E.minE,
                       dia, mes, ano, hora, min)) {
        printf("\n Erro: A data/hora de saída deve ser posterior à entrada!\n");
        printf("   Entrada: %02d/%02d/%d às %02d:%02d\n", E.diaE, E.mesE, E.anoE, E.horaE, E.minE);
        printf("   Saída:   %02d/%02d/%d às %02d:%02d\n", dia, mes, ano, hora, min);
        return 0;
    }
    
    // Atualizar o ficheiro 
    FILE *f_temp = fopen("temp_estacionamentos.txt", "w");
    if (f_temp == NULL) {
        printf("Erro: Não foi possível criar ficheiro temporário!\n");
        return 0;
    }
    
    f = fopen("estacionamentos.txt", "r");
    if (f == NULL) {
        printf("Erro: Não foi possível reabrir o ficheiro!\n");
        fclose(f_temp);
        return 0;
    }
    
    // Copiar todos os registos, atualizando o correto
    while (fscanf(f, "%d %s %d %d %d %d %d %s %d %d %d %d %d",
                  &E.numE, E.matricula,
                  &E.anoE, &E.mesE, &E.diaE, &E.horaE, &E.minE,
                  E.lugar,
                  &E.anoS, &E.mesS, &E.diaS, &E.horaS, &E.minS) == 13)
    {
        if (strcmp(E.matricula, matriculaProcurada) == 0 && E.anoS == 0) {
            fprintf(f_temp, "%d\t%s\t%d\t%d\t%d\t%d\t%d\t%s\t%d\t%d\t%d\t%d\t%d\n",
                    E.numE, E.matricula,
                    E.anoE, E.mesE, E.diaE, E.horaE, E.minE,
                    E.lugar,
                    ano, mes, dia, hora, min);
        } else {
            fprintf(f_temp, "%d\t%s\t%d\t%d\t%d\t%d\t%d\t%s\t%d\t%d\t%d\t%d\t%d\n",
                    E.numE, E.matricula,
                    E.anoE, E.mesE, E.diaE, E.horaE, E.minE,
                    E.lugar,
                    E.anoS, E.mesS, E.diaS, E.horaS, E.minS);
        }
    }
    
    fclose(f);
    fclose(f_temp);
    remove("estacionamentos.txt");
    rename("temp_estacionamentos.txt", "estacionamentos.txt");
    
    // Calcular preço
    Tarifa tarifas[MAX_TARIFAS];
    int numTarifas = 0;
    
    if (!lertarifas(tarifas, &numTarifas)) {
        printf("Erro: Não foi possível carregar as tarifas!\n");
        return 0;
    }
    float precoPagar = CalcularPreco(E.diaE, E.mesE, E.anoE, E.horaE, E.minE,
                                     dia, mes, ano, hora, min,
                                     tarifas, numTarifas);
    
    // Mostrar recibo
    printf("\n Saída registada com sucesso!\n");
    mostrarRecibo(E.numValidado, matriculaProcurada, E.lugar,
                  E.diaE, E.mesE, E.anoE, E.horaE, E.minE,
                  dia, mes, ano, hora, min,
                  precoPagar);
    return 1;
}

// FUNÇÃO: Mostrar recibo de saída
void mostrarRecibo(int numE, char *matricula, char *lugar,
                   int diaE, int mesE, int anoE, int horaE, int minE,
                   int diaS, int mesS, int anoS, int horaS, int minS,
                   float preco) {
    
    // Calcular duração
    int totalMinutos = 0;
    
    // Converter datas para minutos totais (simplificado)
    int minutosEntrada = (diaE * 24 * 60) + (horaE * 60) + minE;
    int minutosSaida = (diaS * 24 * 60) + (horaS * 60) + minS;
    
    // Ajustar para mudança de mês/ano (aproximação)
    if (anoS > anoE) {
        minutosSaida += (anoS - anoE) * 365 * 24 * 60;
    }
    if (mesS > mesE) {
        minutosSaida += (mesS - mesE) * 30 * 24 * 60;
    }
    
    totalMinutos = minutosSaida - minutosEntrada;
    
    int dias = totalMinutos / (24 * 60);
    int horas = (totalMinutos % (24 * 60)) / 60;
    int minutos = totalMinutos % 60;
    
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════╗\n");
    printf("║                 RECIBO DE ESTACIONAMENTO                  ║\n");
    printf("╠═══════════════════════════════════════════════════════════╣\n");
    printf("║                                                           ║\n");
    printf("║  Nº Entrada: %-6d                                         ║\n", numE);
    printf("║  Matrícula:  %-10s                                        ║\n", matricula);
    printf("║  Lugar:      %-5s                                         ║\n", lugar);
    printf("║                                                           ║\n");
    printf("║  ───────────────────────────────────────────────────────  ║\n");
    printf("║                                                           ║\n");
    printf("║     Entrada: %02d/%02d/%d às %02d:%02d                    ║\n", diaE, mesE, anoE, horaE, minE);
    printf("║     Saída:   %02d/%02d/%d às %02d:%02d                    ║\n", diaS, mesS, anoS, horaS, minS);
    printf("║                                                           ║\n");
    printf("║     Duração: ");
    if (dias > 0) printf("%d dia(s), ", dias);
    printf("%02d:%02d:%02d", horas, minutos / 60, minutos % 60);
    int espacos = 28 - (dias > 0 ? 10 : 0);
    for (int i = 0; i < espacos; i++) printf(" ");
    printf("     ║\n");
    printf("║                                                           ║\n");
    printf("║  ───────────────────────────────────────────────────────  ║\n");
    printf("║                                                           ║\n");
    printf("║    VALOR A PAGAR: %.2f €                                  ║\n", preco);
    printf("║                                                           ║\n");
    printf("║  Obrigado pela preferência!                               ║\n");
    printf("║                                                           ║\n");
    printf("╚═══════════════════════════════════════════════════════════╝\n");
}

// Função: 
int registarEntradaAutomatica(Confparque config, char *ficheiroEstacionamentos) {
    estacionamento novoEstac;
    char matriculaTemp[10];
    int dia, mes, ano, hora, min;
    int carroJaNoParque = 0;
    
    // Obter data e hora automática
    obterDataHoraAtual(&dia, &mes, &ano, &hora, &min);
    
    printf("\n╔═══════════════════════════════════════════════════════════╗\n");
    printf("║                 REGISTAR ENTRADA DE VEÍCULO               ║\n");
    printf("╚═══════════════════════════════════════════════════════════╝\n\n");
    
    printf("Data/Hora atual: %02d/%02d/%d às %02d:%02d\n\n",
           dia, mes, ano, hora, min);
    
    // Pedir matrícula
    do {
        printf("Matrícula do veículo (XX-XX-XX): ");
        scanf("%s", matriculaTemp);
        
        if (!validamatricula(matriculaTemp)) {
            printf("Matrícula inválida! Formato correto: XX-XX-XX\n\n");
            continue;
        }
        
        if (verificarCarroNoParque(matriculaTemp, "estacionamentos.txt")) {
            printf("Erro: O veículo %s já se encontra no parque!\n", matriculaTemp);
            printf("   Por favor, verifique a matrícula ou registe a saída primeiro.\n\n");
            carroJaNoParque = 1;
        } else {
            carroJaNoParque = 0;
        }
        
    } while (!validamatricula(matriculaTemp) || carroJaNoParque);
    
    char *lugarAtribuido = atribuirLugar(config, "estacionamentos.txt");
    
    if (lugarAtribuido == NULL) {
        printf("\n Erro: Não há lugares disponíveis no parque!\n");
        return 0;
    }
    
    // Preencher estruturas
    novoEstac.numE = obterProximoNumeroEntrada("estacionamentos.txt");
    novoEstac.numValidado = obterProximoNumeroValidado("estacionamentos_validos.txt");
    
    strcpy(novoEstac.matricula, matriculaTemp);
    novoEstac.anoE = ano;
    novoEstac.mesE = mes;
    novoEstac.diaE = dia;
    novoEstac.horaE = hora;
    novoEstac.minE = min;
    strcpy(novoEstac.lugar, lugarAtribuido);
    
    // Data de saída = 0 (ainda não saiu)
    novoEstac.anoS = 0;
    novoEstac.mesS = 0;
    novoEstac.diaS = 0;
    novoEstac.horaS = 0;
    novoEstac.minS = 0;
    
    // Gravar no ficheiro base
    FILE *f = fopen("estacionamentos.txt", "a");
    
    if (f == NULL) {
        printf("Erro: Não foi possível abrir o ficheiro!\n");
        return 0;
    }
    
    fprintf(f, "%d\t%s\t%d\t%d\t%d\t%d\t%d\t%s\t%d\t%d\t%d\t%d\t%d\n",
            novoEstac.numE,
            novoEstac.matricula,
            novoEstac.anoE, novoEstac.mesE, novoEstac.diaE,
            novoEstac.horaE, novoEstac.minE,
            novoEstac.lugar,
            novoEstac.anoS, novoEstac.mesS, novoEstac.diaS,
            novoEstac.horaS, novoEstac.minS);
    
    fclose(f);
    
    // Gravar no ficheiro validado
    f = fopen("estacionamentos_validos.txt", "a");
    
    if (f == NULL) {
        printf("Erro: Não foi possível abrir o ficheiro validado!\n");
        return 0;
    }
    
    fprintf(f, "%d %s %d %d %d %d %d %s %d %d %d %d %d %.2f\n",
            novoEstac.numValidado,
            novoEstac.matricula,
            novoEstac.anoE, novoEstac.mesE, novoEstac.diaE,
            novoEstac.horaE, novoEstac.minE,
            novoEstac.lugar,
            novoEstac.anoS, novoEstac.mesS, novoEstac.diaS,
            novoEstac.horaS, novoEstac.minS,
            0.00);
    
    fclose(f);
    
    // Mostrar ticket
    printf("\nEntrada registada com sucesso!\n");
    mostrarTicket(novoEstac);
    return 1;
}

// Função:
int registarSaidaAutomatica(Confparque config, char *ficheiroEstacionamentos) {
    char matriculaProcurada[10];
    int dia, mes, ano, hora, min;
    int encontrado = 0;
    
    // Obter data e hora automática
    obterDataHoraAtual(&dia, &mes, &ano, &hora, &min);
    
    printf("\n╔═══════════════════════════════════════════════════════════╗\n");
    printf("║                 REGISTAR SAÍDA DE VEÍCULO                 ║\n");
    printf("╚═══════════════════════════════════════════════════════════╝\n\n");
    
    printf("Data/Hora atual: %02d/%02d/%d às %02d:%02d\n\n",
           dia, mes, ano, hora, min);
    
    // Pedir matrícula
    do {
        printf("Matrícula do veículo (XX-XX-XX): ");
        scanf("%s", matriculaProcurada);
        
        if (!validamatricula(matriculaProcurada)) {
            printf("Matrícula inválida! Formato correto: XX-XX-XX\n\n");
        }
    } while (!validamatricula(matriculaProcurada));
    
    // Verificar se está no parque
    FILE *f = fopen("estacionamentos.txt", "r");
    if (f == NULL) {
        printf("Erro: Não foi possível abrir o ficheiro!\n");
        return 0;
    }
    estacionamento E;
    while (fscanf(f, "%d %s %d %d %d %d %d %s %d %d %d %d %d",
                  &E.numE, E.matricula,
                  &E.anoE, &E.mesE, &E.diaE, &E.horaE, &E.minE,
                  E.lugar,
                  &E.anoS, &E.mesS, &E.diaS, &E.horaS, &E.minS) == 13)
    {
        if (strcmp(E.matricula, matriculaProcurada) == 0 && E.anoS == 0) {
            encontrado = 1;
            break;
        }
    }
    fclose(f);
    
    if (!encontrado) {
        printf("\nErro: O veículo %s não se encontra no parque!\n", matriculaProcurada);
        printf("   Verifique se a matrícula está correta.\n");
        return 0;
    }
    
    // ========== MOSTRAR INFO DA ENTRADA ==========
    printf("\n Veículo encontrado no parque!\n");
    printf("┌────────────────────────────────────────────┐\n");
    printf("│ INFORMAÇÃO DO ESTACIONAMENTO               │\n");
    printf("├────────────────────────────────────────────┤\n");
    printf("│ Nº Entrada: %-6d                           │\n", E.numE);
    printf("│ Matrícula:  %-10s                          │\n", E.matricula);
    printf("│ Lugar:      %-5s                           │\n", E.lugar);
    printf("│ Entrada:    %02d/%02d/%d às %02d:%02d      │\n", E.diaE, E.mesE, E.anoE, E.horaE, E.minE);
    printf("└────────────────────────────────────────────┘\n\n");
    
    // Validar que saída é depois da entrada
    if (!validaEantesS(E.diaE, E.mesE, E.anoE, E.horaE, E.minE,
                       dia, mes, ano, hora, min)) {
        printf("\nErro: A data/hora atual é anterior à entrada!\n");
        printf("   Entrada: %02d/%02d/%d às %02d:%02d\n", E.diaE, E.mesE, E.anoE, E.horaE, E.minE);
        printf("   Atual:   %02d/%02d/%d às %02d:%02d\n", dia, mes, ano, hora, min);
        return 0;
    }
    
    // Atualizar o ficheiro
    FILE *f_temp = fopen("temp_estacionamentos.txt", "w");
    if (f_temp == NULL) {
        printf("Erro: Não foi possível criar ficheiro temporário!\n");
        return 0;
    }
    
    f = fopen("estacionamentos.txt", "r");
    if (f == NULL) {
        printf("Erro: Não foi possível reabrir o ficheiro!\n");
        fclose(f_temp);
        return 0;
    }
    
    // Copiar todos os registos, atualizando o correto
    while (fscanf(f, "%d %s %d %d %d %d %d %s %d %d %d %d %d",
                  &E.numE, E.matricula,
                  &E.anoE, &E.mesE, &E.diaE, &E.horaE, &E.minE,
                  E.lugar,
                  &E.anoS, &E.mesS, &E.diaS, &E.horaS, &E.minS) == 13)
    {
        if (strcmp(E.matricula, matriculaProcurada) == 0 && E.anoS == 0) {
            fprintf(f_temp, "%d\t%s\t%d\t%d\t%d\t%d\t%d\t%s\t%d\t%d\t%d\t%d\t%d\n",
                    E.numE, E.matricula,
                    E.anoE, E.mesE, E.diaE, E.horaE, E.minE,
                    E.lugar,
                    ano, mes, dia, hora, min);
        } else {
            fprintf(f_temp, "%d\t%s\t%d\t%d\t%d\t%d\t%d\t%s\t%d\t%d\t%d\t%d\t%d\n",
                    E.numE, E.matricula,
                    E.anoE, E.mesE, E.diaE, E.horaE, E.minE,
                    E.lugar,
                    E.anoS, E.mesS, E.diaS, E.horaS, E.minS);
        }
    }
    
    fclose(f);
    fclose(f_temp);
    
    remove("estacionamentos.txt");
    rename("temp_estacionamentos.txt", "estacionamentos.txt");
    
    // Calcular preço
    Tarifa tarifas[MAX_TARIFAS];
    int numTarifas = 0;
    
    if (!lertarifas(tarifas, &numTarifas)) {
        printf("Erro: Não foi possível carregar as tarifas!\n");
        return 0;
    }
    
    float precoPagar = CalcularPreco(E.diaE, E.mesE, E.anoE, E.horaE, E.minE,
                                     dia, mes, ano, hora, min,
                                     tarifas, numTarifas);
    
    // Mostrar recibo
    printf("\nSaída registada com sucesso!\n");
    mostrarRecibo(E.numE, matriculaProcurada, E.lugar,
                  E.diaE, E.mesE, E.anoE, E.horaE, E.minE,
                  dia, mes, ano, hora, min,
                  precoPagar);
    return 1;
}

// Função:
void MostrarMapaOcupacao_Paginado(Confparque config, char *ficheiroOcupacao, Lugar mapa[][MAX_FILAS][MAX_LUGARES]) {
    
    printf("\n MAPA DE OCUPAÇÃO DO PARQUE (PAGINADO)\n");
    printf("═══════════════════════════════════════════\n\n");
    
    // Inicializar tudo como livre
    InicializarMapa(mapa, config);
    
    // Carregar lugares indisponíveis
    carregarLugaresIndisponiveis(mapa, config);
    
    // Ler ficheiro e marcar ocupados
    PreencherMapaComOcupacoes(mapa, ficheiroOcupacao);
    
    // Inicializar paginação (1 piso por página)
    ControlePaginacao ctrl = inicializarPaginacao(config.numpisos, 1);
    
    char opcao;
    do {
        system("cls");
        printf("\n");
        printf("╔═══════════════════════════════════════════════════════════╗\n");
        printf("║                 MAPA DE OCUPAÇÃO DO PARQUE                ║\n");
        printf("╚═══════════════════════════════════════════════════════════╝\n\n");
        
        // Calcular qual piso mostrar (página atual - 1 = índice do piso)
        int pisoAtual = ctrl.paginaAtual - 1;
        
        // Desenhar apenas o piso atual
        printf("╔═══════════════════════════════════════════════════════════╗\n");
        printf("║                      PISO %d                               ║\n", pisoAtual + 1);
        printf("╚═══════════════════════════════════════════════════════════╝\n\n");
        
        // Cabeçalho com números dos lugares
        printf("    ");
        for (int lugar = 0; lugar < config.numlugares; lugar++) {
            printf("%2d ", lugar + 1);
        }
        printf("\n");
        
        printf("    ");
        for (int lugar = 0; lugar < config.numlugares; lugar++) {
            printf("───");
        }
        printf("\n");
        
        // Desenhar cada fila do piso
        for (int fila = 0; fila < config.numfilas; fila++) {
            char letraFila = 'A' + fila;
            printf(" %c │ ", letraFila);
            
            for (int lugar = 0; lugar < config.numlugares; lugar++) {
                char status = mapa[pisoAtual][fila][lugar].status;
                printf("%c  ", status);
            }
            printf("\n");
        }
        printf("\n");
        
        // Calcular estatísticas do piso
        int totalLugaresPiso = config.numfilas * config.numlugares;
        int ocupados = 0;
        int indisponiveis = 0;
        
        for (int f = 0; f < config.numfilas; f++) {
            for (int l = 0; l < config.numlugares; l++) {
                char status = mapa[pisoAtual][f][l].status;
                if (status == 'X') {
                    ocupados++;
                } else if (status == 'i' || status == 'o' ||
                          status == 'r' || status == 'm') {
                    indisponiveis++;
                }
            }
        }
        
        int livres = totalLugaresPiso - ocupados - indisponiveis;
        float percentagem = (ocupados * 100.0) / totalLugaresPiso;
        
        // Informações do piso
        printf("┌──────────────────────────────────────────────┐\n");
        printf("│      ESTATÍSTICAS - PISO %d                  │\n", pisoAtual + 1);
        printf("├──────────────────────────────────────────────┤\n");
        printf("│ Total de lugares: %d                         │\n", totalLugaresPiso);
        printf("│ Ocupados: %d  |  Livres: %d  |  Indispon: %d │\n", ocupados, livres, indisponiveis);
        printf("│ Taxa de ocupação: %.1f%%                     │\n", percentagem);
        printf("└──────────────────────────────────────────────┘\n");
        
        // Barra de navegação
        printf("\n");
        printf("╔═══════════════════════════════════════════════════════════╗\n");
        printf("║  Piso %d de %d                                            ║\n", ctrl.paginaAtual, ctrl.totalPaginas);
        printf("╠═══════════════════════════════════════════════════════════╣\n");
        printf("║  [N] Próximo piso       [P] Piso anterior                 ║\n");
        printf("║  [I] Ir para piso...    [0] Voltar ao menu                ║\n");
        printf("╚═══════════════════════════════════════════════════════════╝\n");
        printf("\nOpção: ");
        scanf(" %c", &opcao);
        
        switch (opcao) {
            case 'N':
            case 'n':
                if (ctrl.paginaAtual < ctrl.totalPaginas) {
                    ctrl.paginaAtual++;
                } else {
                    printf("\n Já está no último piso!\n");
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
                    printf("\n Já está no primeiro piso!\n");
                    printf("Pressione ENTER para continuar...");
                    getchar();
                    getchar();
                }
                break;
                
            case 'I':
            case 'i': {
                int pisoProcurado;
                printf("\n Qual piso deseja visualizar? (1 a %d): ", config.numpisos);
                scanf("%d", &pisoProcurado);
                
                if (pisoProcurado >= 1 && pisoProcurado <= config.numpisos) {
                    ctrl.paginaAtual = pisoProcurado;
                } else {
                    printf(" Piso inválido!\n");
                    printf("Pressione ENTER para continuar...");
                    getchar();
                    getchar();
                }
                break;
            }
                
            case '0':
                break;
                
            default:
                printf("\n Opção inválida!\n");
                printf("Pressione ENTER para continuar...");
                getchar();
                getchar();
        }
        
    } while (opcao != '0');
}

// Legenda (pode ser mostrada uma única vez no início ou ao fim)
void MostrarLegendaMapa(void) {
    printf("\n╔═══════════════════════════════════════════════════════════╗\n");
    printf("║                        LEGENDA                            ║\n");
    printf("╠═══════════════════════════════════════════════════════════╣\n");
    printf("║  -  = Lugar Livre                                         ║\n");
    printf("║  X  = Lugar Ocupado                                       ║\n");
    printf("║  i  = Condições inadequadas                               ║\n");
    printf("║  o  = Objeto de obras                                     ║\n");
    printf("║  r  = Reservado                                           ║\n");
    printf("║  m  = Outros motivos                                      ║\n");
    printf("╚═══════════════════════════════════════════════════════════╝\n");
}
