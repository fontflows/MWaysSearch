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

/**
 * @brief Registro armazenado no arquivo principal.
 * @details Campos: key (chave), active (1=ativo, 0=removido logicamente) e payload (texto fixo de 64 bytes).
 */
struct Record {
    int key;
    int active;
    char payload[64];
};

/**
 * @brief Acesso ao arquivo principal binário (dados).
 * @details Oferece abrir/fechar, criação a partir de .txt/CSV simples, busca sequencial,
 *          inserção ao final, remoção lógica e listagem de chaves ativas. Expõe contadores de I/O.
 */
class DataFile {
private:
    std::fstream file;
    std::string filename;
    long long reads = 0;
    long long writes = 0;

public:
    DataFile() = default;
    /**
     * @brief Destrutor: garante fechamento do arquivo, se aberto.
     */
    ~DataFile();

    /**
     * @brief Abre (ou cria) o arquivo binário de dados.
     * @param fname Caminho do arquivo .bin.
     * @return true se aberto/criado com sucesso.
     */
    bool open(const std::string& fname);

    /**
     * @brief Fecha o arquivo binário se estiver aberto.
     */
    void close();

    /**
     * @brief Gera o arquivo de dados a partir de um .txt de nós (1 registro por chave).
     * @param textFilename Caminho do .txt (mesmo layout do índice).
     * @param dataFilename Caminho do data.bin de saída.
     * @return true em caso de sucesso.
     */
    static bool createFromText(const std::string& textFilename, const std::string& dataFilename);

    /**
     * @brief Gera o data.bin a partir de employees.txt no formato "id;Nome;Depto".
     * @param employeesTxt Caminho do arquivo texto de entrada.
     * @param dataFilename Caminho do data.bin de saída.
     * @return true em caso de sucesso.
     */
    static bool createFromEmployees(const std::string& employeesTxt, const std::string& dataFilename);

    /**
     * @brief Busca sequencial pelo primeiro registro ativo com a chave.
     * @param key Chave a procurar.
     * @param out Saída: registro encontrado (se true).
     * @return true se encontrado (O(n)).
     */
    bool find(int key, Record& out);

    /**
     * @brief Insere um registro ativo no final do arquivo.
     * @param rec Registro de entrada (active é forçado para 1).
     * @return true se a escrita foi bem-sucedida.
     */
    bool insert(const Record& rec);

    /**
     * @brief Insere funcionario com nome/depto (monta payload).
     * @param key Chave do funcionario.
     * @param nome Nome do funcionario.
     * @param depto Depto do funcionario.
     * @return true se a escrita foi bem-sucedida.
     */
    bool insertEmployee(int key, const std::string& nome, const std::string& depto);

    /**
     * @brief Remove logicamente (active=0) o primeiro registro ativo com a chave.
     * @param key Chave a remover.
     * @return true se encontrou e marcou como removido.
     */
    bool remove(int key);

    /**
     * @brief Imprime todos os registros (ativos/removidos) em stdout para inspeção.
     */
    void printAll();

    /**
     * @brief Coleta todas as chaves ativas presentes no arquivo.
     * @param outKeys Vetor de saída com as chaves ativas.
     * @return true se leitura executada.
     */
    bool listActiveKeys(std::vector<int>& outKeys);

    /**
     * @brief Zera contadores de I/O (reads/writes) da última operação.
     */
    void resetCounters();

    /**
     * @brief Retorna os contadores de I/O acumulados desde o último reset.
     * @return Par (reads, writes).
     */
    std::pair<long long,long long> getCounters() const;
};

#endif
