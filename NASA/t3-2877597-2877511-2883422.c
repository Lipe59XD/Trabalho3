/*============================================================================*/

#include <stdlib.h>
#include "t3-2877597-2877511-2883422.h"
#define CIMA 1
#define BAIXO 2
#define ESQUERDA 3
#define DIREITA 4
#define RISCO_MAX 5

/*============================================================================*/
/** Simplesmente aloca um Decisor.
 *
 * Parâmetros: int altura: número de linhas no tabuleiro.
 *             int largura: número de colunas no tabuleiro.
 *
 * Valor de Retorno: um Decisor alocado. */

Decisor* criaDecisor (int altura, int largura)
{
    int i, j;
    Decisor* decisor = (Decisor*) malloc (sizeof (Decisor));
    decisor->altura = altura;
    decisor->largura = largura;
    decisor->passos = 0;

    //Aloca dinamicamente a matriz de risco

    decisor->matriz_risco = (int**)malloc(altura*sizeof(int*));
    for(i=0; i<altura; i++)
    {
        decisor->matriz_risco[i] = (int*)malloc(sizeof(int)*largura);
        for(j=0; j<largura; j++)
            decisor->matriz_risco[i][j] = 1;
    }

    decisor->lab = (int**)malloc(sizeof(int*)*largura);
    for(i=0; i<altura; i++)
        decisor->lab[i] = (int*)malloc(sizeof(int)*largura);
    //Aloca dinamicamento a matriz caminho_do_robo, que contem o historico de posicoes do robo
    //O tamanho do vetor eh no maximo a quantidade de passos ate acabar a bateria
    decisor->caminho_do_robo = (Coordenada*)malloc(sizeof(Coordenada)*decisor->altura*decisor->largura*20);

    return decisor;
}

/*----------------------------------------------------------------------------*/
/** Simplesmente desaloca um Decisor.
 *
 * Parâmetros: Decisor* d: Decisor a destruir.
 *
 * Valor de Retorno: NENHUM */

void destroiDecisor (Decisor* d)
{
    int i;
    for(i=0; i<(d->altura); i++)
        free(d->matriz_risco[i]);
    for(i=0; i<(d->altura); i++)
        free(d->lab[i]);
    free(d->lab);
    free(d->matriz_risco);
    free(d->caminho_do_robo);
    free(d);
}

/*----------------------------------------------------------------------------*/
/** Este decisor não faz absolutamente nada quando é informado sobre o conteúdo
 * da posição atual! Ele simplesmente retorna um movimento aleatório.
 *
 * Parâmetros: Decisor* d: o Decisor.
 *             Coordenada pos: posição atual.
 *             int agua: 1 se a posição atual tiver água, 0 do contrário.
 *             int n_lava: número de poços de lava em casas vizinhas à atual.
 *
 * Valor de Retorno: a direção do próximo movimento: 1 – para cima, 2 – para baixo, 3 – para a esquerda, 4 – para a direita. */

int proximoMovimento (Decisor* d, Coordenada pos, int agua, int n_lava)
{
    int i, linha, coluna, acumulado, sorteado, soma_pesos, encontrou_agua;
    soma_pesos = 0;
    int pesos[] = {0, 0, 0, 0, 0};
    linha = pos.y;
    coluna = pos.x;
    d->matriz_risco[linha][coluna] = 0;
    d->caminho_do_robo[d->passos] = pos;
    d->passos++;
    if(!agua)
    {
        if(linha != 0 && d->matriz_risco[linha-1][coluna] != 0)
        {
            d->matriz_risco[linha-1][coluna]+=n_lava;
            pesos[CIMA] = RISCO_MAX-(d->matriz_risco[linha-1][coluna]);
            soma_pesos+=pesos[CIMA];
        }
        if(linha != (d->altura)-1 && d->matriz_risco[linha+1][coluna] != 0)
        {
            d->matriz_risco[linha+1][coluna] +=n_lava;
            pesos[BAIXO] = RISCO_MAX-(d->matriz_risco[linha+1][coluna]);
            soma_pesos+=pesos[BAIXO];
        }
        if(coluna != 0  && d->matriz_risco[linha][coluna-1] != 0)
        {
            d->matriz_risco[linha][coluna-1] += n_lava;
            pesos[ESQUERDA] = RISCO_MAX-(d->matriz_risco[linha][coluna-1]);
            soma_pesos+=pesos[ESQUERDA];
        }
        if(coluna != (d->largura)-1 && d->matriz_risco[linha][coluna+1] != 0)
        {
            d->matriz_risco[linha][coluna+1] += n_lava;
            pesos[DIREITA] = RISCO_MAX-(d->matriz_risco[linha][coluna+1]);
            soma_pesos+=pesos[DIREITA];
        }
        acumulado = 0;
        if(soma_pesos!= 0)
            sorteado = 1+(rand()%soma_pesos);
        else    return 1+(rand()%4);

        for(i=1; i<5; i++)
        {
            acumulado += pesos[i];
            if(sorteado < acumulado)
                return i;
        }
    }
    else
    {
        //funcao para voltar
    }


}

void constroi_caminho(Decisor* d, int altura, int largura)
{
    int i, j;
    for(i=0; i<altura; i++)
    {
        for(j=0; j<largura; j++)
        {
            if(d->matriz_risco[i][j] != 0)
                d->lab[i][j] = -2; //-2 representa a parede
            else d->lab[i][j] = 0;
        }
    }

}

/*void preencheMatrizCusto (int** lab, Coordenada pos_alvo)
{
    int i, x, y, custo, pos_atual_da_fila, qtd_adicionada;
    qtd_adicionada = 1;
    pos_atual_da_fila = 0;
    custo = 1;
    Coordenada* fila;
    Coordenada2D adiciona;
    //fila das coordenadas a serem preenchidas
    fila = (Coordenada2D*)malloc(sizeof(Coordenada2D)*BUFLEN);
    fila[0] = pos_queijo; //coloca o primeiro componente da fila como a posicao do queijo
    lab->mat[pos_queijo.y][pos_queijo.x] = 0; //inicializa a posicao do queijo como zero

    for(i=0; i<=pos_atual_da_fila; i++)//percorre os elementos da fila
    {
        qtd_adicionada = 0;
        x = fila[i].y;
        y = fila[i].x;
        custo = lab->mat[x][y];

        if(lab->mat[x+1][y] == -1)
        {
            lab->mat[x+1][y] = custo+1;
            qtd_adicionada++;
            pos_atual_da_fila++;
            adiciona.x = y;
            adiciona.y = x+1;
            fila[pos_atual_da_fila] = adiciona;
        }
        if(lab->mat[x-1][y] == -1)
        {
            lab->mat[x-1][y] = custo+1;
            qtd_adicionada++;
            pos_atual_da_fila++;
            adiciona.x = y;
            adiciona.y = x-1;
            fila[pos_atual_da_fila] = adiciona;
        }
        if(lab->mat[x][y+1] == -1)
        {
            lab->mat[x][y+1] = custo+1;
            qtd_adicionada++;
            pos_atual_da_fila++;
            adiciona.x = y+1;
            adiciona.y = x;
            fila[pos_atual_da_fila] = adiciona;
        }
        if(lab->mat[x][y-1] == -1)
        {
            lab->mat[x][y-1] = custo+1;
            qtd_adicionada++;
            pos_atual_da_fila++;
            adiciona.x = y-1;
            adiciona.y = x;
            fila[pos_atual_da_fila] = adiciona;
        }
    }
   free(fila);

}*/
/*============================================================================*/


