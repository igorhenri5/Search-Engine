#include <iostream>
#include <cstdlib>
#include <chrono>

// Using the Chilkat Library, write a program in C++ that receives 2 parameters:
// - URL of a page to to crawl and
// - number n of additional links within this page to crawl next.

// Once executed, your program should crawl n+1 pages, starting with the URL provided as the first parameter, and display for each page crawled:
// 1.the URL crawled and
// 2.the title of the respective page as it appears in the Web

using namespace std;

#include <CkSpider.h>

void crawlUrl(char* first_url, int numPages){
    CkSpider spider;

    //  The spider object crawls a single web site at a time.
    spider.Initialize(first_url);

    //  Begin crawling the site by calling CrawlNext repeatedly.
    double meanDuration = 0;
    int i;
    for(i = 0; i <= numPages; i++){
        bool success;

        auto t1 = std::chrono::high_resolution_clock::now();
            success = spider.CrawlNext();
        auto t2 = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>( t2 - t1 ).count();
        meanDuration += duration;
        std::cout << duration << "ms - ";

        if (success == true) {
            //  Show the Title and URL of the page just spidered.
            std::cout << i+1 << " [" << spider.lastUrl() << "] - " << spider.lastHtmlTitle() << "\r\n";
            //  The HTML is available in the LastHtml property
        }
        else {
            //  Did we get an error or are there no more URLs to crawl?
            if (spider.get_NumUnspidered() == 0) {
                std::cout << "No more URLs to spider" << "\r\n";
                break;
            }
            else {
                std::cout << spider.lastErrorText() << "\r\n";
            }
        }
        //  Sleep 0.4 second before spidering the next URL.
        spider.SleepMs(400);
    }
    std::cout << "Average seconds per page: " << (meanDuration/i)/1000 << std::endl;
}

int main(int argc, char* argv[]){
    if(argc < 3)
        return 1;
        
    int nPages = atoi(argv[2]);

    crawlUrl(argv[1], nPages);
    return 0;
}