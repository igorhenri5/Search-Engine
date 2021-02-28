#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <chrono>
#include <cstdlib>

#include <CkSpider.h>

#include "crawler/Crawler.hpp"
#include "util/util.hpp"
#include "site/Site.hpp"
#include "sched/LongTermScheduler.hpp"

using namespace std;

int main(int argc, char* argv[]){
    if(argc < 2)
        return 1;

    ifstream myfile(argv[1]);
    string line;
    if(myfile.is_open()){
        while(getline(myfile,line)){
            LongTermScheduler::pushSite(Site(line));
        }
        myfile.close();
    }
    else 
        cout << "Unable to open file"; 

    Crawler* crawler = new Crawler(THREAD_NUM);
    crawler->start();
        
    return 0;
}