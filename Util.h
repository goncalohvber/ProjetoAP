#ifndef Util_h
#define Util_h

#include <stdio.h>
#include "Estacionamento.h"
void mostrarMenu(void);
void limparTela(void);
void mostrarMensagem(char *mens);
void obterDataHoraAtual(int *dia, int *mes, int *ano, int *hora, int *min);
void menuConfiguracoes(Confparque config);
typedef struct {
    int total;
    int disponiveis;
    int ocupados;
    int indisponiveis;
} EstatisticasParque;

EstatisticasParque obterEstatisticasParque(Confparque config);
#endif /* Util_h */
