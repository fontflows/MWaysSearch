/**
* @file MWayTree.h
 * @authors
 *   Francisco Eduardo Fontenele - 15452569
 *   Vinicius Botte - 15522900
 *
 * AED II - Trabalho 1 - Parte 2
 * Declaração da classe MWayTree para árvore B em disco
 */

#ifndef MWAYTREE_H
#define MWAYTREE_H

#include <fstream>
#include <string>
#include <tuple>
#include <utility>
#include <stack>

using namespace std;

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

    // I/O counters for the index file (reset per operation)
    long long idxReads = 0;
    long long idxWrites = 0;

    void writeNode(const Node& node, int position);
    int writeNode(const Node& node);
    Node readNode(int position);

    // Header helpers
    void updateHeader();                 // write header with n=-1, keys[0]=m, children[0]=root
    bool loadAndValidateHeader();        // read and validate header (m); init if missing

public:
    MWayTree();
    explicit MWayTree(int order);
    ~MWayTree();

    bool openBinary(const std::string& filename);
    void closeBinary();

    // Creates binary index from text, writing header with provided order (m)
    static bool createFromText(const std::string& textFilename, const std::string& binFilename, int order);

    // Export binary index back to text file (sequential nodes 1..N)
    bool exportToText(const std::string& textFilename) const;

    void displayTree(const std::string& binFilename) const;

    void createRoot(const Node& node);

    // mSearch algorithm
    std::tuple<int, int, bool> mSearch(int key, stack<int>* branch = nullptr);

    // insertB algorithm
    void insertB(int key);

    // Counters API
    void resetCounters();
    std::pair<long long,long long> getCounters() const;
};

#endif