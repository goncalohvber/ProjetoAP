#include "Estacionamento.h"
#include "validacoes.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
void LimpaFicheiroEstac(char *ficheirobase, char *ficheirovalido, char *ficheiroerros, Confparque config);

// CHAMA NOVAMENTE A FUNCAO LIMPAR PARA VALIDAR OS ESTACIONAMENTOS.
void atualizarValidacao(Confparque config) {
    printf("\n Atualizando ficheiro validado...\n");

    // Revalidar o ficheiro estacionamentos.txt
    LimpaFicheiroEstac("estacionamentos.txt",
                       "estacionamentos_validos.txt",
                       "relatorio_erros.txt",
                       config);

    printf(" Ficheiro atualizado!\n");
}
