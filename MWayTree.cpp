/**
 * @file MWayTree.cpp
 * @authors Francisco Eduardo Fontenele - 15452569
 *          Vinicius Botte - 15522900
 *
 * AED II - Trabalho 1 - Parte 1
 * Implementação da classe MWayTree para árvore de busca m-vias
 */

#include "MWayTree.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cstring>

using namespace std;

Node::Node() : n(0) {
    fill(begin(keys), end(keys), 0);
    fill(begin(children), end(children), 0);
}

MWayTree::MWayTree() : root(1), m(M) {}

MWayTree::~MWayTree() {
    closeBinary();
}

/**
 * @brief Writes a node to the binary file at the specified position
 * @pre The binary file must be open for writing
 * @post Node is written to the file at the specified position
 */
void MWayTree::writeNode(const Node& node, int position) {
    file.seekp(position * sizeof(Node));
    file.write(reinterpret_cast<const char*>(&node), sizeof(Node));
    file.flush();
}

/**
 * @brief Reads a node from the binary file at the specified position
 * @pre The binary file must be open for reading
 * @post Returns the node read from the file
 */
Node MWayTree::readNode(int position) {
    Node node;
    file.seekg(position * sizeof(Node));
    file.read(reinterpret_cast<char*>(&node), sizeof(Node));
    return node;
}

/**
 * @brief Opens the binary file for operations
 * @pre The binary file must exist
 * @post Returns true if the file was successfully opened
 */
bool MWayTree::openBinary(const string& filename) {
    file.open(filename, ios::in | ios::out | ios::binary);
    return file.is_open();
}

/**
 * @brief Closes the binary file
 * @pre None
 * @post The binary file is closed if it was open
 */
void MWayTree::closeBinary() {
    if (file.is_open()) {
        file.close();
    }
}

/**
 * @brief Creates a binary file from a text file
 * @pre The text file must exist and be properly formatted
 * @post Returns true if the binary file was successfully created
 */
bool MWayTree::createFromText(const string& textFilename, const string& binFilename) {
    ifstream textFile(textFilename);
    if (!textFile.is_open()) {
        cerr << "Error: Cannot open " << textFilename << endl;
        return false;
    }

    ofstream binFile(binFilename, ios::binary | ios::trunc);
    if (!binFile.is_open()) {
        cerr << "Error: Cannot create " << binFilename << endl;
        textFile.close();
        return false;
    }

    Node emptyNode;
    binFile.write(reinterpret_cast<const char*>(&emptyNode), sizeof(Node));

    string line;
    while (getline(textFile, line) && !line.empty()) {
        istringstream iss(line);
        Node node;

        iss >> node.n;
        iss >> node.children[0];

        for (int i = 0; i < node.n; i++) {
            iss >> node.keys[i] >> node.children[i + 1];
        }

        binFile.write(reinterpret_cast<const char*>(&node), sizeof(Node));
    }

    textFile.close();
    binFile.close();
    return true;
}

/**
 * @brief Displays the tree structure from the binary file
 * @pre The binary file must exist
 * @post The tree structure is printed to standard output
 */
void MWayTree::displayTree(const string& binFilename) const {
    cout << "T = " << root << ", m = " << m << endl;
    cout << "------------------------------------------------------------------" << endl;
    cout << "No n,A[0],(K[1],A[1]),...,(K[n],A[n])" << endl;
    cout << "------------------------------------------------------------------" << endl;

    ifstream binFile(binFilename, ios::binary);
    if (!binFile.is_open()) return;

    Node node;
    binFile.read(reinterpret_cast<char*>(&node), sizeof(Node)); // Skip position 0

    int nodeIndex = 1;
    while (binFile.read(reinterpret_cast<char*>(&node), sizeof(Node))) {
        cout << nodeIndex << " " << node.n << ", " << node.children[0];

        for (int i = 0; i < node.n; i++) {
            cout << ",(" << setw(3) << node.keys[i] << ", " << node.children[i + 1] << ")";
        }
        cout << endl;
        nodeIndex++;
    }

    binFile.close();
    cout << "------------------------------------------------------------------" << endl;
}

/**
 * @brief Searches for a key in the m-way tree
 * @pre The binary file must be open
 * @post Returns a tuple with (node, position, found) where:
 *        - node: the node where the key was found or should be inserted
 *        - position: the position in the node
 *        - found: true if the key was found, false otherwise
 */
tuple<int, int, bool> MWayTree::mSearch(int key) {
    if (!file.is_open()) {
        return make_tuple(0, 0, false);
    }

    int currentNode = root;

    while (currentNode != 0) {
        Node node = readNode(currentNode);

        int i = 0;
        while (i < node.n && key > node.keys[i]) {
            i++;
        }

        if (i < node.n && key == node.keys[i]) {
            return make_tuple(currentNode, i + 1, true); // 1-based indexing
        }

        if (node.children[i] == 0) {
            return make_tuple(currentNode, i, false); // Position for insertion
        }

        currentNode = node.children[i];
    }

    return make_tuple(0, 0, false);
}