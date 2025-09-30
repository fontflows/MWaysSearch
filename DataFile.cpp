#include "DataFile.h"
#include <iostream>
#include <sstream>

using namespace std;

DataFile::~DataFile() {
    close();
}

bool DataFile::open(const std::string& fname) {
    filename = fname;
    file.open(filename, ios::in | ios::out | ios::binary);
    if (!file.is_open()) {
        // create if not exists
        file.clear();
        file.open(filename, ios::out | ios::binary | ios::trunc);
        if (!file.is_open()) return false;
        file.close();
        file.open(filename, ios::in | ios::out | ios::binary);
    }
    return file.is_open();
}

void DataFile::close() {
    if (file.is_open()) file.close();
}

bool DataFile::createFromText(const std::string& textFilename, const std::string& dataFilename) {
    ifstream txt(textFilename);
    if (!txt.is_open()) return false;

    ofstream out(dataFilename, ios::binary | ios::trunc);
    if (!out.is_open()) {
        txt.close();
        return false;
    }

    string line;
    while (getline(txt, line)) {
        if (line.empty()) continue;
        istringstream iss(line);
        int n, a0;
        iss >> n >> a0;
        for (int i = 0; i < n; ++i) {
            int k, ai;
            iss >> k >> ai;
            Record r{};
            r.key = k;
            // payload sample
            std::snprintf(r.payload, sizeof(r.payload), "record-%d", k);
            out.write(reinterpret_cast<const char*>(&r), sizeof(Record));
        }
    }

    txt.close();
    out.close();
    return true;
}

bool DataFile::find(int key, Record& out) {
    if (!file.is_open()) return false;
    resetCounters();
    file.clear();
    file.seekg(0, ios::beg);
    Record r{};
    while (file.read(reinterpret_cast<char*>(&r), sizeof(Record))) {
        reads++;
        if (r.key == key) {
            out = r;
            return true;
        }
    }
    return false;
}

bool DataFile::insert(const Record& rec) {
    if (!file.is_open()) return false;
    file.clear();
    file.seekp(0, ios::end);
    file.write(reinterpret_cast<const char*>(&rec), sizeof(Record));
    file.flush();
    writes++;
    return true;
}

void DataFile::printAll() {
    if (!file.is_open()) return;
    file.clear();
    file.seekg(0, ios::beg);
    Record r{};
    cout << "------------------- data.bin -------------------" << endl;
    while (file.read(reinterpret_cast<char*>(&r), sizeof(Record))) {
        cout << "key=" << r.key << " | payload=\"" << r.payload << "\"" << endl;
    }
    cout << "------------------------------------------------" << endl;
}

void DataFile::resetCounters() {
    reads = 0;
    writes = 0;
}

pair<long long,long long> DataFile::getCounters() const {
    return {reads, writes};
}
