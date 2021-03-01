#ifndef __ENTR__
#define __ENTR__

#include <regex>
#include <vector>
#include <utility>
#include <unordered_map>

class Entry{
    public:
        Entry();
        ~Entry();
        void insertOccurrence(int, int);
        void to_string();
        unsigned long sizeBytes();
        int totalOccurences();
        int invertedListSize();
        std::unordered_map<int, std::vector<int>* >* umap;
    private:
        // static std::unordered_map<int, std::vector<int>> index;
};

#endif