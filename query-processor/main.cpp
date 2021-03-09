#include <iostream>
#include <string>
#include <chrono>

#include "qproc/query_processor.hpp"

int main(int argc, char* argv[]){
    if(argc < 6)
        return 1;

    std::ifstream queriesFile(argv[1]);
    if(!queriesFile.is_open()){
        std::cerr << "Could not open query file" << std::endl;
        exit(1);
    }

    int k = std::stoi(argv[5]);   

        auto t1 = std::chrono::high_resolution_clock::now();
    QueryProcessor* queryProcessor = new QueryProcessor(argv[2], argv[3], argv[4]);
        auto t2 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = t2 - t1;
        std::cout << "Init Elapsed Time: " << elapsed.count() << " milliseconds" << std::endl;

    // std::string query;
    // int i=1;
    // while(getline(queriesFile, query)){
    //         t1 = std::chrono::high_resolution_clock::now();
    //     queryProcessor->retrieve_k_first_results(query, k);
    //         t2 = std::chrono::high_resolution_clock::now();
    //         elapsed = t2 - t1;
    //     std::cout << "\tQuery " << i << " :: " << query << "  --- in " << elapsed.count() << " ms" << std::endl;
    //     i++;
    // }
    queryProcessor->to_string();

    queriesFile.close();
    delete queryProcessor;

    return 0;
}