#ifndef __Q_PROC__
#define __Q_PROC__

#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>
#include <chrono>
#include <cstdlib>
#include <math.h> 
#include <map>
#include <unordered_map>
#include <vector>
#include <tuple>
#include <utility>
#include <limits>
#include <algorithm>
#include <iostream>
#include "../../util/util.hpp"

class QueryProcessor{
private:
    std::string indexFilePath;
    std::string vocabularyFilePath;
    std::string urlsFilePath;
    std::string normsFilePath;

    std::ifstream indexFile;
    std::ifstream vocabularyFile;
    std::ifstream urlsFile;
    std::ifstream normFile;

    std::map<std::string, double>* queryWeightMap;
    std::unordered_map<int, double>* docsNormMap;
    std::unordered_map<std::string, std::pair<double, unsigned int>>* vocabulary;
    std::unordered_map<int, std::string>* urlMap;

    void load_queries();
    void load_urls();
    void load_vocabulary();
    void load_document_norms();
    // void read_index_fetch_weight();
    void pre_process_query(std::string query);

    static bool compare_doc_id_smaller(const std::pair<int, double> &a, const std::pair<int, double> &b);
    static bool compare_score_greater(const std::pair<int, double> &a, const std::pair<int, double> &b);

public:
    QueryProcessor(std::string, std::string, std::string, std::string);
    ~QueryProcessor();
    void retrieve_k_first_results(std::string, int);
    void to_string();

};


#endif