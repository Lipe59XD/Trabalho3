/*============================================================================*/
/* CAVERNA MARCIANA                                                           */
/*----------------------------------------------------------------------------*/
/* Autor: Bogdan T. Nassu                                                     */
/*============================================================================*/
/** Funções para manipulação da "caverna marciana". A caverna é representada
 * como uma matriz alocada dinamicamente e preenchida aleatoriamente. Os valores
 * de cada posição da matriz indicam o conteúdo daquela casa no tabuleiro:
 *
 * 0: Casa livre/vazia.
 * -1: Poço de lava.
 * 1 a 4: Casa contendo "calor" - o número diz quantas casas vizinhas contêm
 *   poços de lava.
 * 5: Poça d'água.
 * 6 a 9: Casa contendo poça d'água E calor - o número-5 diz quantas casas
 *   vizinhas contêm poços de lava.*/
/*============================================================================*/

#ifndef __CAVERNA_H
#define __CAVERNA_H

/*============================================================================*/

#define POSICAO_INVALIDA -10
#define POSICAO_COM_LAVA -1
#define POSICAO_COM_AGUA 5

/* Um par x,y. */
typedef struct
{
    int x;
    int y;
} Coordenada;

/* Representa a "caverna marciana". */
typedef struct
{
    int altura;
    int largura;
    int** tabuleiro;
} Caverna;

/*============================================================================*/

Caverna* geraCaverna (int altura, int largura, int n_pocos_lava, int random_seed);
void destroiCaverna (Caverna* caverna);
void mostraCaverna (Caverna* caverna);

int posicaoLivre (int valor_casa);
int posicaoComLava (int valor_casa);
int posicaoComCalor (int valor_casa);
int posicaoComAguaECalor (int valor_casa);
int posicaoComAgua (int valor_casa);

/*============================================================================*/
#endif /* __CAVERNA_H */
