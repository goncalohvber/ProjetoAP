#ifndef Estacionamento_h
#define Estacionamento_h
#include "Instalacao.h"
#include "Tarifas.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define MAX_MATRICULA 10
#define MAX_LUGAR 6
#define FICHEIRO_ESTAC_BIN "estacionamentos.bin"
#define FICHEIRO_OCUPACAO_BIN "ocupacao.bin"
#define FICHEIRO_CONTADOR_BIN "contador.bin"

typedef struct estacionamento{
    int numE;
    int numValidado;
    char matricula[MAX_MATRICULA];
    int anoE, mesE, diaE, horaE, minE, anoS, mesS, diaS, horaS, minS;
    char lugar[MAX_LUGAR];
    float valorPago;
    int ativo;
} estacionamento;

typedef struct {
    char status;
    char matricula[MAX_MATRICULA];
    char motivo;
} Lugar;

typedef struct {
    int ultimoNumE;
    int totalEstacionamentos;
} ContadorSistema;

void MostrarFicheiroEstacionamento(char *nomeFicheiro);
int personalizapp(Confparque config);
void MostrarMapaOcupacao_ComMapa(Confparque config, char *ficheiroOcupacao, Lugar mapa[][MAX_FILAS][MAX_LUGARES]);
void InicializarMapa(Lugar mapa[][MAX_FILAS][MAX_LUGARES], Confparque config);
void PreencherMapaComOcupacoes(Lugar mapa[][MAX_FILAS][MAX_LUGARES], char *ficheiroOcupacao);
void DesenharMapa(Lugar mapa[][MAX_FILAS][MAX_LUGARES], Confparque config);
int registarEntrada(Confparque config, char *ficheiroEstacionamentos);
int verificarCarroNoParque(char *matricula, char *ficheiroEstacionamentos);
int obterProximoNumeroEntrada(char *ficheiroEstacionamentos);
char* atribuirLugar(Confparque config, char *ficheiroEstacionamentos);
void mostrarTicket(estacionamento E);
int registarSaida(Confparque config, char *ficheiroEstacionamentos);
void mostrarRecibo(int numE, char *matricula, char *lugar,
                   int diaE, int mesE, int anoE, int horaE, int minE,
                   int diaS, int mesS, int anoS, int horaS, int minS,
                   float preco);
int verificarCarroNoParque(char *matricula, char *ficheiroEstacionamentos);
int obterProximoNumeroEntrada(char *ficheiroEstacionamentos);
char* atribuirLugar(Confparque config, char *ficheiroEstacionamentos);
void LimpaFicheiroEstac(char *ficheirobase, char *ficheirovalido,
                        char *ficheiroerros, Confparque config);
void gerarficheiroocupacao(char *ficheirovalido, char *ficheiroocupacao,
                           int diaU, int mesU, int anoU, int horaU, int minU);
void atualizarValidacao(Confparque config);
void ProcessarLugar(char *lugarStr, int *piso, int *fila, int *numero);
#endif /* Estacionamento_h */
