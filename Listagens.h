//
//  Listagens.h
//  ProjetoEstacionamento
//
//  Sistema de listagens paginadas
//

#ifndef Listagens_h
#define Listagens_h

#include "Estacionamento.h"
#include "Instalacao.h"


// ESTRUTURA PARA CONTROLAR PAGINAÇÃO
typedef struct {
    int paginaAtual;      // Página atual
    int totalPaginas;     // Total de páginas
    int registosPorPag;   // Registos por página (ex: 15)
    int totalRegistos;    // Total de registos disponíveis
} ControlePaginacao;

// Menu principal de listagens
void menuListagens(Confparque config);

// Listar TODOS os estacionamentos
void listarTodosEstacionamentos(char *ficheiroEstacionamentos);

// Listar estacionamentos POR DATA
void listarPorData(char *ficheiroEstacionamentos);

// Listar estacionamentos POR MATRÍCULA
void listarPorMatricula(char *ficheiroEstacionamentos);

// 5. Listar apenas veículos NO PARQUE
void listarVeiculosNoParque(char *ficheiroEstacionamentos);


// FUNÇÕES AUXILIARES DE PAGINAÇÃO
// Inicializar controle de paginação
ControlePaginacao inicializarPaginacao(int totalRegistos, int registosPorPag);

// Mostrar barra de navegação
void mostrarBarraNavegacao(ControlePaginacao ctrl);

// Gravar listagem atual em ficheiro TXT
void gravarListagemTXT(estacionamento est[], int numRegistos, char *nomeFicheiro);

// Gravar listagem em CSV (EXTRA - se tiverem tempo)
void gravarListagemCSV(estacionamento est[], int numRegistos, char *nomeFicheiro);

void gerarTabelaDinamica(char *ficheiroEstacionamentos);
#endif /* Listagens_h */
