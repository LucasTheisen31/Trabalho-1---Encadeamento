#ifndef MATRIZ_H /* Evita que o arquivo seja incluído mais de uma vez */
#define MATRIZ_H

/*
 * Cada célula da matriz é um "nó".
 * Além do valor inteiro armazenado, o nó guarda sua posição (x,y)
 * e quatro ponteiros que apontam para os nós vizinhos.
 * Se o vizinho não existe (borda da matriz), o ponteiro é NULL.
 */
typedef struct No
{
    int valor;           /* número inteiro armazenado nesta célula        */
    int x;               /* coluna deste nó (índice horizontal)            */
    int y;               /* linha  deste nó (índice vertical)              */
    struct No *esquerda; /* aponta para o nó à esquerda  (mesma linha, x-1) */
    struct No *direita;  /* aponta para o nó à direita   (mesma linha, x+1) */
    struct No *cima;     /* aponta para o nó acima       (linha y-1, mesma coluna) */
    struct No *baixo;    /* aponta para o nó abaixo      (linha y+1, mesma coluna) */
} No;

/*
 * Estrutura principal que representa a matriz inteira.
 * Guarda as dimensões e um ponteiro para o primeiro nó (0,0),
 * a partir do qual todos os outros nós são alcançáveis.
 */
typedef struct Matriz
{
    No *origem;  /* ponteiro para o nó da posição (0,0) — canto superior esquerdo */
    int linhas;  /* quantidade de linhas  da matriz */
    int colunas; /* quantidade de colunas da matriz */
} Matriz;

/* Aloca a matriz com as dimensões informadas.
   Retorna ponteiro para Matriz ou NULL em falha. */
Matriz *alocarMatriz(int linhas, int colunas);

/* Desaloca todos os nós e a estrutura da matriz. */
void desalocarMatriz(Matriz *m);

/* Insere 'valor' na posição (x, y).
   Retorna 1 em sucesso, 0 se fora dos limites. */
int inserir(Matriz *m, int x, int y, int valor);

/* Retorna o nó na posição (x, y) ou NULL se fora dos limites. */
No *consultar(Matriz *m, int x, int y);

/* Busca o primeiro nó com 'valor' na matriz.
   Retorna o nó encontrado ou NULL. */
No *buscar(Matriz *m, int valor);

/* Imprime os valores dos 4 vizinhos (cima, baixo, esquerda, direita)
   do nó na posição (x, y). */
void imprimirVizinhos(Matriz *m, int x, int y);

/* Imprime toda a matriz formatada. */
void imprimirMatriz(Matriz *m);

#endif /* MATRIZ_H */
