#include "index.hpp"
#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include "chunk.hpp"
#include <tuple>
#include <map>

#include <CkString.h>

InvertedIndex::InvertedIndex(){
    this->index = new std::unordered_map<std::string, Entry*>;
    this->subIndexList = new std::vector<std::string>;
    this->numInsertions = 0;
}

InvertedIndex::~InvertedIndex(){
    exportIndex();
    mergeSubIndexes();
    for(auto itr = this->index->begin(); itr != this->index->end(); itr++){ 
        delete itr->second;
    } 
    delete this->index;
    delete this->subIndexList;
}

void InvertedIndex::insert(std::string word, int docId, int position){
    if(this->numInsertions >= SUB_INDEX_SIZE){
        // std::cout << "EXPORTING after hitting" << numInsertions << std::endl;
        exportIndex();
        clearIndex();
    }

    auto got = this->index->find(word);
    if(got == this->index->end())
        this->index->insert(std::pair<std::string, Entry*>(word, new Entry()));
    this->index->at(word)->insertOccurrence(docId, position);
    this->numInsertions++;
}

void InvertedIndex::exportIndex(){

    std::string filename = "sub_" + std::to_string(this->subIndexList->size()) + ".sidx";
    std::ofstream subIndexFile("output/" + filename);
    //sort before exporting

    //OP1
    std::map<std::string, Entry*> orderedIndex(this->index->begin(), this->index->end());

    for(auto i = orderedIndex.begin(); i != orderedIndex.end(); i++){ 
        for(auto j = i->second->umap->begin(); j != i->second->umap->end(); j++){ 
            for(auto k = j->second->begin(); k != j->second->end(); k++){ 
                subIndexFile << i->first << " " << j->first << " " << *k  << "\n";
            }
        }
    }

    //OP2
    // std::map<std::string, Entry*> orderedIndex(this->index->begin(), this->index->end());
    //     std::vector<std::string> v;
    //     for(auto it = orderedIndex.begin(); it != orderedIndex.end(); ++it) {
    //         v.push_back(it->first);
    //     }
    //     sort(v.begin(), v.end(), std::locale(""));
    // for(auto i = v.begin(); i != v.end(); i++){ 
    //     // subIndexFile << *i << "\n";
    //     auto _entry = orderedIndex[*i];
    //     for(auto j = _entry->umap->begin(); j != _entry->umap->end(); j++){ 
    //         for(auto k = j->second->begin(); k != j->second->end(); k++){ 
    //             subIndexFile << *i << " " << j->first << " " << *k  << "\n";
    //         }
    //     }
    // }

    subIndexFile.close();
    this->subIndexList->push_back(filename);
}

void InvertedIndex::clearIndex(){
    for(auto itr = this->index->begin(); itr != this->index->end(); itr++){ 
        delete itr->second;
    } 
    delete this->index;
    this->index = new std::unordered_map<std::string, Entry*>;
    this->numInsertions = 0;
}

void InvertedIndex::to_string(){
    for(auto itr = this->index->begin(); itr != this->index->end(); itr++){ 
        std::cout << itr->first << " "; 
        itr->second->to_string();
        std::cout << std::endl;
    } 
}

unsigned long InvertedIndex::sizeBytes(){
    unsigned long size = sizeof(*(this->index));
    for(auto itr = this->index->begin(); itr != this->index->end(); itr++){ 
        size += itr->first.size() + itr->second->sizeBytes();
    }
    return size;
}

int InvertedIndex::vocabularySize(){
    return this->index->size();
}

void InvertedIndex::occurrenceToCSV(std::string filename){
    std::ofstream csv(filename);
    csv << "str,occ\n";
    for(auto itr = this->index->begin(); itr != this->index->end(); itr++){ 
        csv << itr->first << "," << itr->second->totalOccurences() << "\n";
    }
    csv.close();

}

double InvertedIndex::avgInvertedListSize(){
    int numLists = 0;
    for(auto itr = this->index->begin(); itr != this->index->end(); itr++){ 
        numLists += itr->second->invertedListSize();
    }
    return (double)numLists/(double)this->vocabularySize();
}

void InvertedIndex::mergeSubIndexes(){
    std::ofstream mergedFile("output/merged.idx");
    std::ofstream vocabulary("output/vocab.txt");

    int numSubIndexes = this->subIndexList->size();
    bool sortingDone = false;
    
    std::vector<Chunk*>* chunks = new std::vector<Chunk*>(numSubIndexes);
    for(int i=0; i<numSubIndexes; i++){
        chunks->at(i) = new Chunk(SORT_CHUNK_SIZE);
        chunks->at(i)->setSource("output/"+this->subIndexList->at(i));
        chunks->at(i)->fill();
    }

    std::string lastSmallest = "";
    while(!sortingDone){
        sortingDone = true;
        //fetch the smallest occ among all the chunks
        int smallest = 0;
        for(int i=0; i<numSubIndexes; i++){
            if(!chunks->at(i)->gotEof()){
                if(chunks->at(i)->isEmpty()){
                    chunks->at(i)->fill();
                    if(chunks->at(i)->gotEof())
                        continue;
                }
                if(sortingDone)
                    smallest = i;
                sortingDone = false;

                if(std::get<0>(chunks->at(i)->getFirst()) < std::get<0>(chunks->at(smallest)->getFirst())){
                    smallest = i;
                }
            }
        }        
        if(sortingDone)
            break;

        //save the smallest to definitive file        
        // std::string buff = chunks->at(smallest)->firstIntoString();
        std::tuple<std::string, int, int> tup = chunks->at(smallest)->getFirst();

        if(std::get<0>(tup) != lastSmallest){
            vocabulary << std::get<0>(tup) << std::endl;
            lastSmallest = std::get<0>(tup);
        }

        std::stringstream ss;
        std::string line;
        ss << std::get<0>(tup) << " " << std::get<1>(tup)  << " " << std::get<2>(tup) << "\n";
        line = ss.str();
        mergedFile << line;
        
        chunks->at(smallest)->removeFirst();
    }
    mergedFile.close();

    for(int i=0; i<numSubIndexes; i++){
        delete chunks->at(i);
    }
    delete chunks;

}
