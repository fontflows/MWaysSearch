/**
* @file DataFile.cpp
 * @authors
 *   Francisco Eduardo Fontenele - 15452569
 *   Vinicius Botte - 15522900
 *
 * AED II - Trabalho 1
 */

#include "DataFile.h"
#include <iostream>
#include <sstream>
#include <cstdio>
#include <cctype>

using namespace std;

/**
 * @brief Destrutor: fecha o arquivo se ainda aberto.
 */
DataFile::~DataFile() {
    close();
}

/**
 * @brief Abre (ou cria) o arquivo binário de dados.
 * @param fname Caminho do arquivo.
 * @return true se aberto com sucesso.
 */
bool DataFile::open(const std::string& fname) {
    filename = fname;
    file.open(filename, ios::in | ios::out | ios::binary);
    if (!file.is_open()) {
        file.clear();
        file.open(filename, ios::out | ios::binary | ios::trunc);
        if (!file.is_open()) return false;
        file.close();
        file.open(filename, ios::in | ios::out | ios::binary);
    }
    return file.is_open();
}

/**
 * @brief Fecha o arquivo binário se aberto.
 */
void DataFile::close() {
    if (file.is_open()) file.close();
}

/**
 * @brief Constrói data.bin a partir de um .txt de nós (gera 1 registro por chave).
 * @param textFilename Caminho do .txt (linhas: "n A0 K1 A1 ... Kn An").
 * @param dataFilename Caminho do data.bin de saída (sobrescrito).
 * @return true se criado com sucesso; false ao detectar formato inválido.
 */
bool DataFile::createFromText(const std::string& textFilename, const std::string& dataFilename) {
    ifstream txt(textFilename);
    if (!txt.is_open()) return false;

    ofstream out(dataFilename, ios::binary | ios::trunc);
    if (!out.is_open()) {
        txt.close();
        return false;
    }

    string line;
    int lineNo = 0;
    while (getline(txt, line)) {
        ++lineNo;
        bool onlySpace = true;
        for (char c : line) { if (!std::isspace(static_cast<unsigned char>(c))) { onlySpace = false; break; } }
        if (onlySpace) continue;
        istringstream iss(line);
        int n, a0;
        if (!(iss >> n >> a0)) {
            cerr << "DataFile: erro ao ler n e A0 na linha " << lineNo << endl;
            txt.close(); out.close();
            return false;
        }
        for (int i = 0; i < n; ++i) {
            int k, ai;
            if (!(iss >> k >> ai)) {
                cerr << "DataFile: par Ki/Ai faltando na linha " << lineNo << endl;
                txt.close(); out.close();
                return false;
            }
            Record r{};
            r.key = k;
            r.active = 1;
            char dept = "ABCDE"[k % 5];
            std::snprintf(r.payload, sizeof(r.payload), "Funcionario %d | depto=%c", k, dept);
            out.write(reinterpret_cast<const char*>(&r), sizeof(Record));
        }
    }

    txt.close();
    out.close();
    return true;
}

/**
 * @brief Constrói data.bin a partir de employees.txt no formato "id;Nome;Depto".
 * @param employeesTxt Caminho do arquivo texto de entrada.
 * @param dataFilename Caminho do data.bin de saída (sobrescrito).
 * @return true se criado; false se houver linhas inválidas ou falha de I/O.
 */
bool DataFile::createFromEmployees(const std::string& employeesTxt, const std::string& dataFilename) {
    ifstream in(employeesTxt);
    if (!in.is_open()) return false;

    ofstream out(dataFilename, ios::binary | ios::trunc);
    if (!out.is_open()) {
        in.close();
        return false;
    }

    string line;
    int lineNo = 0;
    while (getline(in, line)) {
        ++lineNo;
        bool onlySpace = true;
        for (char c : line) { if (!std::isspace(static_cast<unsigned char>(c))) { onlySpace = false; break; } }
        if (onlySpace) continue;

        int id = 0;
        string nome, depto;

        size_t p1 = line.find(';');
        size_t p2 = (p1 == string::npos) ? string::npos : line.find(';', p1 + 1);
        if (p1 == string::npos || p2 == string::npos) {
            cerr << "employees.txt: formato invalido na linha " << lineNo << endl;
            in.close(); out.close();
            return false;
        }
        try {
            id = stoi(line.substr(0, p1));
        } catch (...) {
            cerr << "employees.txt: id invalido na linha " << lineNo << endl;
            in.close(); out.close();
            return false;
        }
        nome = line.substr(p1 + 1, p2 - (p1 + 1));
        depto = line.substr(p2 + 1);

        auto trim = [](string& s){
            size_t a = s.find_first_not_of(" \t\r\n");
            size_t b = s.find_last_not_of(" \t\r\n");
            if (a == string::npos) { s.clear(); return; }
            s = s.substr(a, b - a + 1);
        };
        trim(nome); trim(depto);

        Record r{};
        r.key = id;
        r.active = 1;
        string payload = "Funcionario " + to_string(id) + " | " + nome + " | " + depto;
        if (payload.size() >= sizeof(r.payload)) {
            payload.resize(sizeof(r.payload) - 1);
        }
        std::snprintf(r.payload, sizeof(r.payload), "%s", payload.c_str());

        out.write(reinterpret_cast<const char*>(&r), sizeof(Record));
    }

    in.close();
    out.close();
    return true;
}

/**
 * @brief Busca sequencial por chave (apenas registros active=1).
 * @param key Chave a procurar.
 * @param out Registro de saída, se encontrado.
 * @return true se encontrado (O(n)); counters são atualizados.
 */
bool DataFile::find(int key, Record& out) {
    if (!file.is_open()) return false;
    resetCounters();
    file.clear();
    file.seekg(0, ios::beg);
    Record r{};
    while (file.read(reinterpret_cast<char*>(&r), sizeof(Record))) {
        reads++;
        if (r.key == key && r.active == 1) {
            out = r;
            return true;
        }
    }
    return false;
}

/**
 * @brief Insere um registro ativo ao final do arquivo (append).
 * @param rec Registro de entrada; active será definido como 1.
 * @return true se escrita OK; counters são atualizados.
 */
bool DataFile::insert(const Record& rec) {
    if (!file.is_open()) return false;
    resetCounters();
    Record w = rec;
    w.active = 1;
    file.clear();
    file.seekp(0, ios::end);
    file.write(reinterpret_cast<const char*>(&w), sizeof(Record));
    file.flush();
    writes++;
    return file.good();
}

/**
 * @brief Marca como removido (active=0) o primeiro registro ativo com a chave.
 * @param key Chave a remover.
 * @return true se encontrou e marcou; counters são atualizados.
 */
bool DataFile::remove(int key) {
    if (!file.is_open()) return false;
    resetCounters();
    file.clear();
    file.seekg(0, ios::beg);
    Record r{};
    std::streampos pos{};
    while (true) {
        pos = file.tellg();
        if (!file.read(reinterpret_cast<char*>(&r), sizeof(Record))) break;
        reads++;
        if (r.key == key && r.active == 1) {
            r.active = 0;
            file.clear();
            file.seekp(pos, ios::beg);
            file.write(reinterpret_cast<const char*>(&r), sizeof(Record));
            file.flush();
            writes++;
            return true;
        }
    }
    return false;
}

/**
 * @brief Imprime todos os registros (ativos e removidos) para depuração.
 */
void DataFile::printAll() {
    if (!file.is_open()) return;
    file.clear();
    file.seekg(0, ios::beg);
    Record r{};
    cout << "------------------- data.bin -------------------" << endl;
    while (file.read(reinterpret_cast<char*>(&r), sizeof(Record))) {
        cout << "key=" << r.key << " | active=" << r.active << " | payload=\"" << r.payload << "\"" << endl;
    }
    cout << "------------------------------------------------" << endl;
}

/**
 * @brief Lê todas as chaves com active=1 e devolve em outKeys.
 * @param outKeys Vetor de saída com as chaves ativas.
 * @return true se leitura executada; counters não são alterados.
 */
bool DataFile::listActiveKeys(std::vector<int>& outKeys) {
    if (!file.is_open()) return false;
    file.clear();
    file.seekg(0, ios::beg);
    Record r{};
    outKeys.clear();
    while (file.read(reinterpret_cast<char*>(&r), sizeof(Record))) {
        if (r.active == 1) outKeys.push_back(r.key);
    }
    return true;
}

/**
 * @brief Zera contadores de I/O da última operação.
 */
void DataFile::resetCounters() {
    reads = 0;
    writes = 0;
}

/**
 * @brief Retorna os contadores de I/O acumulados desde o último reset.
 * @return Par (reads, writes).
 */
pair<long long,long long> DataFile::getCounters() const {
    return {reads, writes};
}
