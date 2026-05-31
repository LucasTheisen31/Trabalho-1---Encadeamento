# Trabalho 1 — Matriz Bidimensional com Listas Encadeadas

**Disciplina:** Algoritmos e Estrutura de Dados III  
**Docente:** Thiago Naves  
**Instituição:** UTFPR — Campus Santa Helena

---

## 1. O Problema

Em C, uma matriz convencional (`int m[3][4]`) armazena todos os elementos em um bloco contínuo de memória e o acesso é feito por índice numérico.

Neste trabalho, cada elemento da matriz é um **nó alocado individualmente** na memória. Em vez de índices, os nós se conectam por meio de **ponteiros**, formando uma grade encadeada. A ideia é simular o comportamento de uma matriz convencional usando essa estrutura.

---

## 2. Estrutura de Dados

### 2.1 O Nó (`struct No`)

Cada célula da grade é um `No`. Além do valor armazenado, ele conhece sua posição e guarda ponteiros para os quatro vizinhos diretos.

```c
typedef struct No {
    int valor;           // número inteiro desta célula
    int x;               // coluna (índice horizontal)
    int y;               // linha  (índice vertical)
    struct No *esquerda;
    struct No *direita;
    struct No *cima;
    struct No *baixo;
} No;
```

Se um vizinho não existe (borda da matriz), o ponteiro vale `NULL`.

### 2.2 A Matriz (`struct Matriz`)

```c
typedef struct Matriz {
    No *origem;  // ponteiro para o nó (0,0)
    int linhas;
    int colunas;
} Matriz;
```

O campo `origem` aponta para o canto superior esquerdo. A partir dele, todos os outros nós são alcançáveis seguindo os ponteiros.

### 2.3 Visualização da grade (3×3)

```mermaid
flowchart TB
    gradeOrigem["origem"] --> grade00["No 0,0"]

    subgraph linha0["Linha 0"]
        direction LR
        grade00 --> grade10["No 1,0"] --> grade20["No 2,0"]
    end

    subgraph linha1["Linha 1"]
        direction LR
        grade01["No 0,1"] --> grade11["No 1,1"] --> grade21["No 2,1"]
    end

    subgraph linha2["Linha 2"]
        direction LR
        grade02["No 0,2"] --> grade12["No 1,2"] --> grade22["No 2,2"]
    end

    grade00 --> grade01
    grade10 --> grade11
    grade20 --> grade21
    grade01 --> grade02
    grade11 --> grade12
    grade21 --> grade22

    style linha0 fill:transparent,stroke:transparent
    style linha1 fill:transparent,stroke:transparent
    style linha2 fill:transparent,stroke:transparent
```

> Para manter o diagrama legível, ele mostra apenas algumas ligações para direita e para baixo. As referências para esquerda e para cima existem também, como ponteiros inversos.
>
> As setas horizontais representam o ponteiro `direita`; as verticais representam o ponteiro `baixo`.
>
> Os nós das bordas têm ponteiros `NULL` nos sentidos sem vizinho (ex: `(0,0)->esquerda == NULL` e `(0,0)->cima == NULL`).

---

## 3. Arquivos do Projeto

| Arquivo | Função |
|---|---|
| `matriz.h` | Define os tipos (`No`, `Matriz`) e declara os protótipos das funções |
| `matriz.c` | Implementa todas as funções |
| `main.c` | Contém `main()` com o menu interativo |

O bloco `#ifndef MATRIZ_H / #define / #endif` em `matriz.h` evita que o arquivo seja incluído mais de uma vez na mesma compilação.

---

## 4. Funções — Como Funcionam

### `alocarMatriz(linhas, colunas)`

```mermaid
flowchart TD
    alocarInicio["Inicio"] --> alocarVerifica["Verifica dimensoes"]
    alocarVerifica --> alocarInvalida["Se invalidas retorna NULL"]
    alocarVerifica --> alocarMatriz["Se validas aloca Matriz"]
    alocarMatriz --> alocarNos["Cria nos"]
    alocarNos --> alocarConecta["Conecta vizinhos"]
    alocarConecta --> alocarOrigem["Define origem"]
    alocarOrigem --> alocarLibera["Libera grade auxiliar"]
    alocarLibera --> alocarFim["Fim"]
```

**Por que dois laços separados?**  
No primeiro laço os nós ainda não existem todos. Só no segundo, com todos já criados, é possível conectá-los com segurança.

O array `grade` é apenas uma escada temporária para construção — depois de conectar tudo, ele é descartado com `free()`.

---

### `desalocarMatriz(m)`

```mermaid
flowchart TD
    desalocarInicio["Inicio"] --> desalocarLinha["Linha atual recebe origem"]
    desalocarLinha --> desalocarVerifica["Verifica se existe linha atual"]
    desalocarVerifica --> desalocarLibera["Se nao existir libera Matriz"]
    desalocarLibera --> desalocarFim["Fim"]
    desalocarVerifica --> desalocarProxima["Se existir guarda proxima linha"]
    desalocarProxima --> desalocarNos["Libera nos da linha"]
    desalocarNos --> desalocarAvanca["Avanca para proxima linha"]
    desalocarAvanca --> desalocarVerifica
```

> **Detalhe importante:** o ponteiro `->baixo` é salvo *antes* do `free()`. Após liberar um nó, acessá-lo causa comportamento indefinido — por isso salvamos o endereço com antecedência.

---

### `consultar(m, x, y)`

Navega pelos ponteiros encadeados a partir de `(0,0)`:

```mermaid
flowchart LR
    consultarOrigem["origem"] --> consultarLinhas["Desce y linhas"]
    consultarLinhas --> consultarColunas["Anda x colunas"]
    consultarColunas --> consultarNo["No encontrado"]
```

1. Valida se `x` e `y` estão dentro dos limites.
2. Parte de `m->origem`.
3. Segue `->baixo` por `y` passos → chega na linha correta.
4. Segue `->direita` por `x` passos → chega na coluna correta.
5. Retorna o nó (ou `NULL` se inválido).

---

### `inserir(m, x, y, valor)`

Usa `consultar()` para localizar o nó e simplesmente atribui o valor:

```c
No *no = consultar(m, x, y);
if (!no) return 0;   // fora dos limites
no->valor = valor;
return 1;            // sucesso
```

---

### `buscar(m, valor)`

Varredura completa linha por linha:

```mermaid
flowchart TD
    buscarInicio["Inicio"] --> buscarLinha["Linha atual recebe origem"]
    buscarLinha --> buscarExisteLinha["Verifica se existe linha"]
    buscarExisteLinha --> buscarNulo["Se nao existir retorna NULL"]
    buscarExisteLinha --> buscarAtual["Se existir atual recebe linha"]
    buscarAtual --> buscarExisteNo["Verifica se existe no atual"]
    buscarExisteNo --> buscarDesce["Se nao existir desce uma linha"]
    buscarDesce --> buscarExisteLinha
    buscarExisteNo --> buscarCompara["Se existir compara o valor"]
    buscarCompara --> buscarEncontrou["Se encontrou retorna no"]
    buscarCompara --> buscarDireita["Se nao encontrou avanca para direita"]
    buscarDireita --> buscarExisteNo
```

---

### `imprimirVizinhos(m, x, y)`

Usa `consultar()` para obter o nó e então acessa diretamente os quatro ponteiros de vizinhança. Se o ponteiro for `NULL` (borda), imprime `"NULL"`.

---

## 5. Menu Interativo (`main.c`)

```mermaid
flowchart TD
    menuInicio["Inicio"] --> menuVazia["Matriz comeca vazia"]
    menuVazia --> menuExibe["Exibe menu"]
    menuExibe --> menuLe["Le opcao"]
    menuLe --> menuExecuta["Executa a opcao escolhida"]
    menuExecuta --> menuAcoes["Alocar inserir consultar buscar"]
    menuExecuta --> menuOutras["Ver vizinhos imprimir desalocar"]
    menuAcoes --> menuExibe
    menuOutras --> menuExibe
    menuExecuta --> menuFim["Se opcao for zero encerra"]
```

A função `limparBuffer()` descarta o `\n` que fica no buffer do teclado após cada `scanf`, evitando que leituras seguintes recebam uma entrada vazia.

---

## 6. Conceitos Fundamentais de C

### Ponteiro
Uma variável que guarda o **endereço** de outra variável na memória.

```c
int x = 42;
int *p = &x;  // p guarda o endereço de x
*p = 10;      // altera x através do ponteiro → x agora vale 10
```

No projeto, `no->direita` guarda o endereço do nó vizinho. Seguir esse ponteiro significa "ir até aquele nó na memória". `NULL` significa "não há vizinho aqui".

### `malloc` e `free`
- `malloc(tamanho)` — reserva um bloco de memória e retorna seu endereço.
- `free(ponteiro)` — devolve esse bloco ao sistema operacional.

Todo `malloc()` deve ter um `free()` correspondente para evitar **vazamento de memória**.

---

## 7. Desafios e Dificuldades

| Desafio | Como foi resolvido |
|---|---|
| Gerenciar memória em caso de falha | Em cada `malloc()`, se falhar, todo o que já foi alocado é liberado antes de retornar `NULL` |
| Ordem de construção dos nós | Dividir `alocarMatriz` em dois laços: primeiro criar, depois conectar |
| Desalocação segura | Salvar o ponteiro `->baixo` e `->direita` antes de chamar `free()` |
| Clareza sobre `x = coluna` e `y = linha` | Comentários explícitos no código e consistência nas assinaturas das funções |
| Menu interativo com `scanf` | Uso de `limparBuffer()` após cada leitura para evitar entradas espúrias |
