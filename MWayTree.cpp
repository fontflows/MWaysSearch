#include "MWayTree.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cstring>
#include <cmath>
#include <algorithm>

using namespace std;

Node::Node() : n(0) {
    fill(begin(keys), end(keys), 0);
    fill(begin(children), end(children), 0);
}

MWayTree::MWayTree() : root(1), m(3) {}

MWayTree::MWayTree(int order) : root(1) {
    // clamp to valid on-disk fixed layout
    if (order < 3) m = 3;
    else if (order > MAX_M) m = MAX_M;
    else m = order;
}

MWayTree::~MWayTree() {
    closeBinary();
}

/**
 * @brief Writes a node to the binary file at the specified position
 * @pre The binary file must be open for writing
 * @post Node is written to the file at the specified position
 */
void MWayTree::writeNode(const Node& node, int position) {
    file.seekp(static_cast<std::streamoff>(position) * sizeof(Node), ios::beg);
    file.write(reinterpret_cast<const char*>(&node), sizeof(Node));
    file.flush();
}

/**
 * @brief Writes a node to the binary file at EOF and returns the position where it was written
 * @pre The binary file must be open for writing
 * @post Returns the position where the node was written
*/
int MWayTree::writeNode(const Node& node){
    file.seekp(0, ios::end);
    std::streampos endPos = file.tellp();
    int position = static_cast<int>(endPos / static_cast<std::streampos>(sizeof(Node)));
    file.write(reinterpret_cast<const char*>(&node), sizeof(Node));
    file.flush();
    return position;
}

/**
 * @brief Reads a node from the binary file at the specified position
 * @pre The binary file must be open for reading
 * @post Returns the node read from the file
 */
Node MWayTree::readNode(int position) {
    Node node{};
    file.seekg(static_cast<std::streamoff>(position) * sizeof(Node), ios::beg);
    file.read(reinterpret_cast<char*>(&node), sizeof(Node));
    return node;
}

/**
 * @brief Opens the binary file for operations
 * @pre The binary file must exist
 * @post Returns true if the file was successfully opened
 */
bool MWayTree::openBinary(const string& filename_) {
    filename = filename_;
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
    while (getline(textFile, line)) {
        if (line.empty()) continue;
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
        cout << setw(2) << nodeIndex << " " << node.n << ", " << node.children[0];
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
 * @brief Returns the parent of a node
 * @pre The child node and one of its keys are given as parameters
 * @post The parent node is returned
 */
int MWayTree::parent(int childNode, int key){
    int currentNode = root;
    int parentNode = 0;

    while (currentNode != childNode && currentNode != 0) {
        Node node = readNode(currentNode);

        int i = 0;
        while (i < node.n && key > node.keys[i]) {
            i++;
        }

        parentNode = currentNode;
        currentNode = node.children[i];
    }

    return parentNode;
}

/**
 * @brief Creates a new root given a node
 * @pre The binary file must exist
 * @post A new root is inserted into the tree
*/
void MWayTree::createRoot(const Node& node){
    // compute current node count
    file.flush();
    file.seekg(0, ios::end);
    std::streampos endPos = file.tellg();
    int count = static_cast<int>(endPos / static_cast<std::streampos>(sizeof(Node)));

    // shift all nodes (including header at 0) one position forward: i -> i+1
    for (int position = count - 1; position >= 0; --position) {
        Node temp = readNode(position);
        for (int i = 0; i < MAX_M + 1; i++) {
            if (temp.children[i] > 0) {
                temp.children[i]++; // update child references due to shift
            }
        }
        writeNode(temp, position + 1);
    }

    // write new root at position 1
    writeNode(node, 1);
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
            return make_tuple(currentNode, i + 1, true); // 1-based indexing on hit
        }

        if (node.children[i] == 0) {
            return make_tuple(currentNode, i, false); // position for insertion (0..n)
        }

        currentNode = node.children[i];
    }

    return make_tuple(0, 0, false);
}

/**
 * @brief Inserts a key into the m-way tree
 * @pre A key is given as parameter
 * @post The key is inserted into the m-way tree
 */
void MWayTree::insertB(int key){
    if (!file.is_open()){
        return;
    }

    auto [node, pos, found] = mSearch(key);
    if(found){
        return;
    }

    int pNode = 0, qNode = 0;

    while(node != 0){
        Node newNode = readNode(node);

        // find insertion index
        int i = 0;
        while(i < newNode.n && key > newNode.keys[i]){
            i++;
        }

        // shift keys to the right
        for(int j = newNode.n - 1; j >= i; --j){
            newNode.keys[j + 1] = newNode.keys[j];
        }
        // shift children to the right (for internal nodes)
        for(int j = newNode.n; j >= i + 1; --j){
            newNode.children[j + 1] = newNode.children[j];
        }

        // place key and updated children (if any from a lower-level split)
        newNode.keys[i] = key;
        if(pNode != 0){
            newNode.children[i] = pNode;
        }
        if(qNode != 0){
            newNode.children[i + 1] = qNode;
        }
        newNode.n++;

        // no overflow: write back and done
        if(newNode.n <= m - 1){
            writeNode(newNode, node);
            return;
        }

        // overflow: split node and promote median
        const int mid = m / 2; // median index in 0-based array after temporary n==m
        Node p{}, q{};

        // left node (p): 0..mid-1 keys, 0..mid children
        p.n = mid;
        for(int k = 0; k < p.n; ++k){
            p.keys[k] = newNode.keys[k];
        }
        for(int k = 0; k <= p.n; ++k){
            p.children[k] = newNode.children[k];
        }

        // right node (q): keys mid+1..newNode.n-1, children mid+1..newNode.n
        int qKeys = newNode.n - mid - 1;
        q.n = qKeys;
        for(int k = 0; k < qKeys; ++k){
            q.keys[k] = newNode.keys[mid + 1 + k];
        }
        for(int k = 0; k <= qKeys; ++k){
            q.children[k] = newNode.children[mid + 1 + k];
        }

        // write split nodes
        writeNode(p, node);
        pNode = node;
        qNode = writeNode(q);

        // promote median and go up
        key = newNode.keys[mid];
        node = parent(node, p.keys[0]);
    }

    // create new root at level above, considering the file shift semantics
    Node newRoot{};
    newRoot.n = 1;
    newRoot.keys[0] = key;
    newRoot.children[0] = pNode + 1;
    newRoot.children[1] = qNode + 1;
    createRoot(newRoot);
}
