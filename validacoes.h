#ifndef validacoes_h
#define validacoes_h

int validaData(int dia, int mes, int ano);
int validamatricula(char *mat);
int validaLugar(char *lugar, int maxPisos, char maxFila, int maxLugares);
int validaEantesS(int diaE, int mesE, int anoE, int horaE, int minE,
                  int diaS, int mesS, int anoS, int horaS, int minS);
int ComparaDatas(int d1, int m1, int a1, int h1, int min1,
                 int d2, int m2, int a2, int h2, int min2);

#endif /* validacoes_h */
