/**
* @file MWayTree.h
 * @authors Francisco Eduardo Fontenele - 15452569
 *          Vinicius Botte - 15522900
 *
 * AED II - Trabalho 1 - Parte 2
 * Declaração da classe MWayTree para árvore de busca m-vias
 */

#ifndef MWAYTREE_H
#define MWAYTREE_H

#include <fstream>
#include <string>
#include <tuple>

const int MAX_M = 32; // Max supported order for stable on-disk layout

struct Node {
    int n;                    // Number of keys
    int keys[MAX_M];          // Keys (fixed maximum size)
    int children[MAX_M+1];    // Child pointers (fixed maximum size)

    Node();
};

class MWayTree {
private:
    std::fstream file;
    std::string filename;
    int root;
    int m;

    void writeNode(const Node& node, int position);
    int writeNode(const Node& node);
    Node readNode(int position);

public:
    MWayTree();
    MWayTree(int order);
    ~MWayTree();

    bool openBinary(const std::string& filename);
    void closeBinary();

    static bool createFromText(const std::string& textFilename, const std::string& binFilename);

    void displayTree(const std::string& binFilename) const;

    int parent(int childNode, int key);

    void createRoot(const Node& node);

    // mSearch algorithm implementation
    std::tuple<int, int, bool> mSearch(int key);

    // insertB algorithm implementation
    void insertB(int key);
};

#endif
