#ifndef __CHNK__
#define __CHNK__

#include <vector>
#include <string>
#include <tuple>
#include <fstream>

class Chunk{
    public:
        Chunk(int maxSize);
        ~Chunk();
        bool setSource(std::string filename);
        void fill();
        std::tuple<std::string, int, int> getFirst();
        std::string firstIntoString();
        void removeFirst();
        bool isEmpty();
        bool gotEof();
        void to_string();
    private:
        std::vector<std::tuple<std::string, int, int>>* occurrences;
        bool eof;
        int maxSize;
        std::string filename; 
        std::ifstream sourceFile;
};

#endif