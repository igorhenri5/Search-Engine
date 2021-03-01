#include "entry.hpp"
#include <iostream>

Entry::Entry(){
    this->umap = new std::unordered_map<int, std::vector<int>*>;
}

Entry::~Entry(){
    for(auto itr = this->umap->begin(); itr != this->umap->end(); itr++){ 
        delete itr->second;
    } 
    delete this->umap;
}

void Entry::insertOccurrence(int docId, int position){
    auto got = this->umap->find(docId);

    if(got == this->umap->end())
        this->umap->insert(std::pair<int, std::vector<int>*>(docId, new std::vector<int>));
    this->umap->at(docId)->push_back(position);
}

void Entry::to_string(){
    for(auto itr = this->umap->begin(); itr != this->umap->end(); itr++){ 
        std::cout << "[ doc: " << itr->first << " occ: " << itr->second->size() << " { ";
        for(auto itr2 = itr->second->begin(); itr2 != itr->second->end(); itr2++){ 
            std::cout << *itr2 << ", ";
        } 
        std::cout << " }";
    } 
}

unsigned long Entry::sizeBytes(){
    unsigned long size = sizeof(*(this->umap));
    for(auto itr = this->umap->begin(); itr != this->umap->end(); itr++){ 
        size += sizeof(int);
        size += sizeof(int)*itr->second->capacity();
    }
    return size;
}

int Entry::invertedListSize(){
    return this->umap->size();
}

int Entry::totalOccurences(){
    int size = 0;
    for(auto itr = this->umap->begin(); itr != this->umap->end(); itr++){ 
        size += itr->second->size();
    }
    return size;
}