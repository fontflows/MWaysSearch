#ifndef DATAFILE_H
#define DATAFILE_H

#include <fstream>
#include <string>
#include <vector>
#include <utility>

struct Record {
    int key;
    char payload[64];
};

class DataFile {
private:
    std::fstream file;
    std::string filename;

    // I/O counters (reset per operation)
    long long reads = 0;
    long long writes = 0;

public:
    DataFile() = default;
    ~DataFile();

    bool open(const std::string& fname);
    void close();

    // Create from text file (scans all keys and writes a record for each)
    static bool createFromText(const std::string& textFilename, const std::string& dataFilename);

    // Sequential search by key
    bool find(int key, Record& out);

    // Append record (does not check duplicates)
    bool insert(const Record& rec);

    // Print all records
    void printAll();

    // Counters API
    void resetCounters();
    std::pair<long long,long long> getCounters() const;
};

#endif
