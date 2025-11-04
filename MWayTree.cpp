/**
* @file MWayTree.cpp
 * @authors
 *   Francisco Eduardo Fontenele - 15452569
 *   Vinicius Botte - 15522900
 *
 * AED II - Trabalho 1
 */

#include "MWayTree.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <stack>
#include <queue>
#include <unordered_set>
#include <vector>
#include <cctype>
#include <limits>

using namespace std;

Node::Node() : n(0) {
    fill(begin(keys), end(keys), 0);
    fill(begin(children), end(children), 0);
}

MWayTree::MWayTree() : file(), filename(), root(0), m(3) {
}

MWayTree::MWayTree(int order) : file(), filename(), root(0) {
    if (order < 3) m = 3;
    else if (order > MAX_M) m = MAX_M;
    else m = order;
}

MWayTree::~MWayTree() {
    closeBinary();
}

void MWayTree::resetCounters() {
    idxReads = 0;
    idxWrites = 0;
}

pair<long long,long long> MWayTree::getCounters() const {
    return {idxReads, idxWrites};
}

/**
 * @brief Persiste o nó na posição lógica informada (>=1).
 * @param node Nó a gravar.
 * @param position Posição lógica (1..N).
 */
void MWayTree::writeNode(const Node& node, int position) {
    file.seekp(static_cast<std::streamoff>(position) * sizeof(Node), ios::beg);
    file.write(reinterpret_cast<const char*>(&node), sizeof(Node));
    file.flush();
    idxWrites++;
}

/**
 * @brief Persiste o nó ao final do arquivo (após o último nó).
 * @param node Nó a gravar.
 * @return Posição lógica (1..N) atribuída ao nó.
*/
int MWayTree::writeNode(const Node& node){
    file.seekp(0, ios::end);
    std::streampos endPos = file.tellp();
    int position = static_cast<int>(endPos / static_cast<std::streampos>(sizeof(Node)));
    file.write(reinterpret_cast<const char*>(&node), sizeof(Node));
    file.flush();
    idxWrites++;
    return position;
}

/**
 * @brief Carrega o nó na posição lógica indicada.
 * @param position Posição lógica (1..N).
 * @return Nó carregado da mídia.
 */
Node MWayTree::readNode(int position) {
    Node node{};
    file.seekg(static_cast<std::streamoff>(position) * sizeof(Node), ios::beg);
    file.read(reinterpret_cast<char*>(&node), sizeof(Node));
    idxReads++;
    return node;
}

/**
 * @brief Atualiza o header com m e root atuais.
 */
void MWayTree::updateHeader() {
    if (!file.is_open()) return;
    Node hdr{};
    hdr.n = -1;
    hdr.keys[0] = m;
    hdr.children[0] = root;
    file.seekp(0, ios::beg);
    file.write(reinterpret_cast<const char*>(&hdr), sizeof(Node));
    file.flush();
}

/**
 * @brief Lê e valida o header do arquivo aberto.
 * @return true se header válido (n=-1, 3<=m<=MAX_M).
 */
bool MWayTree::loadAndValidateHeader() {
    if (!file.is_open()) return false;
    Node hdr{};
    file.seekg(0, ios::beg);
    file.read(reinterpret_cast<char*>(&hdr), sizeof(Node));
    if (!file.good()) return false;
    if (hdr.n != -1) return false;
    int ord = hdr.keys[0];
    if (ord < 3 || ord > MAX_M) return false;
    m = ord;
    root = hdr.children[0];
    return true;
}

/**
 * @brief Abre o arquivo binário do índice e valida header.
 * @param filename_ Caminho do arquivo.
 * @return true se aberto e válido.
 */
bool MWayTree::openBinary(const string& filename_) {
    filename = filename_;
    file.open(filename, ios::in | ios::out | ios::binary);
    if (!file.is_open()) return false;
    if (!loadAndValidateHeader()) {
        file.close();
        return false;
    }
    return true;
}

/**
 * @brief Fecha o arquivo e persiste o header.
 */
void MWayTree::closeBinary() {
    if (file.is_open()) {
        updateHeader();
        file.close();
    }
}

/**
 * @brief Cria binário a partir de texto com validações (ordem, filhos, alcance).
 * @param textFilename Caminho do .txt de entrada.
 * @param binFilename Caminho do .bin de saída.
 * @param order Ordem m desejada (ajustada para [3..MAX_M]).
 * @return true se criado com sucesso.
 */
bool MWayTree::createFromText(const string& textFilename, const string& binFilename, int order) {
    int effOrder = (order < 3 ? 3 : (order > MAX_M ? MAX_M : order));

    ifstream textFile(textFilename);
    if (!textFile.is_open()) return false;

    struct Parsed {
        int n{};
        int a0{};
        int keys[MAX_M]{};
        int children[MAX_M+1]{};
    };
    vector<Parsed> nodes;
    nodes.reserve(64);

    string line;
    int lineNo = 0;
    while (getline(textFile, line)) {
        ++lineNo;
        bool onlySpace = true;
        for (char c : line) { if (!std::isspace(static_cast<unsigned char>(c))) { onlySpace = false; break; } }
        if (onlySpace) continue;

        istringstream iss(line);
        Parsed p{};
        if (!(iss >> p.n >> p.a0)) {
            cerr << "Erro ao ler n e A0 na linha " << lineNo << " do arquivo texto." << endl;
            return false;
        }
        if (p.n < 0 || p.n > effOrder - 1) {
            cerr << "Valor de n invalido (n=" << p.n << ") na linha " << lineNo << ". m=" << effOrder << endl;
            return false;
        }
        p.children[0] = p.a0;

        int prevK = std::numeric_limits<int>::min();
        for (int i = 0; i < p.n; ++i) {
            int k, ai;
            if (!(iss >> k >> ai)) {
                cerr << "Par Ki/Ai faltando na linha " << lineNo << " (esperado " << p.n << " pares)." << endl;
                return false;
            }
            if (i > 0 && k <= prevK) {
                cerr << "Chaves devem ser estritamente crescentes (linha " << lineNo << ")." << endl;
                return false;
            }
            prevK = k;
            p.keys[i] = k;
            p.children[i + 1] = ai;
        }
        int extraProbe;
        if (iss >> extraProbe) {
            cerr << "Tokens extras apos Kn/An na linha " << lineNo << "." << endl;
            return false;
        }

        nodes.push_back(p);
    }
    textFile.close();

    const int N = static_cast<int>(nodes.size());
    if (N == 0) {
        ofstream empty(binFilename, ios::binary | ios::trunc);
        if (!empty.is_open()) return false;
        Node hdr{};
        hdr.n = -1;
        hdr.keys[0] = effOrder;
        hdr.children[0] = 0;
        empty.write(reinterpret_cast<const char*>(&hdr), sizeof(Node));
        empty.close();
        return true;
    }

    auto childInRange = [&](int c)->bool { return c == 0 || (c >= 1 && c <= N); };

    for (int pos = 1; pos <= N; ++pos) {
        const auto& p = nodes[pos - 1];
        for (int i = 0; i <= p.n; ++i) {
            if (!childInRange(p.children[i])) {
                cerr << "Filho fora do intervalo (A" << i << "=" << p.children[i] << ") no no " << pos
                     << ". Valido: 0 ou [1.." << N << "]." << endl;
                return false;
            }
        }
    }

    vector<char> vis(N + 1, 0);
    queue<int> q;
    q.push(1); vis[1] = 1;
    while (!q.empty()) {
        int pos = q.front(); q.pop();
        const auto& p = nodes[pos - 1];
        for (int i = 0; i <= p.n; ++i) {
            int c = p.children[i];
            if (c != 0 && !vis[c]) { vis[c] = 1; q.push(c); }
        }
    }
    for (int pos = 1; pos <= N; ++pos) {
        if (!vis[pos]) {
            cerr << "No " << pos << " nao alcancavel a partir da raiz (1). Arquivo inconsistente." << endl;
            return false;
        }
    }

    ofstream binFile(binFilename, ios::binary | ios::trunc);
    if (!binFile.is_open()) return false;

    Node hdr{};
    hdr.n = -1;
    hdr.keys[0] = effOrder;
    hdr.children[0] = 1;
    binFile.write(reinterpret_cast<const char*>(&hdr), sizeof(Node));

    for (int pos = 1; pos <= N; ++pos) {
        Node node{};
        node.n = nodes[pos - 1].n;
        for (int i = 0; i < node.n; ++i) node.keys[i] = nodes[pos - 1].keys[i];
        for (int i = 0; i <= node.n; ++i) node.children[i] = nodes[pos - 1].children[i];
        binFile.write(reinterpret_cast<const char*>(&node), sizeof(Node));
    }

    binFile.close();
    return true;
}

/**
 * @brief Cria índice vazio com header e root=0.
 * @param binFilename Caminho do .bin de saída.
 * @param order Ordem m desejada (ajustada para [3..MAX_M]).
 * @return true em caso de sucesso.
 */
bool MWayTree::createEmpty(const std::string& binFilename, int order) {
    int ord = (order < 3 ? 3 : (order > MAX_M ? MAX_M : order));
    ofstream bin(binFilename, ios::binary | ios::trunc);
    if (!bin.is_open()) return false;
    Node hdr{};
    hdr.n = -1;
    hdr.keys[0] = ord;
    hdr.children[0] = 0;
    bin.write(reinterpret_cast<const char*>(&hdr), sizeof(Node));
    bin.flush();
    bin.close();
    return true;
}

/**
 * @brief Lê header sem manter arquivo aberto.
 * @param binFilename Caminho do .bin.
 * @param outM Saída: ordem m.
 * @param outRoot Saída: posição da raiz.
 * @return true se header válido.
 */
bool MWayTree::readHeader(const std::string& binFilename, int& outM, int& outRoot) {
    ifstream in(binFilename, ios::binary);
    if (!in.is_open()) return false;
    Node hdr{};
    in.read(reinterpret_cast<char*>(&hdr), sizeof(Node));
    in.close();
    if (hdr.n != -1) return false;
    outM = hdr.keys[0];
    outRoot = hdr.children[0];
    return true;
}

/**
 * @brief Exporta índice para .txt (exclui o header).
 * @param textFilename Caminho do .txt de saída.
 * @return true em caso de sucesso.
 */
bool MWayTree::exportToText(const std::string& textFilename) const {
    ifstream binFile(filename, ios::binary);
    if (!binFile.is_open()) return false;

    ofstream txt(textFilename, ios::trunc);
    if (!txt.is_open()) {
        binFile.close();
        return false;
    }

    Node node{};
    binFile.read(reinterpret_cast<char*>(&node), sizeof(Node));
    while (binFile.read(reinterpret_cast<char*>(&node), sizeof(Node))) {
        txt << node.n << " " << node.children[0];
        for (int i = 0; i < node.n; ++i) {
            txt << " " << node.keys[i] << " " << node.children[i + 1];
        }
        txt << "\n";
    }

    binFile.close();
    txt.close();
    return true;
}

/**
 * @brief Exibe nós alcançáveis a partir da raiz (BFS) em formato legível.
 * @param binFilename Caminho do .bin (leitura independente).
 */
void MWayTree::displayTree(const string& binFilename) const {
    cout << "T = " << root << ", m = " << m << endl;
    cout << "------------------------------------------------------------------" << endl;
    cout << "No n,A[0],(K[1],A[1]),...,(K[n],A[n])" << endl;
    cout << "------------------------------------------------------------------" << endl;

    if (root == 0) {
        cout << "(arvore vazia)" << endl;
        cout << "------------------------------------------------------------------" << endl;
        return;
    }

    ifstream bin(binFilename, ios::binary);
    if (!bin.is_open()) return;

    auto readAt = [&](int pos, Node& node) -> bool {
        bin.seekg(static_cast<std::streamoff>(pos) * sizeof(Node), ios::beg);
        bin.read(reinterpret_cast<char*>(&node), sizeof(Node));
        return bin.good();
    };

    queue<int> q;
    unordered_set<int> vis;
    q.push(root);
    vis.insert(root);

    while (!q.empty()) {
        int pos = q.front(); q.pop();
        Node node{};
        if (!readAt(pos, node)) continue;

        cout << setw(2) << pos << " " << node.n << ", " << setw(2) << node.children[0];
        for (int i = 0; i < node.n; i++) {
            cout << ",(" << setw(2) << node.keys[i] << ", " << setw(2) << node.children[i + 1] << ")";
        }
        cout << endl;

        for (int i = 0; i <= node.n; ++i) {
            int c = node.children[i];
            if (c != 0 && !vis.count(c)) {
                vis.insert(c);
                q.push(c);
            }
        }
    }

    bin.close();
    cout << "------------------------------------------------------------------" << endl;
}

/**
 * @brief Cria a raiz em árvore vazia e persiste no arquivo.
 * @param node Nó a persistir como raiz.
 */
void MWayTree::createRoot(const Node& node){
    int pos = writeNode(node);
    root = pos;
    updateHeader();
}

/**
 * @brief Busca mSearch: desce a árvore comparando chaves até encontrar ou determinar posição.
 * @param key Chave a buscar.
 * @param branch (Opcional) pilha com as posições dos nós visitados (topo = último visitado).
 * @return (nodePos, slot, found): se found=true, slot é 1-based do vetor keys;
 *         se found=false, slot é o índice do ponteiro de filho a seguir (e posição de inserção).
 */
tuple<int, int, bool> MWayTree::mSearch(int key, stack<int>* branch) {
    if (!file.is_open() || root == 0) return make_tuple(0, 0, false);

    resetCounters();

    int current = root;
    if (branch) branch->push(current);

    while (current != 0) {
        Node node = readNode(current);

        int i = 0;
        while (i < node.n && key > node.keys[i]) i++;

        if (i < node.n && key == node.keys[i]) {
            return make_tuple(current, i + 1, true);
        }

        if (node.children[i] == 0) {
            return make_tuple(current, i, false);
        }

        current = node.children[i];
        if (branch) branch->push(current);
    }

    return make_tuple(0, 0, false);
}

/**
 * @brief Inserção bottom-up: insere em folha; se nó ficar cheio (n>=m), divide e promove chave central.
 * @param key Chave a inserir; duplicatas são ignoradas.
 */
void MWayTree::insertB(int key){
    if (!file.is_open()) return;

    resetCounters();

    if (root == 0) {
        Node r{};
        r.n = 1;
        r.keys[0] = key;
        createRoot(r);
        return;
    }

    vector<int> path;
    int cur = root;
    while (true) {
        path.push_back(cur);
        Node node = readNode(cur);

        int i = 0;
        while (i < node.n && key > node.keys[i]) i++;

        if (i < node.n && key == node.keys[i]) return;

        if (node.children[i] == 0) {
            for (int j = node.n; j > i; --j) node.keys[j] = node.keys[j - 1];
            for (int j = node.n + 1; j > i; --j) node.children[j] = node.children[j - 1];
            node.keys[i] = key;
            node.children[i] = 0;
            node.n++;
            writeNode(node, cur);

            int upKey = 0;
            int rightPos = 0;
            while (node.n >= m) {
                int mid = m / 2;
                Node left = node;
                Node right{};
                int rightCount = node.n - mid - 1;

                left.n = mid;
                for (int k = 0; k < rightCount; ++k) right.keys[k] = node.keys[mid + 1 + k];
                for (int k = 0; k <= rightCount; ++k) right.children[k] = node.children[mid + 1 + k];
                right.n = rightCount;
                upKey = node.keys[mid];

                writeNode(left, cur);
                rightPos = writeNode(right);

                if (cur == root) {
                    Node newRoot{};
                    newRoot.n = 1;
                    newRoot.keys[0] = upKey;
                    newRoot.children[0] = cur;
                    newRoot.children[1] = rightPos;
                    int newRootPos = writeNode(newRoot);
                    root = newRootPos;
                    updateHeader();
                    return;
                } else {
                    int parentPos = path[path.size() - 2];
                    Node parent = readNode(parentPos);

                    int pi = 0;
                    while (pi <= parent.n && parent.children[pi] != cur) pi++;

                    for (int j2 = parent.n; j2 > pi; --j2) parent.keys[j2] = parent.keys[j2 - 1];
                    for (int j2 = parent.n + 1; j2 > pi + 1; --j2) parent.children[j2] = parent.children[j2 - 1];

                    parent.keys[pi] = upKey;
                    parent.children[pi] = cur;
                    parent.children[pi + 1] = rightPos;
                    parent.n++;
                    writeNode(parent, parentPos);

                    cur = parentPos;
                    node = parent;
                    path.pop_back();
                    continue;
                }
            }

            return;
        } else {
            cur = node.children[i];
        }
    }
}

int MWayTree::minKeys() const {
    int t = (m + 1) / 2;
    return t - 1;
}

bool MWayTree::isLeaf(const Node& node) const {
    return node.children[0] == 0;
}

/**
 * @brief Corrige underflow do filho childIndex de parentPos.
 * @param parentPos Posição do pai.
 * @param childIndex Índice do filho [0..n].
 * @details Estratégia: (1) tentar empréstimo do irmão esquerdo/direito com >minKeys;
 *          (2) caso contrário, fundir com irmão adjacente e puxar chave do pai.
 */
void MWayTree::fixUnderflow(int parentPos, int childIndex) {
    Node parent = readNode(parentPos);
    int minK = minKeys();

    int childPos = parent.children[childIndex];
    Node child = readNode(childPos);

    int leftIdx = childIndex - 1;
    int rightIdx = childIndex + 1;

    if (leftIdx >= 0) {
        int leftPos = parent.children[leftIdx];
        Node left = readNode(leftPos);
        if (left.n > minK) {
            for (int j = child.n; j > 0; --j) {
                child.keys[j] = child.keys[j - 1];
                child.children[j + 1] = child.children[j];
            }
            child.children[1] = child.children[0];
            child.keys[0] = parent.keys[leftIdx];
            child.children[0] = left.children[left.n];
            child.n++;

            parent.keys[leftIdx] = left.keys[left.n - 1];
            left.n--;

            writeNode(left, leftPos);
            writeNode(child, childPos);
            writeNode(parent, parentPos);
            return;
        }
    }

    if (rightIdx <= parent.n) {
        int rightPos = parent.children[rightIdx];
        Node right = readNode(rightPos);
        if (right.n > minK) {
            child.keys[child.n] = parent.keys[childIndex];
            child.children[child.n + 1] = right.children[0];
            child.n++;

            parent.keys[childIndex] = right.keys[0];
            for (int j = 0; j < right.n - 1; ++j) {
                right.keys[j] = right.keys[j + 1];
                right.children[j] = right.children[j + 1];
            }
            right.children[right.n - 1] = right.children[right.n];
            right.n--;

            writeNode(right, rightPos);
            writeNode(child, childPos);
            writeNode(parent, parentPos);
            return;
        }
    }

    if (leftIdx >= 0) {
        int leftPos = parent.children[leftIdx];
        Node left = readNode(leftPos);

        left.keys[left.n] = parent.keys[leftIdx];
        left.children[left.n + 1] = child.children[0];
        for (int j = 0; j < child.n; ++j) {
            left.keys[left.n + 1 + j] = child.keys[j];
            left.children[left.n + 2 + j] = child.children[j + 1];
        }
        left.n += 1 + child.n;

        for (int j = leftIdx; j < parent.n - 1; ++j) {
            parent.keys[j] = parent.keys[j + 1];
            parent.children[j + 1] = parent.children[j + 2];
        }
        parent.n--;

        writeNode(left, leftPos);
        writeNode(parent, parentPos);
    } else {
        int rightPos = parent.children[rightIdx];
        Node right = readNode(rightPos);

        child.keys[child.n] = parent.keys[childIndex];
        child.children[child.n + 1] = right.children[0];
        for (int j = 0; j < right.n; ++j) {
            child.keys[child.n + 1 + j] = right.keys[j];
            child.children[child.n + 2 + j] = right.children[j + 1];
        }
        child.n += 1 + right.n;

        for (int j = childIndex; j < parent.n - 1; ++j) {
            parent.keys[j] = parent.keys[j + 1];
            parent.children[j + 1] = parent.children[j + 2];
        }
        parent.n--;

        writeNode(child, childPos);
        writeNode(parent, parentPos);
    }
}

/**
 * @brief Remoção recursiva com substituto por antecessor em nós internos.
 * @param nodePos Posição atual.
 * @param key Chave a remover.
 * @return Ok/Underflow/NotFound conforme progresso; underflow propagará ajuste ao retorno.
 */
MWayTree::DelResult MWayTree::deleteRecursive(int nodePos, int key) {
    Node node = readNode(nodePos);
    int minK = minKeys();

    int i = 0;
    while (i < node.n && key > node.keys[i]) i++;

    if (i < node.n && node.keys[i] == key) {
        if (isLeaf(node)) {
            for (int j = i; j < node.n - 1; ++j) node.keys[j] = node.keys[j + 1];
            node.n--;
            writeNode(node, nodePos);
            if (nodePos != root && node.n < minK) return DelResult::Underflow;
            return DelResult::Ok;
        } else {
            int predPos = node.children[i];
            Node cur = readNode(predPos);
            while (!isLeaf(cur)) {
                predPos = cur.children[cur.n];
                cur = readNode(predPos);
            }
            int predKey = cur.keys[cur.n - 1];
            node.keys[i] = predKey;
            writeNode(node, nodePos);
            auto res = deleteRecursive(node.children[i], predKey);
            if (res == DelResult::Underflow) {
                fixUnderflow(nodePos, i);
                Node p = readNode(nodePos);
                if (nodePos != root && p.n < minK) return DelResult::Underflow;
            }
            return DelResult::Ok;
        }
    }

    if (isLeaf(node)) {
        return DelResult::NotFound;
    } else {
        int childIndex = i;
        auto res = deleteRecursive(node.children[childIndex], key);
        if (res == DelResult::Underflow) {
            fixUnderflow(nodePos, childIndex);
            Node p = readNode(nodePos);
            if (nodePos != root && p.n < minK) return DelResult::Underflow;
            return DelResult::Ok;
        }
        return res;
    }
}

/**
 * @brief Remoção e contração de raiz: após remover, se root ficar com n=0, adota único filho (ou zera).
 * @param key Chave a remover.
 * @return true se a chave existia no índice.
 */
bool MWayTree::deleteB(int key) {
    if (!file.is_open() || root == 0) return false;
    resetCounters();

    auto res = deleteRecursive(root, key);
    if (res == DelResult::NotFound) return false;

    Node r = readNode(root);
    if (r.n == 0) {
        if (r.children[0] != 0) {
            root = r.children[0];
        } else {
            root = 0;
        }
        updateHeader();
    }
    return true;
}

/**
 * @brief Verifica invariantes estruturais em todos os nós alcançáveis a partir da raiz.
 * @param verbose Se true, imprime diagnósticos detalhados no stdout.
 * @return true se íntegro.
 * @details Checa: header (n=-1, m válido); consistência de raiz; alcance via BFS;
 *          ordenação estrita das chaves; limites de faixa por subárvore; ponteiros de filhos no intervalo [0..N];
 *          mínimos por nó não-raiz (interno: >=minKeys, folha: >=1); ausência de nós órfãos.
 */
bool MWayTree::verifyIntegrity(bool verbose) const {
    ifstream in(filename, ios::binary);
    if (!in.is_open()) {
        if (verbose) cout << "Falha ao abrir arquivo do indice para verificacao." << endl;
        return false;
    }
    in.seekg(0, ios::end);
    auto sz = in.tellg();
    if (sz < static_cast<std::streamoff>(sizeof(Node))) {
        if (verbose) cout << "Arquivo muito pequeno para conter header." << endl;
        return false;
    }
    int totalNodes = static_cast<int>(sz / static_cast<std::streamoff>(sizeof(Node))) - 1;

    Node hdr{};
    in.seekg(0, ios::beg);
    in.read(reinterpret_cast<char*>(&hdr), sizeof(Node));
    if (hdr.n != -1) {
        if (verbose) cout << "Header invalido (n != -1)." << endl;
        return false;
    }
    if (hdr.keys[0] != m) {
        if (verbose) cout << "Ordem m do header (" << hdr.keys[0] << ") difere da carregada (" << m << ")." << endl;
        return false;
    }
    int rt = hdr.children[0];
    if (rt == 0) {
        if (totalNodes != 0) {
            if (verbose) cout << "Raiz vazia, mas existem nos gravados (" << totalNodes << ")." << endl;
            return false;
        }
        return true;
    }

    auto readAt = [&](int pos, Node& node)->bool {
        in.seekg(static_cast<std::streamoff>(pos) * sizeof(Node), ios::beg);
        in.read(reinterpret_cast<char*>(&node), sizeof(Node));
        return in.good();
    };
    auto childInRange = [&](int c)->bool { return c == 0 || (c >= 1 && c <= totalNodes); };

    struct Item { int pos; int low; int high; };
    queue<Item> q;
    vector<char> vis(totalNodes + 1, 0);
    q.push({rt, std::numeric_limits<int>::min(), std::numeric_limits<int>::max()});
    vis[rt] = 1;

    int minK = minKeys();
    while (!q.empty()) {
        auto it = q.front(); q.pop();
        Node node{};
        if (!readAt(it.pos, node)) {
            if (verbose) cout << "Falha ao ler no " << it.pos << "." << endl;
            return false;
        }
        if (node.n < 0 || node.n > m - 1) {
            if (verbose) cout << "No " << it.pos << " com n fora de [0," << (m - 1) << "]." << endl;
            return false;
        }
        for (int i = 0; i < node.n; ++i) {
            if (i > 0 && node.keys[i] <= node.keys[i - 1]) {
                if (verbose) cout << "Chaves nao estritamente crescentes no no " << it.pos << "." << endl;
                return false;
            }
            if (!(node.keys[i] > it.low && node.keys[i] < it.high)) {
                if (verbose) cout << "Chave " << node.keys[i] << " do no " << it.pos
                                  << " fora da faixa (" << it.low << "," << it.high << ")." << endl;
                return false;
            }
        }
        for (int i = 0; i <= node.n; ++i) {
            int c = node.children[i];
            if (!childInRange(c)) {
                if (verbose) cout << "Filho fora do intervalo (A" << i << "=" << c << ") no no " << it.pos << "." << endl;
                return false;
            }
            if (c != 0) {
                int childLow  = (i == 0) ? it.low : node.keys[i - 1];
                int childHigh = (i == node.n) ? it.high : node.keys[i];
                if (!vis[c]) { vis[c] = 1; q.push({c, childLow, childHigh}); }
            }
        }
        if (it.pos != rt) {
            bool leaf = (node.children[0] == 0);
            if (!leaf && node.n < minK) {
                if (verbose) cout << "No interno " << it.pos << " com n < minKeys (" << minK << ")." << endl;
                return false;
            }
            if (leaf && node.n < 1) {
                if (verbose) cout << "Folha nao-raiz " << it.pos << " com n=0." << endl;
                return false;
            }
        }
    }
    for (int pos = 1; pos <= totalNodes; ++pos) {
        if (!vis[pos]) {
            if (verbose) cout << "No " << pos << " nao alcancavel a partir da raiz." << endl;
            return false;
        }
    }
    return true;
}
