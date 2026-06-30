/*============================================================================*/
/* UM DECISOR MUITO, MUITO SIMPLES PARA O PROBLEMA DA CAVERNA MARCIANA        */
/*----------------------------------------------------------------------------*/
/* Autor: Bogdan T. Nassu                                                     */
/*============================================================================*/
/* Um robô muito ruim - ele decide seus movimentos aleatoriamente! */
/*============================================================================*/

#ifndef T3_2877597_2877511_2883422_H
#define T3_2877597_2877511_2883422_H

/*============================================================================*/

#include "game_state.h"

/*============================================================================*/
/*Estrutura do decisor*/

typedef struct
{
    int altura;
    int largura;
    int** matriz_risco;
    int** lab; //casas desconhecidas: -1, casas com lava -2, casas conhecidas 0
    int** temperature_map; //guarda o n_lava de cada casa e -1 para casas desconhecidas
    Coordenada* caminho_volta;
    int init;
    int passo_da_volta;
    int encontrou_agua;

    struct{
        Coordenada* posicoes;
        int topo;
    }pilha;



} Decisor;

/*============================================================================*/

Decisor* criaDecisor (int altura, int largura);
void destroiDecisor (Decisor* d);
int proximoMovimento (Decisor* d, Coordenada pos, int agua, int n_lava);

/*============================================================================*/
#endif /*T3_2877597_2877511_2883422_H*/
