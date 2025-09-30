#include "MWayTree.h"
#include "DataFile.h"
#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <limits>
#include <cstring>

using namespace std;

// List of test files for different trees
const vector<string> TEXT_FILES = {
    "mvias.txt",    // Tree 1 with standard numbering
    "mvias2.txt",   // Tree 2 with standard numbering
    "mvias3.txt",   // Tree 1 with alternative numbering
    "mvias4.txt",   // Tree 2 with alternative numbering
    "mvias5.txt"    // Tree 3 for m = 5
};

static void clearInputLine() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

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
                cout << "Registro: key=" << rec.key << " payload=\"" << rec.payload << "\"" << endl;
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

int main() {
    cout << "Selecione a arvore para teste:" << endl;
    cout << "1. Arvore 1 com numeracao padrao (mvias.txt)" << endl;
    cout << "2. Arvore 2 com numeracao padrao (mvias2.txt)" << endl;
    cout << "3. Arvore 1 com numeracao alternativa (mvias3.txt)" << endl;
    cout << "4. Arvore 2 com numeracao alternativa (mvias4.txt)" << endl;
    cout << "5. Arvore 3 para ordem 5 (mvias5.txt)" << endl;

    int choice = readIntInRange("Escolha (1-5): ", 1, 5);
    int order = readIntInRange(string("Informe a ordem m (3..") + to_string(MAX_M) + "): ", 3, MAX_M);

    string textFile = TEXT_FILES[choice - 1];
    string binFile = "mvias.bin";
    string dataFile = "data.bin";

    // Resolve absolute paths
    std::filesystem::path cwd = std::filesystem::current_path();
    std::filesystem::path textPath = cwd / textFile;
    std::filesystem::path binPath = cwd / binFile;
    std::filesystem::path dataPath = cwd / dataFile;

    cout << "Lendo dados de " << textFile << " e criando " << binFile << endl;
    cout << "Diretorio de trabalho: " << cwd.string() << endl;
    cout << "Arquivo de texto (absoluto): " << textPath.string() << endl;

    // Create binary index from text (with header m)
    if (!MWayTree::createFromText(textPath.string(), binPath.string(), order)) {
        cerr << "Falha ao criar indice binario a partir de " << textPath.string() << endl;
        return 1;
    }

    // Create data file from text (records)
    if (!DataFile::createFromText(textPath.string(), dataPath.string())) {
        cerr << "Falha ao criar arquivo de dados a partir de " << textPath.string() << endl;
        return 1;
    }

    MWayTree tree(order);
    if (!tree.openBinary(binPath.string())) {
        cerr << "Falha ao abrir indice binario " << binPath.string() << " (m=" << order << ")" << endl;
        return 1;
    }

    DataFile data;
    if (!data.open(dataPath.string())) {
        cerr << "Falha ao abrir arquivo de dados " << dataPath.string() << endl;
        tree.closeBinary();
        return 1;
    }

    cout << "Indice " << binFile << " aberto" << endl;

    while (true) {
        tree.displayTree(binPath.string());

        cout << "Selecione uma opcao:" << endl;
        cout << "1. Buscar chave" << endl;
        cout << "2. Inserir chave" << endl;
        cout << "3. Imprimir arquivo principal" << endl;
        cout << "4. Sair" << endl;

        int opt = readIntInRange("Escolha (1-4): ", 1, 4);

        switch (opt) {
            case 1: {
                runSearchInterface(tree, data);
                break;
            }
            case 2: {
                int key = readAnyInt("Chave para inserir: ");

                // Evitar duplicar no arquivo principal
                Record rec{};
                bool existsInData = data.find(key, rec);

                // Inserir no indice (insertB ja ignora duplicata)
                tree.insertB(key);
                auto [iR, iW] = tree.getCounters();
                cout << "I/O indice (insercao): R=" << iR << " W=" << iW << endl;

                if (!existsInData) {
                    Record newRec{};
                    newRec.key = key;
                    std::snprintf(newRec.payload, sizeof(newRec.payload), "value-%d", key);
                    data.insert(newRec);
                    auto [dR, dW] = data.getCounters();
                    cout << "I/O dados (insercao): R=" << dR << " W=" << dW << endl;
                } else {
                    auto [dR, dW] = data.getCounters();
                    cout << "Registro ja existe no arquivo principal. Nao inserido." << endl;
                    cout << "I/O dados (find): R=" << dR << " W=" << dW << endl;
                }

                char save = readYesNo("Exportar indice atualizado para o .txt selecionado (s/n)? ");
                if (save == 's') {
                    if (tree.exportToText(textPath.string())) {
                        cout << "Indice exportado para " << textPath.string() << endl;
                    } else {
                        cout << "Falha ao exportar indice para " << textPath.string() << endl;
                    }
                }

                // Mostrar arvore apos insercao
                tree.displayTree(binPath.string());
                break;
            }
            case 3: {
                data.printAll();
                break;
            }
            case 4: {
                data.close();
                tree.closeBinary();
                return 0;
            }
        }
    }
}
