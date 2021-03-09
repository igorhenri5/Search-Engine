#include <iostream>
#include <string>
#include <chrono>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <math.h>

namespace pfetch{
    std::ifstream indexFile;
    std::ifstream vocabFile;
    std::ofstream normFile;

    std::unordered_map<int, double>* docsWeightMap;
    std::unordered_map<std::string, double>* vocabulary;
}

void merge_weight(std::unordered_map<int, double>* termTfs, std::string term){
    double weight = 0.0;
    for(auto doc=termTfs->begin(); doc!=termTfs->end(); ++doc){
        if(!(pfetch::vocabulary->find(term) == pfetch::vocabulary->end()))
            weight = (log2(doc->second)+1.0) * pfetch::vocabulary->at(term);

        if(!(pfetch::docsWeightMap->find(doc->first) == pfetch::docsWeightMap->end())){
            pfetch::docsWeightMap->at(doc->first) += weight * weight;
        }else{
            pfetch::docsWeightMap->insert({doc->first , weight * weight});
        }
    }
}

void load_vocabulary(){
    std::cout << "LV" << std::endl; 
    std::string line;
    std::string term;
    double idf;
    unsigned int offset;
    while(getline(pfetch::vocabFile,line)){
        std::stringstream* ss = new std::stringstream(line);
        *ss >> term >> idf >> offset;
        pfetch::vocabulary->insert({term, idf});
        delete ss;
    }
    pfetch::vocabFile.close();
    std::cout << "bg " << pfetch::vocabulary->begin()->first <<  " " << pfetch::vocabulary->begin()->second << std::endl; 
}

int main(int argc, char* argv[]){
    if(argc < 4)
        return 1;

    pfetch::indexFile.open(argv[1]);
    pfetch::vocabFile.open(argv[2]);

    pfetch::vocabulary = new std::unordered_map<std::string, double>;

//
        auto t1 = std::chrono::high_resolution_clock::now();
    load_vocabulary();
        auto t2 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = t2 - t1;
        std::cout << "LV Elapsed Time: " << elapsed.count() << " milliseconds" << std::endl;

//
        t1 = std::chrono::high_resolution_clock::now();
    if(!pfetch::indexFile.is_open()){
       std::cerr << "Could not open index file" << std::endl;
       exit(2); 
    }

    pfetch::docsWeightMap = new std::unordered_map<int, double>;

    std::string line;
    std::string term, currTerm; 
    unsigned int docId, currDocId;
    int position;
    bool done = false;

        getline(pfetch::indexFile, line);
        std::stringstream* ss = new std::stringstream(line);
        *ss >> term >> docId >> position;
        delete ss;
        currTerm = term;

    while(!done){        
        std::unordered_map<int, double>* currTf = new std::unordered_map<int, double>; //document tf
        while(currTerm == term){
            if(currTf->find(docId) == currTf->end()){
                currTf->insert({docId, 1.0});
            }else{
                currTf->at(docId) += 1.0;
            }

            if(!getline(pfetch::indexFile, line)){
                done = true;
                break;
            }
            std::stringstream* ss = new std::stringstream(line);
            *ss >> currTerm >> docId >> position;
            delete ss;
        }
        merge_weight(currTf, term);
        term = currTerm;
        delete currTf;
    }

    pfetch::indexFile.close();
        t2 = std::chrono::high_resolution_clock::now();
        elapsed = t2 - t1;
        std::cout << "DWM Elapsed Time: " << elapsed.count() << " milliseconds" << std::endl;
//  

    pfetch::normFile.open(argv[3]);
    double docWeight, benchsize;
    benchsize = 0.0;
    for(auto doc=pfetch::docsWeightMap->begin(); doc!=pfetch::docsWeightMap->end(); ++doc){
        // std::cout << doc->first << " " << sqrt(doc->second) << std::endl;
        pfetch::normFile << doc->first << " " << sqrt(doc->second) <<  std::endl;
    }
    pfetch::normFile.close();

    delete pfetch::vocabulary;
    delete pfetch::docsWeightMap;

    return 0;
}