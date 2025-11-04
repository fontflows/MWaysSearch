/**
* @file DataFile.h
 * @authors
 *   Francisco Eduardo Fontenele - 15452569
 *   Vinicius Botte - 15522900
 *
 * AED II - Trabalho 1
 */

#ifndef DATAFILE_H
#define DATAFILE_H

#include <fstream>
#include <string>
#include <utility>
#include <vector>

struct Record {
    int key;
    int active;       // 1 = ativo, 0 = removido
    char payload[64];
};

class DataFile {
private:
    std::fstream file;
    std::string filename;
    long long reads = 0;
    long long writes = 0;

public:
    DataFile() = default;
    ~DataFile();

    bool open(const std::string& fname);
    void close();

    static bool createFromText(const std::string& textFilename, const std::string& dataFilename);
    // Novo: cria data.bin a partir de employees.txt (id;nome;depto)
    static bool createFromEmployees(const std::string& employeesTxt, const std::string& dataFilename);

    bool find(int key, Record& out);
    bool insert(const Record& rec);
    bool remove(int key);
    void printAll();

    // Novo: lista todas as chaves ativas do arquivo principal
    bool listActiveKeys(std::vector<int>& outKeys);

    void resetCounters();
    std::pair<long long,long long> getCounters() const;
};

#endif
