#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>
#include <chrono>
#include <cstdlib>
#include <map>
#include <chrono>

#include <CkSpider.h>

#include "util/util.hpp"
#include "index/index.hpp"

#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h" 

using namespace std;
using namespace rapidjson;

namespace Index{
    InvertedIndex* index;
}

void extractAndInsertAll(std::string text, int docId){
    std::stringstream ss(text);
    std::string buffer;

    int wordCount = 0;
    while(ss >> buffer){
        Index::index->insert(buffer, docId, wordCount);
        wordCount++;
    }
}

int main(int argc, char* argv[]){
    auto t1 = std::chrono::high_resolution_clock::now();
    if(argc < 2)
        return 1;

    std::string collectionFilePath = argv[1]; //collection.jl
    std::string line, url, html_content;
    
    Index::index = new InvertedIndex();
    std::map<std::string, int> pages;
    
    int i = 1;

    cout << collectionFilePath << endl;
    ifstream collectionFile(collectionFilePath);

    if(!collectionFile.is_open()){
        perror("Error open");
        exit(EXIT_FAILURE);
    }
    while(getline(collectionFile, line)){
        Document document;
        if(document.Parse(line.c_str()).HasParseError()){
            cout << "PARSE ERR" << endl;
            return 1;
        }
        if(document["url"].IsString()){
            url = document["url"].GetString();
            // cout << url << endl;
        }
        if(document["html_content"].IsString()){
            html_content = document["html_content"].GetString();
            // cout << html_content << endl;
        }
        std::string result = Util::htmlPreProcessing(html_content);
        extractAndInsertAll(result, i);
        pages[url] = i;
        i++;
        // if(i>=1000) break;
    }

    delete Index::index;

    auto t2 = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>( t2 - t1 ).count();
    std::cout << duration << "mili" << std::endl;
    duration = std::chrono::duration_cast<std::chrono::seconds>( t2 - t1 ).count();
    std::cout << duration << "sec" << std::endl;

    return 0;
}
