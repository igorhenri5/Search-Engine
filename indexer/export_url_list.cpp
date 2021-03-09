#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>
#include <chrono>
#include <cstdlib>
#include <map>
#include <chrono>

#include "../util/util.hpp"
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h" 

using namespace std;
using namespace rapidjson;

int main(int argc, char* argv[]){
    if(argc < 2)
        return 1;

    std::string collectionFilePath = argv[1]; //collection.jl
    std::string line, url, html_content;
    
    int i = 1;

    ifstream collectionFile(collectionFilePath);
    // ofstream urlsFile("output/urlset");

    if(!collectionFile.is_open()){
        perror("Error open");
        exit(EXIT_FAILURE);
    }
    auto t1 = std::chrono::high_resolution_clock::now();
    while(getline(collectionFile, line)){
        Document document;
        if(document.Parse(line.c_str()).HasParseError()){
            cout << "PARSE ERR" << endl;
            return 1;
        }
        if(document["url"].IsString()){
            url = document["url"].GetString();
            cout << url << endl;
            // urlsFile << url << endl;
        }
        if(i>=50000) break;
        i++;
    }
    auto t2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = t2 - t1;
    std::cout << "Elapsed Time: " << elapsed.count() << " milliseconds" << std::endl;
    
    return 0;
}
