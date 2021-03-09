#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <chrono>
#include <map>

#include "../util/util.hpp"

using namespace std;

int main(int argc, char* argv[]){
    if(argc < 3)
        return 1;

    std::string indexFilePath = argv[1]; //collection.jl
    std::string docSizeOutput = argv[2]; //doc_size.txt
    std::string tfListOutput = argv[3];  //tf_list.txt

    std::string line, term;
    int docId;
    unsigned int pos;

    std::map<int, unsigned int> docSizeMap;
    
    ifstream indexFile(indexFilePath);
    if(!indexFile.is_open()){
        perror("Error open");
        exit(EXIT_FAILURE);
    }
    
    while(getline(indexFile, line)){
        std::stringstream ss;
        ss << line;
        ss >> term >> docId >> pos;
        if(!(docSizeMap.find(docId) == docSizeMap.end())){
            docSizeMap.at(docId) = docSizeMap.at(docId) + 1;
        }else{
            docSizeMap.insert({docId, 1});
        }
    }
    indexFile.close();

    ofstream outputFile(docSizeOutput);
    for(auto it = docSizeMap.begin(); it != docSizeMap.end(); ++it){
        outputFile << it->first << " " << it->second << std::endl;
    }
    outputFile.close();

// 

    indexFile.open(indexFilePath);
    if(!indexFile.is_open()){
        perror("Error open");
        exit(EXIT_FAILURE);
    }

    std::map<std::string, std::map<int, double>> tfList;
    
    while(getline(indexFile, line)){
        std::stringstream ss;
        ss << line;
        ss >> term >> docId >> pos;
        if(!(tfList.find(term) == tfList.end())){
            if(!(tfList.at(term).find(docId) == tfList.at(term).end())){
                tfList.at(term).at(docId) = tfList.at(term).at(docId) + 1.0/docSizeMap.at(docId);
            }else{
                tfList.at(term).insert({docId, 1.0/docSizeMap.at(docId)});
            }
        }else{
            tfList.insert({term, map<int, double>()});
            tfList.at(term).insert({docId, 1.0/docSizeMap.at(docId)});
        }
    }
    indexFile.close();

    
    outputFile.open(tfListOutput);
    for(auto it = tfList.begin(); it != tfList.end(); ++it){
        outputFile << it->first <<  " " << it->second.size() << std::endl;
        for(auto it2 = it->second.begin(); it2 != it->second.end(); ++it2){
            outputFile << it->first << " " << it2->first << " " << it2->second << std::endl;
        }
    }
    outputFile.close();

    return 0;
}
