/*============================================================================*/
/* GAME STATE PARA O JOGUINHO DA CAVERNA MARCIANA                             */
/*----------------------------------------------------------------------------*/
/* Autor: Bogdan T. Nassu                                                     */
/*============================================================================*/
/** Módulo básico que mantém o estado de um jogo. */
/*============================================================================*/

#include <stdio.h>
#include <stdlib.h>

#include "game_state.h"

/*============================================================================*/
/** Cria e inicializa uma struct GameState (uma partida do jogo).
 *
 * Parâmetros: int altura: altura da caverna do jogo.
 *             int largura: largura da caverna do jogo.
 *             int n_pocos_lava: número de poços de lava.
 *             int random_seed: semente para o gerador de números aleatórios.
 *
 * Valor de Retorno: um ponteiro para um GameState alocado. Lembre-se de destruí-lo depois! */

GameState* criaGameState (int altura, int largura, int n_pocos_lava, int random_seed)
{
    int i, j;
    GameState* g = (GameState*) malloc (sizeof (GameState));

    g->caverna = geraCaverna (altura, largura, n_pocos_lava, random_seed);
    if (!g->caverna)
        exit (1); /* ?! */

    g->pos_atual.x = 0;
    g->pos_atual.y = 0;
    g->n_rodadas = 0;
    g->achou_agua = 0;
    g->morreu = 0;

    g->posicoes_visitadas = (int**) malloc (sizeof (int*) * altura);
    for (i = 0; i < altura; i++)
        g->posicoes_visitadas [i] = (int*) malloc (sizeof (int) * largura);

    for (i = 0; i < altura; i++)
        for (j = 0; j < largura; j++)
            g->posicoes_visitadas [i][j] = 0;

    g->posicoes_visitadas [0][0] = 1;

    return (g);
}

/*----------------------------------------------------------------------------*/
/** Desaloca um GameState.
 *
 * Parâmetros: GameState g: GameState a destruir.
 *
 * Valor de Retorno: NENHUM */

void destroiGameState (GameState* g)
{
    int i;

    for (i = 0; i < g->caverna->altura; i++)
        free (g->posicoes_visitadas [i]);
    free (g->posicoes_visitadas);
    destroiCaverna (g->caverna);
    free (g);
}

/*----------------------------------------------------------------------------*/
/** Diz se um jogo já terminou. O jogo termina quando o robô já coletou a
 * água e retornou à posição (0, 0), ou quando o robô morre.
 *
 * Parâmetros: GameState* g: jogo a testar.
 *
 * Valor de Retorno: 1 se o jogo já terminou, 0 do contrário. */

int jogoTerminado (GameState* g)
{
    return (g->morreu || (g->achou_agua && g->pos_atual.x == 0 && g->pos_atual.y == 0));
}

/*----------------------------------------------------------------------------*/
/** Diz se existe água na posição atual.
 *
 * Parâmetros: GameState* g: jogo a testar.
 *
 * Valor de Retorno: 1 se a posição atual contém água, 0 do contrário. */

int posicaoAtualComAgua (GameState* g)
{
    return (posicaoComAgua (g->caverna->tabuleiro [g->pos_atual.y][g->pos_atual.x]));
}

/*----------------------------------------------------------------------------*/
/** Diz o número de poços de lava em casas vizinhas à atual.
 *
 * Parâmetros: GameState* g: jogo a testar.
 *
 * Valor de Retorno: o número de poços de lava em casas vizinhas à atual. */

int nVizinhosComLava (GameState* g)
{
    int conteudo_atual = g->caverna->tabuleiro [g->pos_atual.y][g->pos_atual.x];

    if (posicaoComCalor (conteudo_atual))
        return (conteudo_atual);
    else if (posicaoComAguaECalor (conteudo_atual))
        return (conteudo_atual - POSICAO_COM_AGUA);

    return (0);
}

/*----------------------------------------------------------------------------*/
/** Mostra as posições já visitadas da caverna para um jogo dado. Esta função
 * é uma cópia da mostraCaverna, com a diferença que ela leva em conta quais
 * casas já foram visitadas pelo robô.
 *
 * As casas são impressas assim:
 * Entrada: | E |
 * Casa não-visitada: | ? |
 * Casa livre: |   |
 * Casa com lava: | X |
 * Casa com calor: | N |, onde N é o número de vizinhos com lava.
 * Casa com água: | A |
 * Casa com água e calor: | AN|, onde N é o número de vizinhos com lava.
 * Casa atual: terá um '*'.
 *
 * GameState* g: jogo a mostrar.
 *
 * Valor de Retorno: NENHUM */

void mostraMapaAtual (GameState* g)
{
     int i, j;

    /* Linha superior - 1 para a linha à esquerda, +4 para cada casa do tabuleiro. */
    for (i = 0; i < g->caverna->largura*4+1; i++)
        printf ("-");
    printf ("\n");

    /* Tabuleiro. */
    for (i = 0; i < g->caverna->altura; i++)
    {
        printf ("|");
        for (j = 0; j < g->caverna->largura; j++)
        {
            char c1 = (i == g->pos_atual.y && j == g->pos_atual.x)? '*' : ' ';

            if (!i && !j) /* Entrada */
                printf ("%cE |", c1);
            else if (!g->posicoes_visitadas [i][j])
                printf ("%c? |", c1);
            else if (posicaoLivre (g->caverna->tabuleiro [i][j]))
                printf ("%c  |", c1);
            else if (posicaoComLava (g->caverna->tabuleiro [i][j]))
                printf ("%cX |", c1);
            else if (posicaoComCalor (g->caverna->tabuleiro [i][j]))
                printf ("%c%d |", c1, g->caverna->tabuleiro [i][j]);
            else if (posicaoComAguaECalor (g->caverna->tabuleiro [i][j]))
                printf ("%cA%d|", c1, g->caverna->tabuleiro [i][j]-POSICAO_COM_AGUA);
            else
                printf ("%cA |", c1);
        }
        printf ("\n");

        /* Linha inferior - 1 para a linha à esquerda, +4 para cada casa do tabuleiro. */
        for (j = 0; j < g->caverna->largura*4+1; j++)
            printf ("-");
        printf ("\n");
    }
}

/*----------------------------------------------------------------------------*/
/** Funções usadas para deslocar o robô. Todas chamam uma função "mestra" que
 * atualiza o estado do jogo, após testar se o movimento é permitido.
 *
 * Parâmetros: GameState* g: jogo a atualizar.
 *
 * Valor de Retorno: o conteúdo da posição da caverna na qual o robô se encontra. */

void moveRobo (GameState* g, int x, int y)
{
    g->pos_atual.x = x;
    g->pos_atual.y = y;
    g->posicoes_visitadas [y][x] = 1;

    if (posicaoComLava (g->caverna->tabuleiro [y][x]))
        g->morreu = 1;
    else if (posicaoComAgua (g->caverna->tabuleiro [y][x]))
        g->achou_agua = 1;
}

int move (GameState* g, int direcao_movimento)
{
    switch (direcao_movimento)
    {
        case (DIR_MOVIMENTO_CIMA): return (moveCima (g));
        case (DIR_MOVIMENTO_BAIXO): return (moveBaixo (g));
        case (DIR_MOVIMENTO_ESQUERDA): return (moveEsquerda (g));
        case (DIR_MOVIMENTO_DIREITA): return (moveDireita (g));
        default: g->n_rodadas++; return (POSICAO_INVALIDA);
    }
}

int moveCima (GameState* g)
{
    g->n_rodadas++;
    if (g->pos_atual.y > 0)
        moveRobo (g, g->pos_atual.x, g->pos_atual.y - 1);
    return (g->caverna->tabuleiro [g->pos_atual.y][g->pos_atual.x]);
}

int moveBaixo (GameState* g)
{
    g->n_rodadas++;
    if (g->pos_atual.y < g->caverna->altura - 1)
        moveRobo (g, g->pos_atual.x, g->pos_atual.y + 1);
    return (g->caverna->tabuleiro [g->pos_atual.y][g->pos_atual.x]);
}

int moveEsquerda (GameState* g)
{
    g->n_rodadas++;
    if (g->pos_atual.x > 0)
        moveRobo (g, g->pos_atual.x - 1, g->pos_atual.y);
    return (g->caverna->tabuleiro [g->pos_atual.y][g->pos_atual.x]);
}

int moveDireita (GameState* g)
{
    g->n_rodadas++;
    if (g->pos_atual.x < g->caverna->largura - 1)
        moveRobo (g, g->pos_atual.x + 1, g->pos_atual.y);
    return (g->caverna->tabuleiro [g->pos_atual.y][g->pos_atual.x]);
}

/*============================================================================*/
