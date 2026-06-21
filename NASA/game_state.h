/*============================================================================*/
/* GAME STATE PARA O JOGUINHO DA CAVERNA MARCIANA                             */
/*----------------------------------------------------------------------------*/
/* Autor: Bogdan T. Nassu                                                     */
/*============================================================================*/
/** Módulo básico que mantém o estado de um jogo. */
/*============================================================================*/

#ifndef __GAME_STATE_H
#define __GAME_STATE_H

/*============================================================================*/

#include "caverna.h"

/*============================================================================*/
/** Direções para o movimento. */
#define DIR_MOVIMENTO_CIMA 1
#define DIR_MOVIMENTO_BAIXO 2
#define DIR_MOVIMENTO_ESQUERDA 3
#define DIR_MOVIMENTO_DIREITA 4

/*============================================================================*/
/** Estrutura que mantém o estado do jogo. */

typedef struct
{
    Caverna* caverna; /* Apontador para a caverna do jogo. */
    Coordenada pos_atual; /* Posição atual. */
    int n_rodadas; /* Número de rodadas já jogadas. */
    int** posicoes_visitadas; /* Mapa que indica quais posições da caverna já foram visitadas. */
    int achou_agua; /* Flag que indica se o robô já coletou a amostra de água. */
    int morreu; /* Flag que indica se o robô caiu em um poço de lava. */
} GameState;

/*============================================================================*/

GameState* criaGameState (int altura, int largura, int n_pocos_lava, int random_seed);
void destroiGameState (GameState* g);
int jogoTerminado (GameState* g);
int posicaoAtualComAgua (GameState* g);
int nVizinhosComLava (GameState* g);
void mostraMapaAtual (GameState* g);
int move (GameState* g, int direcao_movimento);
int moveCima (GameState* g);
int moveBaixo (GameState* g);
int moveEsquerda (GameState* g);
int moveDireita (GameState* g);

/*============================================================================*/
#endif /* __GAME_STATE_H */
