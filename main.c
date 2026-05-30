#include <stdio.h>
#include <stdlib.h>
#include "matriz.h"

/* Remove caracteres restantes do buffer do teclado após um scanf.
 * Evita que uma entrada anterior "sobrescreva" a próxima leitura. */
static void limparBuffer()
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}

int main()
{
    Matriz *m = NULL; /* começa sem nenhuma matriz alocada */
    int opcao;

    printf("=== Matriz Encadeada Bidimensional ===\n");

    do
    {
        /* Recalcula o cabeçalho do menu com estado atual */
        printf("\n+--------------------------------------+\n");
        if (m)
            printf("|  Matriz atual: %dx%-20d|\n", m->linhas, m->colunas);
        else
            printf("|  Nenhuma matriz alocada              |\n");
        printf("+--------------------------------------+\n");
        printf("|  1. Alocar nova matriz               |\n");
        printf("|  2. Inserir valor                    |\n");
        printf("|  3. Consultar posicao (x,y)          |\n");
        printf("|  4. Buscar valor                     |\n");
        printf("|  5. Imprimir vizinhos de (x,y)       |\n");
        printf("|  6. Imprimir matriz                  |\n");
        printf("|  7. Desalocar matriz                 |\n");
        printf("|  0. Sair                             |\n");
        printf("+--------------------------------------+\n");
        printf("Opcao: ");

        /* --- Lê a opção digitada pelo usuário --- */
        if (scanf("%d", &opcao) != 1)
        {
            limparBuffer();
            printf("Entrada invalida. Tente novamente.\n");
            continue;
        }
        limparBuffer();

        /* Executa a função correspondente à opção escolhida */
        switch (opcao)
        {
        case 1: /* Alocar: pede dimensões e chama alocarMatriz() */
        {
            if (m)
            {
                printf("Ja existe uma matriz alocada. Desaloque-a primeiro (opcao 7).\n");
                break;
            }
            int lin, col;
            printf("Numero de linhas: ");
            scanf("%d", &lin);
            limparBuffer();
            printf("Numero de colunas: ");
            scanf("%d", &col);
            limparBuffer();
            m = alocarMatriz(lin, col);
            if (m)
                printf("Matriz %dx%d alocada com sucesso.\n", lin, col);
            else
                printf("Erro ao alocar matriz. Verifique as dimensoes.\n");
            break;
        }

        case 2: /* Inserir: pede x, y e o valor a guardar */
        {
            if (!m)
            {
                printf("Nenhuma matriz alocada.\n");
                break;
            }
            int x, y, v;
            printf("Coluna (x): ");
            scanf("%d", &x);
            limparBuffer();
            printf("Linha  (y): ");
            scanf("%d", &y);
            limparBuffer();
            printf("Valor     : ");
            scanf("%d", &v);
            limparBuffer();
            if (inserir(m, x, y, v))
                printf("Valor %d inserido em (%d,%d).\n", v, x, y);
            else
                printf("Posicao (%d,%d) fora dos limites.\n", x, y);
            break;
        }

        case 3: /* Consultar: exibe o valor armazenado em (x,y) */
        {
            if (!m)
            {
                printf("Nenhuma matriz alocada.\n");
                break;
            }
            int x, y;
            printf("Coluna (x): ");
            scanf("%d", &x);
            limparBuffer();
            printf("Linha  (y): ");
            scanf("%d", &y);
            limparBuffer();
            No *no = consultar(m, x, y);
            if (no)
                printf("Valor em (%d,%d): %d\n", x, y, no->valor);
            else
                printf("Posicao (%d,%d) fora dos limites.\n", x, y);
            break;
        }

        case 4: /* Buscar: procura um valor e mostra sua posição */
        {
            if (!m)
            {
                printf("Nenhuma matriz alocada.\n");
                break;
            }
            int v;
            printf("Valor a buscar: ");
            scanf("%d", &v);
            limparBuffer();
            No *no = buscar(m, v);
            if (no)
                printf("Valor %d encontrado em (%d,%d).\n", v, no->x, no->y);
            else
                printf("Valor %d nao encontrado na matriz.\n", v);
            break;
        }

        case 5: /* Vizinhos: mostra os 4 nós ao redor de (x,y) */
        {
            if (!m)
            {
                printf("Nenhuma matriz alocada.\n");
                break;
            }
            int x, y;
            printf("Coluna (x): ");
            scanf("%d", &x);
            limparBuffer();
            printf("Linha  (y): ");
            scanf("%d", &y);
            limparBuffer();
            imprimirVizinhos(m, x, y);
            break;
        }

        case 6: /* Imprimir: exibe todos os valores da matriz */
        {
            if (!m)
            {
                printf("Nenhuma matriz alocada.\n");
                break;
            }
            imprimirMatriz(m);
            break;
        }

        case 7: /* Desalocar: libera toda a memória usada pela matriz */
        {
            if (!m)
            {
                printf("Nenhuma matriz alocada.\n");
                break;
            }
            desalocarMatriz(m);
            m = NULL;
            printf("Matriz desalocada com sucesso.\n");
            break;
        }

        case 0: /* Sair: garante que a memória é liberada antes de encerrar */
            if (m)
            {
                desalocarMatriz(m);
                m = NULL;
            }
            printf("Encerrando programa.\n");
            break;

        default:
            printf("Opcao invalida.\n");
            break;
        }

    } while (opcao != 0);

    return 0;
}
