#include "chunk.hpp"
#include <sstream>

#include <iostream>

Chunk::Chunk(int maxSize){
    this->eof = false;
    this->maxSize = maxSize;
    this->occurrences = new std::vector<std::tuple<std::string, int, int>>;
}

Chunk::~Chunk(){
    this->sourceFile.close();
    delete occurrences;
}

bool Chunk::setSource(std::string filename){
    this->filename = filename;
    this->sourceFile.open(filename);
    return true;
}

void Chunk::fill(){
    if(this->eof) return;

    std::stringstream ss;
    std::string word, line; 
    int docId, position;

    int i=0;
    while(true){ 
        if(i>=this->maxSize)
            break;
        if (!(this->sourceFile >> word)){ 
            this->eof = true;
            break;
        }
        if (!(this->sourceFile >> docId)){ 
            this->eof = true;
            break;
        }
        if (!(this->sourceFile >> position)){ 
            this->eof = true;
            break;
        }
        
        this->occurrences->push_back(make_tuple(word, docId, position));
        i++;
    }
    // this->to_string();
}

std::tuple<std::string, int, int> Chunk::getFirst(){
    return *this->occurrences->begin();
}

void Chunk::to_string(){
    for(auto it = this->occurrences->begin(); it != this->occurrences->end(); it++){
        std::cout << std::get<0>(*it) << " ";
    }  
    std::cout << std::endl;

} 

std::string Chunk::firstIntoString(){
    std::stringstream ss;
    std::string line;
    ss << std::get<0>(*this->occurrences->begin()) << " " << std::get<1>(*this->occurrences->begin())  << " " << std::get<2>(*this->occurrences->begin()) << "\n";
    line = ss.str();
    return line;
}

void Chunk::removeFirst(){
    // this->to_string();
    this->occurrences->erase(this->occurrences->begin());
}

bool Chunk::isEmpty(){
    return this->occurrences->empty();
}

bool Chunk::gotEof(){
    return this->eof;
}


