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

struct Node {
    int n;
    int keys[MAX_M];
    int children[MAX_M+1];
    Node();
};

class MWayTree {
private:
    std::fstream file;
    std::string filename;
    int root;
    int m;
    long long idxReads = 0;
    long long idxWrites = 0;

    void writeNode(const Node& node, int position);
    int writeNode(const Node& node);
    Node readNode(int position);

    void updateHeader();
    bool loadAndValidateHeader();

    int minKeys() const;
    bool isLeaf(const Node& node) const;

    void fixUnderflow(int parentPos, int childIndex);
    enum class DelResult { Ok, Underflow, NotFound };
    DelResult deleteRecursive(int nodePos, int key);

public:
    MWayTree();
    explicit MWayTree(int order);
    ~MWayTree();

    bool openBinary(const std::string& filename);
    void closeBinary();

    static bool createFromText(const std::string& textFilename, const std::string& binFilename, int order);
    static bool createEmpty(const std::string& binFilename, int order);
    static bool readHeader(const std::string& binFilename, int& outM, int& outRoot);

    bool exportToText(const std::string& textFilename) const;
    void displayTree(const std::string& binFilename) const;

    void createRoot(const Node& node);

    std::tuple<int, int, bool> mSearch(int key, stack<int>* branch = nullptr);
    void insertB(int key);
    bool deleteB(int key);

    void resetCounters();
    std::pair<long long,long long> getCounters() const;

    bool verifyIntegrity(bool verbose = false) const;
};

#endif
