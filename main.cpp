/*
 * Authors: Francisco Eduardo Fontenele - 15452569
 *          [Author 2 Name] - [USP Number]
 * 
 * AED II - Trabalho 1 - Parte 1
 * M-way Search Tree Implementation
 */

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>

using namespace std;

const int M = 3; // Order of m-way tree
const string TEXT_FILE = "mvias.txt";
const string BINARY_FILE = "mvias.bin";

struct Node {
    int n;                    // Number of keys
    int keys[M - 1];          // Keys (fixed size: M-1)
    int children[M];          // Child pointers (fixed size: M)

    Node() : n(0) {
        fill(begin(keys), end(keys), 0);
        fill(begin(children), end(children), 0);
    }
};

class MWayTree {
private:
    fstream file;
    int root;
    int m;
    
    void writeNode(const Node& node, int position) {
        file.seekp(position * sizeof(Node));
        file.write(reinterpret_cast<const char*>(&node), sizeof(Node));
        file.flush();
    }

    Node readNode(int position) {
        Node node;
        file.seekg(position * sizeof(Node));
        file.read(reinterpret_cast<char*>(&node), sizeof(Node));
        return node;
    }
    
public:
    MWayTree() : root(1), m(M) {}
    
    bool openBinary() {
        file.open(BINARY_FILE, ios::in | ios::out | ios::binary);
        return file.is_open();
    }
    
    void closeBinary() {
        if (file.is_open()) {
            file.close();
        }
    }
    
    static bool createFromText() {
        ifstream textFile(TEXT_FILE);
        if (!textFile.is_open()) {
            cerr << "Error: Cannot open " << TEXT_FILE << endl;
            return false;
        }

        ofstream binFile(BINARY_FILE, ios::binary | ios::trunc);
        if (!binFile.is_open()) {
            cerr << "Error: Cannot create " << BINARY_FILE << endl;
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
    
    void displayTree() const {
        cout << "T = " << root << ", m = " << m << endl;
        cout << "------------------------------------------------------------------" << endl;
        cout << "No n,A[0],(K[1],A[1]),...,(K[n],A[n])" << endl;
        cout << "------------------------------------------------------------------" << endl;

        ifstream binFile(BINARY_FILE, ios::binary);
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
    
    // mSearch algorithm implementation
    tuple<int, int, bool> mSearch(int key) {
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
};

void runSearchInterface(MWayTree& tree) {
    int key;
    char continuar;
    
    do {
        tree.displayTree();
        cout << "Chave de busca: ";
        cin >> key;
        
        auto [node, pos, found] = tree.mSearch(key);
        cout << key << " (" << node << "," << pos << "," << (found ? "true" : "false") << ")" << endl;
        
        cout << "Continuar busca (s/n)? ";
        cin >> continuar;
        
    } while (continuar == 's' || continuar == 'S');
}

int main() {
    cout << "Lendo dados de " << TEXT_FILE << " e criando " << BINARY_FILE << endl;
    
    MWayTree tree;
    
    // Create binary file from text file
    if (!MWayTree::createFromText()) {
        cerr << "Error creating binary file from text file" << endl;
        return 1;
    }
    
    // Open binary file for operations
    if (!tree.openBinary()) {
        cerr << "Error: Cannot open " << BINARY_FILE << endl;
        return 1;
    }
    
    cout << "Indice " << BINARY_FILE << " aberto" << endl;
    
    // Run search interface
    runSearchInterface(tree);
    
    tree.closeBinary();
    return 0;
}