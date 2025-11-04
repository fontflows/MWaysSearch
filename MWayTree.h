/**
* @file MWayTree.h
 * @authors
 *   Francisco Eduardo Fontenele - 15452569
 *   Vinicius Botte - 15522900
 *
 * AED II - Trabalho 1
 */

#ifndef MWAYTREE_H
#define MWAYTREE_H

#include <fstream>
#include <string>
#include <tuple>
#include <utility>
#include <stack>

using namespace std;

const int MAX_M = 32;

/**
 * @brief Nó da árvore M-vias persistido no arquivo.
 * @details n = número de chaves válidas; keys[0..n-1] estritamente crescentes;
 *          children[0..n] são posições lógicas dos filhos (0 = inexistente).
 *          Posição 0 do arquivo é reservada ao header da árvore.
 */
struct Node {
    int n;
    int keys[MAX_M];
    int children[MAX_M+1];
    /**
     * @brief Constrói nó vazio (n=0) com arrays zerados.
     */
    Node();
};

/**
 * @brief Árvore M-vias persistente com busca, inserção e remoção no arquivo binário.
 * @details Header no nó lógico 0 (n=-1, keys[0]=m, children[0]=root). Nós válidos começam na posição 1.
 */
class MWayTree {
private:
    std::fstream file;
    std::string filename;
    int root;
    int m;
    long long idxReads = 0;
    long long idxWrites = 0;

    /**
     * @brief Persiste o nó na posição lógica indicada (>=1).
     * @param node Nó a persistir.
     * @param position Posição lógica (1..N).
     */
    void writeNode(const Node& node, int position);

    /**
     * @brief Persiste o nó ao final do arquivo.
     * @param node Nó a persistir.
     * @return Posição lógica escrita (1..N), considerando header em 0.
     */
    int writeNode(const Node& node);

    /**
     * @brief Carrega o nó na posição lógica indicada.
     * @param position Posição lógica (1..N).
     * @return Nó carregado da mídia.
     */
    Node readNode(int position);

    /**
     * @brief Atualiza o header com m e root correntes.
     */
    void updateHeader();

    /**
     * @brief Lê e valida o header ao abrir o arquivo.
     * @return true se válido e compatível com limites [3..MAX_M].
     */
    bool loadAndValidateHeader();

    /**
     * @brief Mínimo de chaves para nós não-raiz.
     * @return ceil(m/2)-1.
     */
    int minKeys() const;

    /**
     * @brief Testa se o nó é folha (sem filhos).
     * @param node Nó a testar.
     * @return true se folha.
     */
    bool isLeaf(const Node& node) const;

    /**
     * @brief Corrige underflow do filho childIndex de parentPos (empréstimo ou fusão).
     * @param parentPos Posição do nó pai.
     * @param childIndex Índice do filho (0..n).
     * @details Primeiro tenta redistribuição a partir de irmãos abundantes; caso contrário, funde nós e ajusta o pai.
     */
    void fixUnderflow(int parentPos, int childIndex);

    /**
     * @brief Resultado interno da remoção recursiva.
     */
    enum class DelResult { Ok, Underflow, NotFound };

    /**
     * @brief Remove chave recursivamente a partir de nodePos.
     * @param nodePos Posição do nó atual.
     * @param key Chave a remover.
     * @return Ok se removido, Underflow se nó ficou abaixo do mínimo, NotFound se chave ausente.
     */
    DelResult deleteRecursive(int nodePos, int key);

public:
    /**
     * @brief Constrói árvore com ordem padrão m=3.
     */
    MWayTree();

    /**
     * @brief Constrói árvore com ordem informada (ajustada para [3..MAX_M]).
     * @param order Ordem desejada.
     */
    explicit MWayTree(int order);

    /**
     * @brief Destrutor: fecha arquivo e persiste header.
     */
    ~MWayTree();

    /**
     * @brief Abre o arquivo binário do índice e valida o header.
     * @param filename Caminho do .bin.
     * @return true se aberto e válido.
     */
    bool openBinary(const std::string& filename);

    /**
     * @brief Fecha o arquivo e grava header atualizado.
     */
    void closeBinary();

    /**
     * @brief Cria o índice a partir de um .txt (com validações e reachability).
     * @param textFilename Caminho do .txt de nós.
     * @param binFilename Caminho do .bin de saída.
     * @param order Ordem m desejada (ajustada para [3..MAX_M]).
     * @return true em caso de sucesso.
     */
    static bool createFromText(const std::string& textFilename, const std::string& binFilename, int order);

    /**
     * @brief Cria um índice vazio (apenas header) com raiz vazia.
     * @param binFilename Caminho do .bin de saída.
     * @param order Ordem m desejada (ajustada para [3..MAX_M]).
     * @return true em caso de sucesso.
     */
    static bool createEmpty(const std::string& binFilename, int order);

    /**
     * @brief Lê o header de um .bin sem mantê-lo aberto.
     * @param binFilename Caminho do .bin.
     * @param outM Saída: ordem m.
     * @param outRoot Saída: posição da raiz.
     * @return true se header válido.
     */
    static bool readHeader(const std::string& binFilename, int& outM, int& outRoot);

    /**
     * @brief Exporta o índice atual para .txt no mesmo layout de entrada.
     * @param textFilename Caminho do .txt de saída.
     * @return true em caso de sucesso.
     */
    bool exportToText(const std::string& textFilename) const;

    /**
     * @brief Exibe nós alcançáveis a partir da raiz (BFS textual).
     * @param binFilename Caminho do .bin (leitura independente).
     */
    void displayTree(const std::string& binFilename) const;

    /**
     * @brief Cria a raiz em árvore vazia persistindo o nó informado.
     * @param node Nó a usar como raiz.
     */
    void createRoot(const Node& node);

    /**
     * @brief Busca mSearch do topo até o alvo.
     * @param key Chave a buscar.
     * @param branch (Opcional) pilha com as posições dos nós visitados.
     * @return (nodePos, slot, found): se found=true, slot é 1-based do vetor keys;
     *         se found=false, slot é o índice do ponteiro de filho a seguir (ou posição de inserção).
     */
    std::tuple<int, int, bool> mSearch(int key, stack<int>* branch = nullptr);

    /**
     * @brief Inserção bottom-up com splits e possível criação de nova raiz.
     * @param key Chave a inserir (duplicatas são ignoradas).
     */
    void insertB(int key);

    /**
     * @brief Remoção com substituição por antecessor e correção de underflow; contrai a raiz se necessário.
     * @param key Chave a remover.
     * @return true se a chave existia e foi removida.
     */
    bool deleteB(int key);

    /**
     * @brief Zera contadores de I/O do índice.
     */
    void resetCounters();

    /**
     * @brief Retorna contadores de I/O do índice.
     * @return Par (leituras, escritas).
     */
    std::pair<long long,long long> getCounters() const;

    /**
     * @brief Verifica a integridade estrutural da árvore alcançável a partir da raiz.
     * @param verbose Se true, imprime mensagens de diagnóstico.
     * @return true se todos os invariantes forem satisfeitos.
     * @details Checa: header válido; alcance de todos os nós usados; chaves estritamente crescentes;
     *          faixas de valores por subárvore; filhos em intervalo válido; mínimo de chaves em nós não-raiz;
     *          consistência da raiz (vazia aponta 0, não-vazia aponta [1..N]).
     */
    bool verifyIntegrity(bool verbose = false) const;
};

#endif
