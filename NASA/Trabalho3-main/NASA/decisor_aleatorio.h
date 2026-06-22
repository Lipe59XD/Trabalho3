/*============================================================================*/
/* UM DECISOR MUITO, MUITO SIMPLES PARA O PROBLEMA DA CAVERNA MARCIANA        */
/*----------------------------------------------------------------------------*/
/* Autor: Bogdan T. Nassu                                                     */
/*============================================================================*/
/** Um robô muito ruim - ele decide seus movimentos aleatoriamente! */
/*============================================================================*/

#ifndef __DECISOR_ALEATORIO_H
#define __DECISOR_ALEATORIO_H

/*============================================================================*/

#include "game_state.h"

/*============================================================================*/
/** Estrutura do decisor. Este é um decisor muito burro, não tem nada... */

typedef struct
{

} Decisor;

/*============================================================================*/

Decisor* criaDecisor (int altura, int largura);
void destroiDecisor (Decisor* d);
int proximoMovimento (Decisor* d, Coordenada pos, int agua, int n_lava);

/*============================================================================*/
#endif /* __DECISOR_ALEATORIO_H */


