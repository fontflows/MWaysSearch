#include "MWayTree.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <stack>

using namespace std;

Node::Node() : n(0) {
    fill(begin(keys), end(keys), 0);
    fill(begin(children), end(children), 0);
}

MWayTree::MWayTree() : file(), filename(), root(1), m(3) {}

MWayTree::MWayTree(int order) : file(), filename(), root(1) {
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
 * @brief Writes a node to the binary file at the specified position
 */
void MWayTree::writeNode(const Node& node, int position) {
    file.seekp(static_cast<std::streamoff>(position) * sizeof(Node), ios::beg);
    file.write(reinterpret_cast<const char*>(&node), sizeof(Node));
    file.flush();
    idxWrites++;
}

/**
 * @brief Writes a node to the binary file at EOF and returns the position where it was written
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
 * @brief Reads a node from the binary file at the specified position
 */
Node MWayTree::readNode(int position) {
    Node node{};
    file.seekg(static_cast<std::streamoff>(position) * sizeof(Node), ios::beg);
    file.read(reinterpret_cast<char*>(&node), sizeof(Node));
    idxReads++;
    return node;
}

bool MWayTree::openBinary(const string& filename_) {
    filename = filename_;
    file.open(filename, ios::in | ios::out | ios::binary);
    if (!file.is_open()) return false;
    // Validate or initialize header
    if (!loadAndValidateHeader()) {
        file.close();
        return false;
    }
    return true;
}

void MWayTree::closeBinary() {
    if (file.is_open()) {
        // Persist header (m, root)
        updateHeader();
        file.close();
    }
}

/**
 * @brief Header layout: Node at position 0 with n=-1 sentinel
 * keys[0] = m, children[0] = root
 */
void MWayTree::updateHeader() {
    if (!file.is_open()) return;
    Node hdr{};
    hdr.n = -1;
    hdr.keys[0] = m;
    hdr.children[0] = root;
    // direct seek/write without counting (header meta)
    file.seekp(0, ios::beg);
    file.write(reinterpret_cast<const char*>(&hdr), sizeof(Node));
    file.flush();
}

bool MWayTree::loadAndValidateHeader() {
    if (!file.is_open()) return false;
    // read header
    Node hdr{};
    file.seekg(0, ios::beg);
    file.read(reinterpret_cast<char*>(&hdr), sizeof(Node));
    // If header is present, validate; else initialize
    if (hdr.n == -1) {
        int fileM = hdr.keys[0];
        int fileRoot = hdr.children[0];
        if (fileM != m) {
            cerr << "Error: index built with m=" << fileM << " but runtime m=" << m << endl;
            return false;
        }
        if (fileRoot > 0) root = fileRoot;
        return true;
    } else {
        // Initialize header with current m and root=1 (legacy file)
        root = 1;
        updateHeader();
        return true;
    }
}

/**
 * @brief Creates a binary file from a text file. Writes header with order (m).
 */
bool MWayTree::createFromText(const string& textFilename, const string& binFilename, int order) {
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

    // Header block (sentinel)
    Node hdr{};
    hdr.n = -1;
    hdr.keys[0] = (order < 3 ? 3 : (order > MAX_M ? MAX_M : order));
    hdr.children[0] = 1; // root at 1 initially
    binFile.write(reinterpret_cast<const char*>(&hdr), sizeof(Node));

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
 * @brief Export the binary index to a text file (same format used in input)
 */
bool MWayTree::exportToText(const std::string& textFilename) const {
    ifstream binFile(filename, ios::binary);
    if (!binFile.is_open()) return false;

    ofstream txt(textFilename, ios::trunc);
    if (!txt.is_open()) {
        binFile.close();
        return false;
    }

    Node node;
    // skip header
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
 * @brief Displays the tree structure from the binary file
 */
void MWayTree::displayTree(const string& binFilename) const {
    cout << "T = " << root << ", m = " << m << endl;
    cout << "------------------------------------------------------------------" << endl;
    cout << "No n,A[0],(K[1],A[1]),...,(K[n],A[n])" << endl;
    cout << "------------------------------------------------------------------" << endl;

    ifstream binFile(binFilename, ios::binary);
    if (!binFile.is_open()) return;

    Node node;
    binFile.read(reinterpret_cast<char*>(&node), sizeof(Node)); // Skip header

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
 * @brief Creates a new root given a node.
 * Shifts only nodes >= 1 one position forward (header at 0 is not moved).
 */
void MWayTree::createRoot(const Node& node){
    file.flush();
    file.seekg(0, ios::end);
    std::streampos endPos = file.tellg();
    int count = static_cast<int>(endPos / static_cast<std::streampos>(sizeof(Node)));

    // shift nodes 1..count-1 -> 2..count (do not move header at 0)
    for (int position = count - 1; position >= 1; --position) {
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
    root = 1;
    updateHeader();
}

/**
 * @brief Searches for a key in the m-way tree
 * Returns (node, position, found). position: 1-based only when found; else 0..n (slot).
 */
tuple<int, int, bool> MWayTree::mSearch(int key, stack<int>* branch) {
    if (!file.is_open()) {
        return make_tuple(0, 0, false);
    }

    resetCounters();

    int currentNode = root;
    if(branch){
        branch->push(currentNode);
    }

    while (currentNode != 0) {
        Node node = readNode(currentNode);

        int i = 0;
        while (i < node.n && key > node.keys[i]) {
            i++;
        }

        if (i < node.n && key == node.keys[i]) {
            return make_tuple(currentNode, i + 1, true); // 1-based on hit
        }

        if (node.children[i] == 0) {
            return make_tuple(currentNode, i, false); // insertion slot (0..n)
        }

        currentNode = node.children[i];
        if(branch){
            branch->push(currentNode);
        }
    }

    return make_tuple(0, 0, false);
}

/**
 * @brief Inserts a key into the m-way tree (bottom-up with split)
 */
void MWayTree::insertB(int key){
    if (!file.is_open()){
        return;
    }

    resetCounters();

    // current branch
    stack<int> branch;
    branch.push(0);
    auto [node, pos, found] = mSearch(key, &branch);
    // mSearch already reset counters; keep cumulative for insert path
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
        // get parent node from branch
        branch.pop();
        node = branch.top();
    }

    // create new root at level above, considering the file shift semantics
    Node newRoot{};
    newRoot.n = 1;
    newRoot.keys[0] = key;
    newRoot.children[0] = pNode + 1;
    newRoot.children[1] = qNode + 1;
    createRoot(newRoot);
}
