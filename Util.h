#ifndef Util_h
#define Util_h

#include <stdio.h>
#include "Estacionamento.h"
void mostrarMenu(void);
void limparTela(void);
void mostrarMensagem(char *mens);
void obterDataHoraAtual(int *dia, int *mes, int *ano, int *hora, int *min);
void menuConfiguracoes(Confparque config);

//Estrutura com todos os dados das estatisticas do parque, usado para mostrar os lugares disponiveis e indisponiveis em todo o parque
typedef struct {
    int total;
    int disponiveis;
    int ocupados;
    int indisponiveis;
} EstatisticasParque;

EstatisticasParque obterEstatisticasParque(Confparque config);
#endif /* Util_h */
