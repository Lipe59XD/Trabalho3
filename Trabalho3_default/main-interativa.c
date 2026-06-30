/*============================================================================*/
/* JOGUINHO DA CAVERNA MARCIANA                                               */
/*----------------------------------------------------------------------------*/
/* Autor: Bogdan T. Nassu                                                     */
/*============================================================================*/
/** Exploração da caverna marciana - versão interativa.  */
/*============================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "game_state.h"

/*============================================================================*/

#define CLEAR_SCREEN_CMD "cls" // No Linux, mude para "clear".

#define ALTURA_CAVERNA 5
#define LARGURA_CAVERNA 5
#define N_POCOS_LAVA 4

/*============================================================================*/

int main()
{
    int ultimo_conteudo = POSICAO_INVALIDA; /* Comeca com uma posicao invalida. */
    GameState* game_state;

    /* Gera o jogo. A semente para os números aleatórios é desconhecida. */
    game_state = criaGameState (ALTURA_CAVERNA, LARGURA_CAVERNA, N_POCOS_LAVA, time (NULL));

    /* Loop principal do jogo. */
    while (!jogoTerminado (game_state))
    {
        char comando;

		// Mostra coisas.
        system (CLEAR_SCREEN_CMD);
        mostraMapaAtual (game_state);

        printf ("\n");
        if (ultimo_conteudo == POSICAO_INVALIDA)
            printf ("(entrada)\n");
        else if (posicaoLivre (ultimo_conteudo))
            printf ("(livre)\n");
        else if (posicaoComCalor (ultimo_conteudo))
            printf ("(%d pocos de lava em casas vizinhas)\n", ultimo_conteudo);
        else if (posicaoComAguaECalor (ultimo_conteudo))
            printf ("(agua! %d pocos de lava em casas vizinhas)\n", ultimo_conteudo - POSICAO_COM_AGUA);
        else
            printf ("(agua!)\n");

        printf ("\nRodadas: %d\n", game_state->n_rodadas);
        printf ("Proximo movimento (w: acima, a: esquerda, s: abaixo, d: direita): ");

		// Lê e interpreta o comando.
		scanf ("%c", &comando);

        if (comando == 'w')
            ultimo_conteudo = moveCima (game_state);
        else if (comando == 'a')
            ultimo_conteudo = moveEsquerda (game_state);
        else if (comando == 's')
            ultimo_conteudo = moveBaixo (game_state);
        else if (comando == 'd')
            ultimo_conteudo = moveDireita (game_state);
    }

    if (game_state->morreu)
        printf ("GAME OVER: O robo caiu na lava!\n");
    else
        printf ("O robo completou sua missao em %d rodadas.\n", game_state->n_rodadas);

    destroiGameState (game_state);
    system ("pause");

    return (0);
}

/*============================================================================*/
