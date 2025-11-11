# Árvore M-Vias com Índice em Disco

**Disciplina:** Algoritmos e Estruturas de Dados II  
**Trabalho:** Implementação de Árvore M-Vias com Persistência em Arquivo Binário  
**Autores:**
- Francisco Eduardo Fontenele - 15452569
- Vinicius Botte - 15522900

---

## Visão Geral

Este projeto implementa uma **árvore M-vias** persistente em disco, oferecendo operações de busca, inserção e remoção com gerenciamento automático de splits (divisões) e fusões de nós. A ordem `m` da árvore é escolhida pelo usuário e validada no header do índice binário, garantindo compatibilidade entre execuções.

O sistema é composto por dois arquivos principais:
- **`mvias.bin`**: índice estruturado como árvore M-vias, com header na posição 0.
- **`data.bin`**: arquivo de dados com registros de tamanho fixo contendo chave e payload textual.

Ambos os arquivos expõem contadores de I/O (leituras/escritas) para análise de desempenho das operações.

---

## Funcionalidades

### Operações na Árvore
- **Busca (`mSearch`)**: localiza uma chave na árvore, retornando `(nó, slot, encontrado)`. Percorre de forma top-down comparando chaves e seguindo ponteiros de filhos.
- **Inserção (`insertB`)**: insere uma chave de forma bottom-up. Ao atingir capacidade máxima (`n >= m`), divide o nó promovendo a chave central ao pai. Cria nova raiz quando necessário.
- **Remoção (`deleteB`)**: remove uma chave substituindo-a pelo antecessor (se em nó interno) e corrige underflows via redistribuição ou fusão de nós. Contrai a raiz se ela ficar vazia.
- **Verificação de Integridade (`verifyIntegrity`)**: valida invariantes estruturais (ordenação de chaves, limites de faixas por subárvore, alcance de nós, mínimos por nó não-raiz).

### Arquivo de Dados
- **Busca sequencial**: localiza registros ativos por chave.
- **Inserção**: adiciona registros ao final do arquivo.
- **Remoção lógica**: marca registros como inativos (`active = 0`).
- **Listagem**: coleta todas as chaves ativas (usado na carga inicial de `employees.txt`).

### Interface Interativa
Menu principal oferece:
1. Buscar chave (mostra nó/slot, I/O de índice e dados).
2. Inserir chave (atualiza índice e arquivo de dados, exibe I/O).
3. Imprimir arquivo principal (lista todos os registros).
4. Remover chave (atualiza índice e marca registro como removido).
5. Verificar integridade (valida estrutura da árvore).
6. Sair (persiste header e fecha arquivos).

---

## Formato de Arquivos

### Índice Binário (`mvias.bin`)
- **Posição 0 (header)**: nó especial com `n = -1`, `keys[0] = m`, `children[0] = root`.
- **Posições 1..N**: nós da árvore com layout fixo definido por `MAX_M` (32). Campos: `n` (número de chaves), `keys[MAX_M]`, `children[MAX_M+1]`.

### Arquivo de Texto (entrada)
Linhas no formato `n A0 K1 A1 K2 A2 ... Kn An`, onde:
- `n`: quantidade de chaves no nó.
- `A0`: ponteiro do filho mais à esquerda.
- `Ki`: chave `i`.
- `Ai`: ponteiro do filho à direita da chave `Ki`.

Exemplo (`mvias.txt`, m=3):
```
2 2 20 3 40 4
2 0 10 0 15 0
2 0 25 0 30 5
2 0 45 0 50 0
1 0 35 0
```

### Arquivo de Dados (`data.bin`)
Registros de tamanho fixo: `{ int key; int active; char payload[64]; }`.
- `key`: chave indexada.
- `active`: 1 (ativo) ou 0 (removido logicamente).
- `payload`: texto descritivo (ex.: "Funcionario 10 | depto=A").

### Arquivo de Funcionários (`employees.txt`)
Formato CSV simples: `id;Nome;Depto`. Usado para criar `data.bin` e popular o índice com chaves existentes.

Exemplo:
```
1001;João Silva;TI
1002;Maria Santos;RH
```

---

## Compilação e Execução

### Requisitos
- C++20 ou superior
- CMake 3.29+
- Testado em Windows com CLion 2024.2.3

### Build
```bash
mkdir build
cd build
cmake ..
cmake --build .
./MWaysSearch
```

Ou abra o projeto no CLion e execute o target `MWaysSearch`.

---

## Uso

### Inicialização
Ao iniciar, o programa oferece quatro opções:

1. **Abrir índice existente**: carrega `mvias.bin` e `data.bin` do diretório corrente. Valida `m` do header.
2. **Criar a partir de .txt**: escolhe um dos arquivos de teste (`mvias.txt`, `mvias2.txt`, etc.) e ordem `m`, gera `mvias.bin` e `data.bin`.
3. **Criar índice vazio**: cria `mvias.bin` vazio com ordem informada (root=0).
4. **Criar a partir de employees.txt**: gera `data.bin` do CSV e popula o índice com as chaves lidas.

### Menu Principal
Após a inicialização, o programa exibe a árvore e oferece:
- **Buscar**: informa chave, mostra `(nó, slot, encontrado)` e I/O; se encontrada, exibe o registro de `data.bin`.
- **Inserir**: informa chave, atualiza índice/dados e exibe I/O. Duplicatas são ignoradas no índice.
- **Imprimir arquivo principal**: lista todos os registros (ativos e removidos).
- **Remover**: informa chave, remove do índice e marca registro como inativo.
- **Verificar integridade**: valida invariantes; exibe diagnóstico detalhado.
- **Sair**: persiste header atualizado e encerra.

---

## Impressão da árvore (BFS)

A impressão percorre a árvore por níveis (BFS), pois fica mais direta de ler e checar:
- Ordem: raiz → filhos da raiz → netos, e assim por diante.
- Cada linha é um nó e segue: "No n,A[0],(K[1],A[1]),...,(K[n],A[n])", onde:
  - No: posição lógica do nó no arquivo (1..N).
  - n: quantidade de chaves do nó.
  - A[i]: ponteiro do filho i (0..n), com A[0] sendo o mais à esquerda.
  - K[i]: i-ésima chave do nó.
- Leitura prática:
  - Filhos de um nó aparecem nas linhas seguintes ao pai (no mesmo nível ou nos próximos).
  - Nó folha: A[0] = 0 (na prática todos A[i] = 0).
  - Estrutura de cada nó: A[0], K[1], A[1], K[2], A[2], ..., K[n], A[n].

Por que manter BFS:
- Facilita conferir ponteiros e comparar com arquivos de teste: ao ver um nó, seus filhos virão logo depois.
- Ajuda na verificação de integridade por nível (mais simples de acompanhar).

---

## Arquivos de Teste

### Árvores com m=3
- **`mvias.txt`**: Árvore 1 (numeração padrão), 5 nós.
- **`mvias2.txt`**: Árvore 2 (numeração padrão), 7 nós.
- **`mvias3.txt`**: Árvore 1 (numeração alternativa).
- **`mvias4.txt`**: Árvore 2 (numeração alternativa).

### Árvore com m=5
- **`mvias5.txt`**: Árvore 3, nós com até 4 chaves.

**Importante:** ao selecionar o arquivo de entrada, informe a ordem `m` correspondente (3 ou 5).

---

## Exemplo de Uso

### Criação e Busca
```
Selecione uma opcao de inicializacao:
1. Abrir indice existente (mvias.bin)
2. Criar indice a partir de um .txt
3. Criar indice vazio
4. Criar indice a partir de employees.txt (arquivo principal)
Escolha (1-4): 2

Selecione a arvore para teste:
1. Arvore 1 (mvias.txt)
...
Escolha (1-5): 1

Informe a ordem m (3..32): 3

T = 1, m = 3
------------------------------------------------------------------
No n,A[0],(K[1],A[1]),...,(K[n],A[n])
------------------------------------------------------------------
 1 2,  2,( 20,  3),( 40,  4)
 2 2,  0,( 10,  0),( 15,  0)
 3 2,  0,( 25,  0),( 30,  5)
 4 2,  0,( 45,  0),( 50,  0)
 5 1,  0,( 35,  0)
------------------------------------------------------------------

Selecione uma opcao:
1. Buscar chave
...
Escolha (1-6): 1

Chave de busca: 45
 45 (4,1,true)
I/O indice: R=2 W=0
Registro: key=45 payload="Funcionario 45 | depto=E" active=1
I/O dados: R=5 W=0
Continuar busca (s/n)? n
```

### Inserção com Split
```
Escolha (1-6): 2
Chave para inserir: 36
I/O indice (insercao): R=3 W=2
I/O dados (insercao): R=0 W=1

(árvore atualizada exibida; nó 5 agora tem keys={35, 36})
```

### Remoção
```
Escolha (1-6): 4
Chave para remover: 45
I/O indice (remocao): R=8 W=3
Remocao no arquivo principal: ok
I/O dados (remocao): R=5 W=1

(árvore atualizada; chave 45 removida, estrutura rebalanceada se necessário)
```

---

## Notas Técnicas

- **Layout fixo**: `MAX_M=32` define o tamanho do nó em disco; não altere sem recompilar e recriar os índices.
- **Ordem dinâmica**: `m` é escolhido pelo usuário e validado no header; índices de ordens diferentes não são intercambiáveis.
- **Contadores I/O**: zerados a cada operação; úteis para análise de complexidade prática.
- **Root creation**: ao dividir a raiz, cria-se nova raiz que referencia os nós resultantes do split.
- **Antecessor na remoção**: em nós internos, substitui a chave pelo maior elemento da subárvore esquerda.

---

## Estrutura do Projeto

```
MWaysSearch/
├── CMakeLists.txt
├── README.md
├── main.cpp
├── MWayTree.h
├── MWayTree.cpp
├── DataFile.h
├── DataFile.cpp
├── mvias.txt
├── mvias2.txt
├── mvias3.txt
├── mvias4.txt
├── mvias5.txt
└── employees.txt (opcional)
```

Gerados em runtime:
- `mvias.bin` (índice)
- `data.bin` (dados)

---

## Checklist de Validação

- [ ] Exibição da árvore corresponde ao formato esperado (nós 1-based).
- [ ] Busca retorna `(nó, slot, encontrado)` correto.
- [ ] Inserção de chave nova atualiza apenas os nós esperados.
- [ ] Inserção de chave existente não altera a árvore.
- [ ] Remoção de chave ajusta estrutura (redistribuição/fusão).
- [ ] Verificação de integridade passa sem erros em árvores válidas.
- [ ] Contadores I/O refletem operações realizadas.
- [ ] Arquivo de dados sincronizado com índice (chaves presentes em ambos).

---

## Contato

Para dúvidas ou sugestões, contate os autores pelos emails cadastrados no sistema acadêmico.
