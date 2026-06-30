/*============================================================================*/
#include<stdio.h>
#include <stdlib.h>
#include "decisor_aleatorio.h"
#define CIMA 1
#define BAIXO 2
#define ESQUERDA 3
#define DIREITA 4
#define RISCO_MAX 6




void constroi_caminho(Decisor* d);
void preencheMatrizCusto (Decisor* d, Coordenada pos_alvo);
void calculaCaminho (Decisor* d, Coordenada pos_alvo);
int identificaSentido (Coordenada atual, Coordenada proxima);
int decideDirecao(Decisor* d, Coordenada pos, int n_lava);
void deducaoMatematica(Decisor* d);
void adicionaNaPilha(Decisor* d, Coordenada novaCoordenada);
Coordenada removeDaPilha(Decisor* d);
int posBoa(Decisor* d, Coordenada Coordenada);
int temCasaSegura(Decisor* d);



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
    decisor->init = 1;
    decisor->passo_da_volta = 1;
    decisor->encontrou_agua = 0;
    decisor->pilha.topo = -1;

    //Aloca e inicializa a matriz de risco
    decisor->matriz_risco = (int**)malloc(altura*sizeof(int*));
    for(i=0; i<altura; i++)
    {
        decisor->matriz_risco[i] = (int*)malloc(sizeof(int)*largura);
        for(j=0; j<largura; j++)
            decisor->matriz_risco[i][j] = 1;
    }
    //Aloca dinamicamente o labirinto
    decisor->lab = (int**)malloc(sizeof(int*)*altura);
    for(i=0; i<altura; i++)
        decisor->lab[i] = (int*)malloc(sizeof(int)*largura);

    //Aloca a pilha de posicoes
    decisor->pilha.posicoes = (Coordenada*)malloc(sizeof(Coordenada)*decisor->altura*decisor->largura*20);

    //Aloca e inicializa a matriz de mapa das lavas
    decisor->temperature_map = (int**)malloc(sizeof(int*)*altura);
    for(i=0; i<altura; i++)
    {
        decisor->temperature_map[i] = (int*)malloc(sizeof(int)*largura);
        for(j=0; j<largura; j++)
            decisor->temperature_map[i][j] = -1;
    }


    //O tamanho do vetor eh no maximo a quantidade de passos ate acabar a bateria
    decisor->caminho_volta = (Coordenada*)malloc(sizeof(Coordenada)*decisor->altura*decisor->largura*20);

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
    {
        free(d->matriz_risco[i]);
        free(d->lab[i]);
        free(d->temperature_map[i]);
    }
    free(d->pilha.posicoes);
    free(d->lab);
    free(d->temperature_map);
    free(d->matriz_risco);
    free(d->caminho_volta);
    free(d);
}

/*----------------------------------------------------------------------------*/
/**
 *
 * Parâmetros: Decisor* d: o Decisor.
 *             Coordenada pos: posição atual.
 *             int agua: 1 se a posição atual tiver água, 0 do contrário.
 *             int n_lava: número de poços de lava em casas vizinhas à atual.
 *
 * Valor de Retorno: a direção do próximo movimento: 1 – para cima, 2 – para baixo, 3 – para a esquerda, 4 – para a direita. */

int proximoMovimento (Decisor* d, Coordenada pos, int agua, int n_lava)
{
    int linha, coluna;
    linha = pos.y;
    coluna = pos.x;

    //inicializa as casas ao redor do robo como sendo seguras para ele explora-las
    if(d->pilha.topo == -1)  //significa q eh a primeira rodada do mapa
    {
        d->matriz_risco[0][1] = -1000;
        d->matriz_risco[1][0] = -1000;
        d->matriz_risco[1][1] = -1000;
    }


    d->matriz_risco[linha][coluna] = -1; //-1 indica que a casa foi explorada
    adicionaNaPilha(d, pos);

    //salva a n_lava se for uma regiao inexplorada
    if(d->temperature_map[linha][coluna] == -1)
        d->temperature_map[linha][coluna] = n_lava;

    if(agua)
        d->encontrou_agua = 1; //manda a informacao de se encontrou agua pro decisor
    if(!d->encontrou_agua)
    {
        deducaoMatematica(d);
        return decideDirecao(d, pos, n_lava);
    }

    else
    {
        if(d->init) //checa se eh a primeira vez que entra no else para criar o labirinto e etc apenas uma vez
        {
            Coordenada pos_alvo;
            pos_alvo.x = 0; pos_alvo.y = 0;
            constroi_caminho(d);
            preencheMatrizCusto(d, pos_alvo);
            calculaCaminho(d, pos);
            d->init = 0;
        }
        d->passo_da_volta++;
        return identificaSentido(pos, d->caminho_volta[(d->passo_da_volta)-1]);
    }

}

/**
 *   Funcao que constroi o labirinto
 *    Onde o risco eh diferente de 0 fica o valor -2, que representa uma parede
*/
void constroi_caminho(Decisor* d)
{
    int i, j;
    for(i=0; i< d->altura; i++)
    {
        for(j=0; j< d->largura; j++)
        {
            if(d->matriz_risco[i][j] != -1)
                d->lab[i][j] = -2; //-2 representa a parede
            else d->lab[i][j] = -1;
        }
    }

}

/**
    Funcao que preenche cada posicao livre do labirinto com a distancia ate chegar ao destino
    parametros: decisor
                pos_alvo: a coordenada do lugar em que se pretende ir
*/
void preencheMatrizCusto (Decisor* d, Coordenada pos_alvo)
{
    int i, linha, coluna, custo, pos_atual_da_fila;
    pos_atual_da_fila = 0;
    custo = 1;
    Coordenada adiciona; //a coordenada que sera adicionada na fila

    Coordenada* fila;
    fila = (Coordenada*)malloc(sizeof(Coordenada)*d->altura * d->largura*20);
    fila[0] = pos_alvo;

    d->lab[pos_alvo.y][pos_alvo.x] = 0; //inicializa a posicao do robo como zero

    for(i=0; i<=pos_atual_da_fila; i++)//percorre os elementos da fila
    {
        linha = fila[i].y;
        coluna = fila[i].x;
        custo = d->lab[linha][coluna];

        if(linha != (d->altura)-1 && d->lab[linha+1][coluna] == -1)
        {
            d->lab[linha+1][coluna] = custo+1;
            pos_atual_da_fila++;
            adiciona.x = coluna;
            adiciona.y = linha+1;
            fila[pos_atual_da_fila] = adiciona;
        }
        if(linha != 0 && d->lab[linha-1][coluna] == -1)
        {
            d->lab[linha-1][coluna] = custo+1;
            pos_atual_da_fila++;
            adiciona.x = coluna;
            adiciona.y = linha-1;
            fila[pos_atual_da_fila] = adiciona;
        }
        if(coluna != (d->largura)-1 && d->lab[linha][coluna+1] == -1)
        {
            d->lab[linha][coluna+1] = custo+1;
            pos_atual_da_fila++;
            adiciona.x = coluna+1;
            adiciona.y = linha;
            fila[pos_atual_da_fila] = adiciona;
        }
        if(coluna != 0 && d->lab[linha][coluna-1] == -1)
        {
            d->lab[linha][coluna-1] = custo+1;
            pos_atual_da_fila++;
            adiciona.x = coluna-1;
            adiciona.y = linha;
            fila[pos_atual_da_fila] = adiciona;
        }
    }
   free(fila);

}

/**

    Funcao que gera uma lista que representa o caminho que o robo tem que fazer pra ir ao local ja escolhido nas funcoes acima
    parametros: decisor
                pos: coordenada atual do robo

*/
void calculaCaminho (Decisor* d, Coordenada pos)
{
    int custo, atual_x, atual_y, i;
    Coordenada adiciona;

    // O caminho começa na posição atual onde o robo achou a agua
    d->caminho_volta[0] = pos;
    custo = d->lab[pos.y][pos.x];


    for(i = 1; custo > 0; i++)
    {
        atual_x = d->caminho_volta[i-1].x;
        atual_y = d->caminho_volta[i-1].y;


        if(atual_y != (d->altura)-1 && d->lab[atual_y+1][atual_x] == custo-1)
        {
            adiciona.x = atual_x;
            adiciona.y = atual_y+1;
        }
        else if(atual_y != 0 && d->lab[atual_y-1][atual_x] == custo-1)
        {
            adiciona.x = atual_x;
            adiciona.y = atual_y-1;
        }
        else if(atual_x != (d->largura)-1 && d->lab[atual_y][atual_x+1] == custo-1)
        {
            adiciona.x = atual_x+1;
            adiciona.y = atual_y;
        }
        else if(atual_x != 0 && d->lab[atual_y][atual_x-1] == custo-1)
        {
            adiciona.x = atual_x-1;
            adiciona.y = atual_y;
        }
            d->caminho_volta[i] = adiciona;
            custo--; // Decrementa o custo para buscar o proximo número menor
    }
}

/**
 * Recebe a posição atual e a próxima posição desejada,
 * retornando o inteiro correspondente à direção do movimento.
 * * Retorno: 1 (CIMA), 2 (BAIXO), 3 (ESQUERDA), 4 (DIREITA)
 */
int identificaSentido (Coordenada atual, Coordenada proxima)
{
    // Diferença nas linhas
    int delta_y = proxima.y - atual.y;
    // Diferença nas colunas
    int delta_x = proxima.x - atual.x;

    // Se mudou de linha
    if (delta_y == -1) {
        return 1;
    }
    if (delta_y == 1) {
        return 2;
    }

    // Se mudou de coluna
    if (delta_x == -1) {
        return 3;
    }
    if (delta_x == 1) {
        return 4;
    }
    //printf("Erro lendo o caminho");
    return 0; // Caso as posições sejam iguais
}


int decideDirecao(Decisor* d, Coordenada pos, int n_lava)
{
    int soma_pesos, linha, coluna, acumulado, sorteado, i;
    soma_pesos = 0;
    linha = pos.y;
    coluna = pos.x;

    int casas_seguras[4];
    int qtd_seguras = 0;

    //prefere explorar casas que tem certeza que sao seguras
    if(linha != 0 && d->matriz_risco[linha-1][coluna] == -1000)
        casas_seguras[qtd_seguras++] = CIMA;

    if(linha != (d->altura)-1 && d->matriz_risco[linha+1][coluna] == -1000)
        casas_seguras[qtd_seguras++] = BAIXO;

    if(coluna != 0 && d->matriz_risco[linha][coluna-1] == -1000)
        casas_seguras[qtd_seguras++] = ESQUERDA;

    if(coluna != (d->largura)-1 && d->matriz_risco[linha][coluna+1] == -1000)
        casas_seguras[qtd_seguras++] = DIREITA;

    // Se achou uma ou mais casas seguras, sorteia apenas entre elas e retorna imediatamente
    if (qtd_seguras > 0) {
        return casas_seguras[rand() % qtd_seguras];
    }



    //antes de chutar um valor e correr o risco de morrer, o robo checa se existe uma casa segura
    if(temCasaSegura(d))
    {
        if(d->pilha.topo >= 0) {
            removeDaPilha(d);
        }
        if(d->pilha.topo >= 0)
        {
            Coordenada destino = removeDaPilha(d);
            return identificaSentido(pos, destino);
        }
    }



    //caso nao tenha nenhuma casa segura, sorteia uma casa com base no risco
    int pesos[] = {0, 0, 0, 0, 0};
    int risco_temp;
    if(linha != 0 && d->matriz_risco[linha-1][coluna] != -1 && d->matriz_risco[linha-1][coluna] < 8000)
        {
            risco_temp = d->matriz_risco[linha-1][coluna];
            if(risco_temp == 1) {
                risco_temp += (n_lava * n_lava);
            }

            pesos[CIMA] = RISCO_MAX - risco_temp;
            soma_pesos += pesos[CIMA];
        }
    if(linha != (d->altura)-1 && d->matriz_risco[linha+1][coluna] != -1 && d->matriz_risco[linha+1][coluna] < 8000)
        {
            risco_temp = d->matriz_risco[linha+1][coluna];
            if(risco_temp == 1) {
                risco_temp += (n_lava * n_lava);
            }

            pesos[BAIXO] = RISCO_MAX - risco_temp;
            soma_pesos += pesos[BAIXO];
        }
    if(coluna != 0  && d->matriz_risco[linha][coluna-1] != -1 && d->matriz_risco[linha][coluna-1] < 8000)
        {
            risco_temp = d->matriz_risco[linha][coluna-1];
            if(risco_temp == 1) {
                risco_temp += (n_lava * n_lava);
            }

            pesos[ESQUERDA] = RISCO_MAX - risco_temp;
            soma_pesos += pesos[ESQUERDA];
        }
    if(coluna != (d->largura)-1 && d->matriz_risco[linha][coluna+1] != -1 && d->matriz_risco[linha][coluna+1] < 8000)
        {
            risco_temp = d->matriz_risco[linha][coluna+1];
            if(risco_temp == 1) {
                risco_temp += (n_lava * n_lava);
            }

            pesos[DIREITA] = RISCO_MAX - risco_temp;
            soma_pesos += pesos[DIREITA];
        }
    acumulado = 0;
    if(soma_pesos!= 0)
    {
        sorteado = 1+(rand()%soma_pesos);
        for(i=1; i<5; i++)
        {
            acumulado += pesos[i];
            if(sorteado <= acumulado)
                return i;
        }
    }



    //se ainda nao retornou ate aqui, entao o robo esta em um beco ou posicao em que ele nao se move
    d->matriz_risco[linha][coluna] = 8888;

    // Remove da pilha a casa onde ele esta agora
    if(d->pilha.topo >= 0) {
        removeDaPilha(d);
    }

    // desempilha ate achar uma casa boa
    while(d->pilha.topo >= 0)
    {
        Coordenada destino = removeDaPilha(d);

        if(posBoa(d, destino)) {
            return identificaSentido(pos, destino); // anda em direcao a casa boa
        }

        // Se a casa nao for boa entao ela tambem eh considerada um beco (valor 8888 no risco)
        d->matriz_risco[destino.y][destino.x] = 8888;
        return identificaSentido(pos, destino);
    }

    return 1;
}

void deducaoMatematica(Decisor* d)
{
    int i, j;
    int houve_mudanca = 1;

    // O loop continua repetindo ate que o robo não consiga deduzir mais nenhuma casa nova
    while (houve_mudanca)
    {
        houve_mudanca = 0;

        for (i = 0; i < d->altura; i++)
        {
            for (j = 0; j < d->largura; j++)
            {
                // So calcula se a casa ja foi explorada (tem uma dica de lava salva >= 0)
                if (d->temperature_map[i][j] >= 0)
                {
                    int lavas_ao_redor = d->temperature_map[i][j];
                    int vizinhos_desconhecidos = 0;
                    int lavas_confirmadas = 0;

                    //Conta o estado dos vizinhos validos
                    if(i != 0) {
                        if (d->matriz_risco[i-1][j] == 1) vizinhos_desconhecidos++;
                        if (d->matriz_risco[i-1][j] >= 9999) lavas_confirmadas++;
                    }
                    if(i != (d->altura)-1) {
                        if (d->matriz_risco[i+1][j] == 1) vizinhos_desconhecidos++;
                        if (d->matriz_risco[i+1][j] >= 9999) lavas_confirmadas++;
                    }
                    if(j != 0) {
                        if (d->matriz_risco[i][j-1] == 1) vizinhos_desconhecidos++;
                        if (d->matriz_risco[i][j-1] >= 9999) lavas_confirmadas++;
                    }
                    if(j != (d->largura)-1) {
                        if (d->matriz_risco[i][j+1] == 1) vizinhos_desconhecidos++;
                        if (d->matriz_risco[i][j+1] >= 9999) lavas_confirmadas++;
                    }

                    //quantidade de lavas que ainda nao foram marcadas
                    int lavas_restantes = lavas_ao_redor - lavas_confirmadas;

                    //calcula onde tem lava e coloca o risco como 9999
                    if (lavas_restantes == vizinhos_desconhecidos && vizinhos_desconhecidos > 0)
                    {
                        if(i != 0 && d->matriz_risco[i-1][j] == 1) d->matriz_risco[i-1][j] = 9999;
                        if(i != (d->altura)-1 && d->matriz_risco[i+1][j] == 1) d->matriz_risco[i+1][j] = 9999;
                        if(j != 0 && d->matriz_risco[i][j-1] == 1) d->matriz_risco[i][j-1] = 9999;
                        if(j != (d->largura)-1 && d->matriz_risco[i][j+1] == 1) d->matriz_risco[i][j+1] = 9999;
                        houve_mudanca = 1; //lava nova, recalcula tudo
                    }

                    //calcula onde nao tem lava e coloca o risco como -1000
                    if (lavas_restantes == 0 && vizinhos_desconhecidos > 0)
                    {
                        if(i != 0 && d->matriz_risco[i-1][j] == 1) d->matriz_risco[i-1][j] = -1000;
                        if(i != (d->altura)-1 && d->matriz_risco[i+1][j] == 1) d->matriz_risco[i+1][j] = -1000;
                        if(j != 0 && d->matriz_risco[i][j-1] == 1) d->matriz_risco[i][j-1] = -1000;
                        if(j != (d->largura)-1 && d->matriz_risco[i][j+1] == 1) d->matriz_risco[i][j+1] = -1000;
                        houve_mudanca = 1; //casa segura nova, recalcula tudo
                    }
                }
            }
        }
    }
}


void adicionaNaPilha(Decisor* d, Coordenada novaCoordenada)
{
    d->pilha.topo++;
    d->pilha.posicoes[d->pilha.topo] = novaCoordenada;

}

Coordenada removeDaPilha(Decisor* d)
{
    if(d->pilha.topo >= 0)
        return d->pilha.posicoes[d->pilha.topo--];
    else
    {
        Coordenada erro;
        erro.x = -1;
        erro.y = -1;
        return erro;
    }
}

int posBoa(Decisor* d, Coordenada Coordenada)
{
    int linha, coluna;
    coluna = Coordenada.x;
    linha = Coordenada.y;
    //a coordenada vai ser boa se houver pelo menos uma coordenada desconhecida que nao seja lava
    if(linha != 0 && d->temperature_map[linha-1][coluna] != -1 && d->matriz_risco[linha-1][coluna] < 8000)
        return 1;
    if(linha != (d->altura)-1 && d->temperature_map[linha+1][coluna] != -1 && d->matriz_risco[linha+1][coluna] < 8000)
        return 1;
    if(coluna != 0  && d->temperature_map[linha][coluna-1] != -1 && d->matriz_risco[linha][coluna-1] < 8000)
        return 1;
    if(coluna != (d->largura)-1 && d->temperature_map[linha][coluna+1] != -1 && d->matriz_risco[linha][coluna+1] < 8000)
        return 1;
    return 0;
}


int temCasaSegura(Decisor* d)
{
    int i, j;
    for(i=0; i<d->altura; i++)
    {
        for(j=0; j<d->largura; j++)
        {
            if (d->matriz_risco[i][j] == -1000)
                return 1;
        }
    }

    return 0;
}
/*============================================================================*/
