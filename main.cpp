/**
* @file main.cpp
 * @authors
 *   Francisco Eduardo Fontenele - 15452569
 *   Vinicius Botte - 15522900
 *
 * AED II - Trabalho 1
 */

#include "MWayTree.h"
#include "DataFile.h"
#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <limits>
#include <cstdio>

using namespace std;

const vector<string> TEXT_FILES = {
    "mvias.txt",
    "mvias2.txt",
    "mvias3.txt",
    "mvias4.txt",
    "mvias5.txt"
};

/**
 * @brief Limpa estado de erro e descarta até o fim da linha no stdin.
 */
static void clearInputLine() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

/**
 * @brief Lê um inteiro no intervalo [minV..maxV] com re-prompt até validar.
 * @param prompt Mensagem exibida.
 * @param minV Valor mínimo aceito.
 * @param maxV Valor máximo aceito.
 * @return Inteiro validado.
 */
static int readIntInRange(const string& prompt, int minV, int maxV) {
    while (true) {
        cout << prompt;
        int v;
        if (cin >> v) {
            if (v >= minV && v <= maxV) {
                clearInputLine();
                return v;
            }
            cout << "Valor invalido. Informe um numero entre " << minV << " e " << maxV << "." << endl;
        } else {
            cout << "Entrada invalida. Tente novamente." << endl;
            clearInputLine();
        }
    }
}

/**
 * @brief Lê qualquer inteiro com re-prompt até validar.
 * @param prompt Mensagem exibida.
 * @return Inteiro lido.
 */
static int readAnyInt(const string& prompt) {
    while (true) {
        cout << prompt;
        int v;
        if (cin >> v) {
            clearInputLine();
            return v;
        } else {
            cout << "Entrada invalida. Tente novamente." << endl;
            clearInputLine();
        }
    }
}

/**
 * @brief Lê resposta 's' ou 'n' (case-insensitive) com re-prompt.
 * @param prompt Mensagem exibida.
 * @return 's' ou 'n'.
 */
static char readYesNo(const string& prompt) {
    while (true) {
        cout << prompt;
        char c;
        if (cin >> c) {
            clearInputLine();
            if (c == 's' || c == 'S') return 's';
            if (c == 'n' || c == 'N') return 'n';
            cout << "Responda com 's' ou 'n'." << endl;
        } else {
            cout << "Entrada invalida. Tente novamente." << endl;
            clearInputLine();
        }
    }
}

/**
 * @brief Loop de busca por chave mostrando contadores de I/O de índice e dados.
 * @param tree Árvore índice aberta.
 * @param data Arquivo de dados aberto.
 */
void runSearchInterface(MWayTree& tree, DataFile& data) {
    while (true) {
        int key = readAnyInt("Chave de busca: ");
        auto [node, pos, found] = tree.mSearch(key);
        auto [idxR, idxW] = tree.getCounters();
        cout << " " << key << " (" << node << "," << pos << "," << (found ? "true" : "false") << ")" << endl;
        cout << "I/O indice: R=" << idxR << " W=" << idxW << endl;

        if (found) {
            Record rec{};
            if (data.find(key, rec)) {
                auto [dR, dW] = data.getCounters();
                cout << "Registro: key=" << rec.key << " payload=\"" << rec.payload << "\" active=" << rec.active << endl;
                cout << "I/O dados: R=" << dR << " W=" << dW << endl;
            } else {
                auto [dR, dW] = data.getCounters();
                cout << "Registro nao encontrado no arquivo principal." << endl;
                cout << "I/O dados: R=" << dR << " W=" << dW << endl;
            }
        }

        char c = readYesNo("Continuar busca (s/n)? ");
        if (c == 'n') break;
    }
}

/**
 * @brief Ponto de entrada: menu para criar/abrir índice e operar (buscar, inserir, remover, verificar).
 * @return Código de retorno do processo (0 em sucesso).
 */
int main() {
    cout << "Selecione uma opcao de inicializacao:" << endl;
    cout << "1. Abrir indice existente (mvias.bin)" << endl;
    cout << "2. Criar indice a partir de um .txt" << endl;
    cout << "3. Criar indice vazio" << endl;
    cout << "4. Criar indice a partir de employees.txt (arquivo principal)" << endl;

    int init = readIntInRange("Escolha (1-4): ", 1, 4);

    string binFile = "mvias.bin";
    string dataFile = "data.bin";

    std::filesystem::path cwd = std::filesystem::current_path();
    std::filesystem::path binPath = cwd / binFile;
    std::filesystem::path dataPath = cwd / dataFile;

    int order = 3;
    string textFile;

    if (init == 1) {
        if (!std::filesystem::exists(binPath)) {
            cout << "Indice inexistente. Crie primeiro pelo menu." << endl;
            return 1;
        }
        int mHdr = 0, rHdr = 0;
        if (!MWayTree::readHeader(binPath.string(), mHdr, rHdr)) {
            cout << "Header invalido em mvias.bin." << endl;
            return 1;
        }
        order = mHdr;
        if (!std::filesystem::exists(dataPath)) {
            ofstream d(dataPath, ios::binary | ios::trunc);
            d.close();
        }
    } else if (init == 2) {
        cout << "Selecione a arvore para teste:" << endl;
        cout << "1. Arvore 1 (mvias.txt)" << endl;
        cout << "2. Arvore 2 (mvias2.txt)" << endl;
        cout << "3. Arvore 1 numeracao alternativa (mvias3.txt)" << endl;
        cout << "4. Arvore 2 numeracao alternativa (mvias4.txt)" << endl;
        cout << "5. Arvore 3 m = 5 (mvias5.txt)" << endl;
        int choice = readIntInRange("Escolha (1-5): ", 1, 5);
        order = readIntInRange(string("Informe a ordem m (3..") + to_string(MAX_M) + "): ", 3, MAX_M);
        textFile = TEXT_FILES[choice - 1];
        std::filesystem::path textPath = cwd / textFile;

        if (!MWayTree::createFromText(textPath.string(), binPath.string(), order)) {
            cout << "Falha ao criar indice a partir de " << textPath.string() << endl;
            return 1;
        }
        if (!DataFile::createFromText(textPath.string(), dataPath.string())) {
            cout << "Falha ao criar arquivo de dados a partir de " << textPath.string() << endl;
            return 1;
        }
    } else if (init == 3) {
        order = readIntInRange(string("Informe a ordem m (3..") + to_string(MAX_M) + "): ", 3, MAX_M);
        if (!MWayTree::createEmpty(binPath.string(), order)) {
            cout << "Falha ao criar indice vazio." << endl;
            return 1;
        }
        if (!std::filesystem::exists(dataPath)) {
            ofstream d(dataPath, ios::binary | ios::trunc);
            d.close();
        }
    } else {
        order = readIntInRange(string("Informe a ordem m (3..") + to_string(MAX_M) + "): ", 3, MAX_M);
        std::filesystem::path empPath = cwd / "employees.txt";
        if (!std::filesystem::exists(empPath)) {
            cout << "Arquivo employees.txt nao encontrado em " << empPath.string() << endl;
            return 1;
        }
        if (!DataFile::createFromEmployees(empPath.string(), dataPath.string())) {
            cout << "Falha ao criar arquivo de dados a partir de employees.txt" << endl;
            return 1;
        }
        if (!MWayTree::createEmpty(binPath.string(), order)) {
            cout << "Falha ao criar indice vazio." << endl;
            return 1;
        }
    }

    MWayTree tree(order);
    if (!tree.openBinary(binPath.string())) {
        cout << "Falha ao abrir indice " << binPath.string() << endl;
        return 1;
    }

    DataFile data;
    if (!data.open(dataPath.string())) {
        cout << "Falha ao abrir arquivo de dados " << dataPath.string() << endl;
        tree.closeBinary();
        return 1;
    }

    if (init == 4) {
        vector<int> keys;
        if (!data.listActiveKeys(keys)) {
            cout << "Falha ao ler chaves de employees em " << dataPath.string() << endl;
            data.close();
            tree.closeBinary();
            return 1;
        }
        for (int k : keys) {
            tree.insertB(k);
        }
        cout << "Indice criado a partir de employees.txt com " << keys.size() << " registros." << endl;
    }

    while (true) {
        tree.displayTree(binPath.string());

        cout << "Selecione uma opcao:" << endl;
        cout << "1. Buscar chave" << endl;
        cout << "2. Inserir chave" << endl;
        cout << "3. Imprimir arquivo principal" << endl;
        cout << "4. Remover chave" << endl;
        cout << "5. Verificar integridade" << endl;
        cout << "6. Sair" << endl;

        int opt = readIntInRange("Escolha (1-6): ", 1, 6);

        switch (opt) {
            case 1: {
                runSearchInterface(tree, data);
                break;
            }
            case 2: {
                int key = readAnyInt("Chave para inserir: ");

                Record rec{};
                bool existsInData = data.find(key, rec);

                tree.insertB(key);
                auto [iR, iW] = tree.getCounters();
                cout << "I/O indice (insercao): R=" << iR << " W=" << iW << endl;

                if (!existsInData) {
                    Record newRec{};
                    newRec.key = key;
                    char dept = "ABCDE"[key % 5];
                    std::snprintf(newRec.payload, sizeof(newRec.payload), "Funcionario %d | depto=%c", key, dept);
                    data.insert(newRec);
                    auto [dR, dW] = data.getCounters();
                    cout << "I/O dados (insercao): R=" << dR << " W=" << dW << endl;
                } else {
                    auto [dR, dW] = data.getCounters();
                    cout << "Registro ja existe no arquivo principal. Nao inserido." << endl;
                    cout << "I/O dados (find): R=" << dR << " W=" << dW << endl;
                }
                break;
            }
            case 3: {
                data.printAll();
                break;
            }
            case 4: {
                int key = readAnyInt("Chave para remover: ");

                bool removedIdx = tree.deleteB(key);
                auto [iR, iW] = tree.getCounters();
                cout << "I/O indice (remocao): R=" << iR << " W=" << iW << endl;

                if (removedIdx) {
                    bool removedData = data.remove(key);
                    auto [dR, dW] = data.getCounters();
                    cout << "Remocao no arquivo principal: " << (removedData ? "ok" : "nao encontrado") << endl;
                    cout << "I/O dados (remocao): R=" << dR << " W=" << dW << endl;
                } else {
                    cout << "Chave nao encontrada no indice." << endl;
                }
                break;
            }
            case 5: {
                bool ok = tree.verifyIntegrity(true);
                cout << "Integridade: " << (ok ? "ok" : "falha") << endl;
                break;
            }
            case 6: {
                data.close();
                tree.closeBinary();
                return 0;
            }
        }
    }
}
