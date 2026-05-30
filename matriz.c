#include <stdio.h>
#include <stdlib.h>
#include "matriz.h"

/* ---------------------------------------------------------------
 * alocarMatriz
 * Aloca todos os nós de uma matriz linhas x colunas e conecta
 * seus ponteiros de vizinhança (esquerda, direita, cima, baixo).
 * O campo 'origem' aponta para o nó (0,0).
 * Retorna NULL se os parâmetros forem inválidos ou falta memória.
 * --------------------------------------------------------------- */
Matriz *alocarMatriz(int linhas, int colunas)
{
    /* Rejeita dimensões inválidas */
    if (linhas <= 0 || colunas <= 0)
        return NULL;

    /* Aloca a estrutura principal que representa a matriz */
    Matriz *m = (Matriz *)malloc(sizeof(Matriz));
    if (!m)
        return NULL;

    m->linhas = linhas;
    m->colunas = colunas;

    /*
     * "grade" é um array temporário de ponteiros usado só durante
     * a construção. Funciona como grade[linha][coluna] e nos permite
     * acessar qualquer nó facilmente para montar os links depois.
     * Após conectar tudo, ele é liberado — os nós ficam na memória.
     */
    No ***grade = (No ***)malloc(linhas * sizeof(No **));
    if (!grade)
    {
        free(m);
        return NULL;
    }

    /* --- Etapa 1: cria cada nó e armazena no array auxiliar --- */
    for (int i = 0; i < linhas; i++)
    {
        grade[i] = (No **)malloc(colunas * sizeof(No *));
        if (!grade[i])
        {
            /* Falha: libera tudo que já foi alocado e retorna NULL */
            for (int k = 0; k < i; k++)
                free(grade[k]);
            free(grade);
            free(m);
            return NULL;
        }
        for (int j = 0; j < colunas; j++)
        {
            /* Aloca um nó individualmente na memória heap */
            No *no = (No *)malloc(sizeof(No));
            if (!no)
            {
                /* Falha: libera tudo parcialmente alocado */
                for (int ki = 0; ki <= i; ki++)
                {
                    int jlim = (ki == i) ? j : colunas;
                    for (int kj = 0; kj < jlim; kj++)
                        free(grade[ki][kj]);
                    free(grade[ki]);
                }
                free(grade);
                free(m);
                return NULL;
            }
            no->valor = 0;       /* valor inicial zero                 */
            no->x = j;           /* j = coluna                         */
            no->y = i;           /* i = linha                          */
            no->esquerda = NULL; /* os ponteiros de vizinhos serão     */
            no->direita = NULL;  /* preenchidos na etapa 2 abaixo      */
            no->cima = NULL;
            no->baixo = NULL;
            grade[i][j] = no; /* guarda o ponteiro no array auxiliar */
        }
    }

    /* --- Etapa 2: conecta cada nó aos seus vizinhos --- */
    for (int i = 0; i < linhas; i++)
    {
        for (int j = 0; j < colunas; j++)
        {
            No *no = grade[i][j];
            /* Se j==0 não há coluna à esquerda  → NULL */
            no->esquerda = (j > 0) ? grade[i][j - 1] : NULL;
            /* Se j é a última coluna            → NULL */
            no->direita = (j < colunas - 1) ? grade[i][j + 1] : NULL;
            /* Se i==0 não há linha acima        → NULL */
            no->cima = (i > 0) ? grade[i - 1][j] : NULL;
            /* Se i é a última linha             → NULL */
            no->baixo = (i < linhas - 1) ? grade[i + 1][j] : NULL;
        }
    }

    /* O nó (0,0) é o ponto de entrada para toda a estrutura */
    m->origem = grade[0][0];

    /* Libera apenas o array auxiliar — os nós permanecem na memória */
    for (int i = 0; i < linhas; i++)
        free(grade[i]);
    free(grade);

    return m;
}

/* ---------------------------------------------------------------
 * desalocarMatriz
 * Percorre todos os nós linha a linha a partir de (0,0) e libera
 * cada um. Por fim libera a estrutura Matriz.
 * --------------------------------------------------------------- */
void desalocarMatriz(Matriz *m)
{
    if (!m)
        return;

    No *linhaAtual = m->origem; /* começa no nó (0,0) */
    while (linhaAtual)
    {
        /* Guarda o início da próxima linha antes de destruir esta */
        No *proximaLinha = linhaAtual->baixo;

        /* Percorre todos os nós da linha atual, liberando um a um */
        No *atual = linhaAtual;
        while (atual)
        {
            No *proximo = atual->direita; /* salva o próximo antes de liberar */
            free(atual);                  /* devolve a memória do nó ao sistema */
            atual = proximo;
        }
        linhaAtual = proximaLinha; /* avança para a linha seguinte */
    }
    free(m); /* por último, libera a estrutura Matriz em si */
}

/* ---------------------------------------------------------------
 * consultar
 * Navega a partir de (0,0) até (x, y) usando os ponteiros
 * direita e baixo. Retorna o nó ou NULL se fora dos limites.
 * --------------------------------------------------------------- */
No *consultar(Matriz *m, int x, int y)
{
    if (!m)
        return NULL;

    /* Verifica se (x,y) está dentro dos limites da matriz */
    if (x < 0 || x >= m->colunas || y < 0 || y >= m->linhas)
        return NULL;

    /* Navega a partir do nó (0,0) usando os ponteiros encadeados */
    No *no = m->origem;

    /* Desce 'y' vezes pelo ponteiro ->baixo para chegar à linha correta */
    for (int i = 0; i < y; i++)
        no = no->baixo;

    /* Avança 'x' vezes pelo ponteiro ->direita para chegar à coluna correta */
    for (int j = 0; j < x; j++)
        no = no->direita;

    return no; /* retorna o nó encontrado */
}

/* ---------------------------------------------------------------
 * inserir
 * Usa consultar() para localizar o nó e atualiza seu valor.
 * Retorna 1 em sucesso ou 0 se a posição estiver fora dos limites.
 * --------------------------------------------------------------- */
int inserir(Matriz *m, int x, int y, int valor)
{
    No *no = consultar(m, x, y);
    if (!no)
        return 0;
    no->valor = valor;
    return 1;
}

/* ---------------------------------------------------------------
 * buscar
 * Percorre a matriz linha a linha procurando 'valor'.
 * Retorna o primeiro nó encontrado ou NULL.
 * --------------------------------------------------------------- */
No *buscar(Matriz *m, int valor)
{
    if (!m)
        return NULL;

    /* Percorre a matriz linha por linha, da esquerda para a direita */
    No *linhaAtual = m->origem;
    while (linhaAtual)
    {
        No *atual = linhaAtual;
        while (atual)
        {
            if (atual->valor == valor) /* encontrou! retorna imediatamente */
                return atual;
            atual = atual->direita; /* avança para a próxima coluna */
        }
        linhaAtual = linhaAtual->baixo; /* desce para a próxima linha */
    }
    return NULL; /* percorreu tudo e não encontrou */
}

/* ---------------------------------------------------------------
 * imprimirVizinhos
 * Imprime os valores dos 4 vizinhos do nó (x, y).
 * Exibe "NULL" quando o vizinho não existe (borda da matriz).
 * --------------------------------------------------------------- */
void imprimirVizinhos(Matriz *m, int x, int y)
{
    No *no = consultar(m, x, y);
    if (!no)
    {
        printf("Posicao (%d,%d) invalida.\n", x, y);
        return;
    }

    printf("Vizinhos de (%d,%d) [valor=%d]:\n", x, y, no->valor);
    printf("  Cima     : ");
    no->cima ? printf("%d\n", no->cima->valor) : printf("NULL\n");
    printf("  Baixo    : ");
    no->baixo ? printf("%d\n", no->baixo->valor) : printf("NULL\n");
    printf("  Esquerda : ");
    no->esquerda ? printf("%d\n", no->esquerda->valor) : printf("NULL\n");
    printf("  Direita  : ");
    no->direita ? printf("%d\n", no->direita->valor) : printf("NULL\n");
}

/* ---------------------------------------------------------------
 * imprimirMatriz
 * Exibe a matriz no console em formato tabular.
 * --------------------------------------------------------------- */
void imprimirMatriz(Matriz *m)
{
    if (!m)
        return;

    printf("Matriz %dx%d:\n", m->linhas, m->colunas);
    No *linhaAtual = m->origem;
    while (linhaAtual)
    {
        No *atual = linhaAtual;
        while (atual)
        {
            printf("%6d", atual->valor);
            atual = atual->direita;
        }
        printf("\n");
        linhaAtual = linhaAtual->baixo;
    }
}
