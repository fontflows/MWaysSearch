/**
 * @file main.cpp
 * @authors Francisco Eduardo Fontenele - 15452569
 *          Vinicius Botte - 15522900
 *
 * AED II - Trabalho 1 - Parte 1
 * Programa principal para teste de árvores de busca m-vias
 */

#include "MWayTree.h"
#include <iostream>
#include <vector>
#include <string>

using namespace std;

// List of test files for different trees
const vector<string> TEXT_FILES = {
    "mvias.txt",    // Tree 1 with standard numbering
    "mvias2.txt",   // Tree 2 with standard numbering
    "mvias3.txt",   // Tree 1 with alternative numbering
    "mvias4.txt"    // Tree 2 with alternative numbering
};

/**
 * @brief Runs the search interface for a given tree
 * @pre The binary file must be created and open
 * @post The search interface is executed until the user chooses to exit
 */
void runSearchInterface(MWayTree& tree, const string& binFilename) {
    int key;
    char continuar;

    do {
        tree.displayTree(binFilename);
        cout << "Chave de busca: ";
        cin >> key;

        auto [node, pos, found] = tree.mSearch(key);
        cout << key << " (" << node << "," << pos << "," << (found ? "true" : "false") << ")" << endl;

        cout << "Continuar busca (s/n)? ";
        cin >> continuar;

    } while (continuar == 's' || continuar == 'S');
}

/**
 * @brief Main function
 * @pre None
 * @post Returns 0 on success, 1 on error
 */
int main() {
    int choice;
    cout << "Selecione a arvore para teste:" << endl;
    cout << "1. Arvore 1 com numeracao padrao (mvias.txt)" << endl;
    cout << "2. Arvore 2 com numeracao padrao (mvias2.txt)" << endl;
    cout << "3. Arvore 1 com numeracao alternativa (mvias3.txt)" << endl;
    cout << "4. Arvore 2 com numeracao alternativa (mvias4.txt)" << endl;
    cout << "Escolha (1-4): ";
    cin >> choice;

    if (choice < 1 || choice > 4) {
        cerr << "Escolha invalida!" << endl;
        return 1;
    }

    string textFile = TEXT_FILES[choice - 1];
    string binFile = "mvias.bin"; // Binary file name

    cout << "Lendo dados de " << textFile << " e criando " << binFile << endl;

    // Create binary file from text file
    if (!MWayTree::createFromText(textFile, binFile)) {
        cerr << "Error creating binary file from text file" << endl;
        return 1;
    }

    MWayTree tree;

    // Open binary file for operations
    if (!tree.openBinary(binFile)) {
        cerr << "Error: Cannot open " << binFile << endl;
        return 1;
    }

    cout << "Indice " << binFile << " aberto" << endl;

    // Run search interface
    runSearchInterface(tree, binFile);
    
    tree.closeBinary();
    return 0;
}