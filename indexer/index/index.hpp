#ifndef __INDX__
#define __INDX__

#include <regex>
#include <vector>
#include <utility>
#include <unordered_map>

#include "../util/util.hpp"
#include "entry.hpp"

// #define SUB_INDEX_SIZE 5000
// #define SORT_CHUNK_SIZE 500

#define SUB_INDEX_SIZE 5000000
#define SORT_CHUNK_SIZE 2000

// #define SUB_INDEX_SIZE 10000000
// #define SORT_CHUNK_SIZE 1000

class InvertedIndex{
    public:
        InvertedIndex();
        ~InvertedIndex();
        void insert(std::string, int, int);
        void to_string();
        unsigned long sizeBytes();
        int vocabularySize();
        void occurrenceToCSV(std::string);
        double avgInvertedListSize();
    private:
        std::unordered_map<std::string, Entry*>* index;
        std::vector<std::string>* subIndexList;
        int numInsertions;
        void exportIndex();
        void clearIndex();
        void mergeSubIndexes();
};

#endif