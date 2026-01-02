#ifndef GestaoLugares_h
#define GestaoLugares_h

#include "Estacionamento.h"
#include "Instalacao.h"

void consultarRegisto(char *ficheiroEstacionamentos);

void mostrarDetalhesEstacionamento(estacionamento E);

int alterarRegisto(char *ficheiroEstacionamentos, Confparque config);

int eliminarRegisto(char *ficheiroEstacionamentos);

int marcarLugarIndisponivel(Confparque config);

int tornarLugarDisponivel(Confparque config);

char verificarIndisponibilidade(char *lugar, Confparque config);

void carregarLugaresIndisponiveis(Lugar mapa[][MAX_FILAS][MAX_LUGARES],
                                  Confparque config);

void guardarLugaresIndisponiveis(Lugar mapa[][MAX_FILAS][MAX_LUGARES],
                                 Confparque config);

void menuGestaoLugares(Confparque config);

#endif /* GestaoLugares_h */
