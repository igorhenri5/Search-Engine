#include "Crawler.hpp"

int Crawler::numGathered;
int Crawler::numThreadsGathering;
bool Crawler::done;

std::mutex Crawler::longTermScheduler_mutex;
std::mutex Crawler::numGathered_mutex;
std::mutex Crawler::numThreadsGathering_mutex;
std::mutex Crawler::visitedPages_mutex;

std::map<std::string, std::string> Crawler::visitedPages;

bool Crawler::checkVisitedPage(std::string url){
    std::string aux = Util::cleanUrlPrefix(url);
    if(Crawler::visitedPages.count(aux)>0)
        return true;
    return false;
}

bool Crawler::insertVisitedPage(std::string url, std::string filename){
    if(Crawler::checkVisitedPage(url))
        return false;
    Crawler::visitedPages.insert(std::pair<std::string,std::string>(Util::cleanUrlPrefix(url),filename));
    return true;
}

bool Crawler::editVisitedPage(std::string url, std::string filename){
    if(!Crawler::checkVisitedPage(url))
        return false;
    Crawler::visitedPages[Util::cleanUrlPrefix(url)] = filename;
    return true;
}

void Crawler::saveVisitedPagesStatus(){
    std::ofstream file;
    file.open("dump/"+(std::to_string(GATHER_LIMIT)+".out"));
    for(auto it = Crawler::visitedPages.cbegin(); it != Crawler::visitedPages.cend(); ++it){
        if(it->second != "!"){
        file << it->second << " " <<  it->first << std::endl;
        }
    }
    file.close();
}

Crawler::Crawler(int numThreads){
    this->numThreads = numThreads;
    Crawler::numGathered = 0;
    Crawler::numThreadsGathering = 0;
    Crawler::done = false;
}

void Crawler::start(){
    std::list<std::thread> running;
    auto t1 = std::chrono::high_resolution_clock::now();

    for(int i=0; i<numThreads; i++) 
        running.push_back(std::thread(&Crawler::crawl, this));

    for(std::list<std::thread>::iterator it=running.begin(); it!=running.end(); it++)
        it->join();

    auto t2 = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>( t2 - t1 ).count();

    std::cout << "\n-----------------------------------------------------" << std::endl;
    std::cout << "Time Elapsed: " << duration/1000 << "s" << std::endl;
    std::cout << Crawler::numGathered << " Pages Gathered" << std::endl;
    std::cout << "ATpP: " << ((duration/1000)/Crawler::numGathered) << " s" << std::endl;
    saveVisitedPagesStatus();
}

void Crawler::crawl(){
    
    std::stringstream ss;
    ss << std::this_thread::get_id();    
    Dumper dumper("p" + ss.str());

    std::string filename;

    CkSpider spider;
    CkString gatheredUrl, gatheredHtml;

    spider.put_Utf8(true);
    spider.put_MaxResponseSize(1000000); 
    spider.AddMustMatchPattern("*.br*");
    spider.AddMustMatchPattern("*br.*");

    //Thread Loop
    while(true){
        if(Crawler::done){
            break;
        }
        
        //Fetchs one Site from LongTermScheduler's PQ
        bool pqEmpty = false;
        Crawler::longTermScheduler_mutex.lock();            
            pqEmpty = LongTermScheduler::isEmpty();
            Site currSite = LongTermScheduler::getFirstSite();
            LongTermScheduler::popSite();
            if(!pqEmpty){
                Crawler::numThreadsGathering_mutex.lock();
                    Crawler::numThreadsGathering++;
                Crawler::numThreadsGathering_mutex.unlock();
            }
        Crawler::longTermScheduler_mutex.unlock();
        
        //if LongTermScheduler's PQ is empty, skip or end
        if(pqEmpty){
            bool isThereThreadGathering = false;
            Crawler::numThreadsGathering_mutex.lock();
                isThereThreadGathering = (Crawler::numThreadsGathering>0);
            Crawler::numThreadsGathering_mutex.unlock();

            if(!isThereThreadGathering){
                Crawler::done=true;
                break;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            continue;            
        }

        if(currSite.url == "" || currSite.size == 0){
            Crawler::numThreadsGathering_mutex.lock();
                Crawler::numThreadsGathering--;
            Crawler::numThreadsGathering_mutex.unlock();
            continue;
        }

        bool alreadyVisited = false;
        Crawler::visitedPages_mutex.lock();
            alreadyVisited = Crawler::insertVisitedPage(currSite.url, "!");
        Crawler::visitedPages_mutex.unlock();
        
        if(!alreadyVisited){
            std::cout << "\nSKIPPING " << currSite.url << "  - Already visited" << std::endl;
            Crawler::numThreadsGathering_mutex.lock();
                Crawler::numThreadsGathering--;
            Crawler::numThreadsGathering_mutex.unlock();
            continue;
        }
         
        spider.Initialize(spider.getUrlDomain(currSite.url.c_str()));
        // spider.put_Utf8(false);

        spider.AddUnspidered(currSite.url.c_str());
        
        bool success = spider.CrawlNext();
        if(!success){
            std::cout << "\n ! FAILED @ " << currSite.url << " \n" << std::endl;
            Crawler::numThreadsGathering_mutex.lock();
                Crawler::numThreadsGathering--;
            Crawler::numThreadsGathering_mutex.unlock();
            continue;
        }

        spider.get_LastUrl(gatheredUrl);
        spider.get_LastHtml(gatheredHtml);  

        Crawler::numGathered_mutex.lock();
            Crawler::numGathered+=1;
            if(numGathered >= GATHER_LIMIT)
                Crawler::done = true;
            std::cout << LongTermScheduler::queueSize() << "  " << numGathered << " Z " << gatheredUrl.getStringUtf8() << std::endl;
        Crawler::numGathered_mutex.unlock();
        
        int unspidered = spider.get_NumUnspidered();
        
        // filename = dumper.dumpPage(gatheredUrl.getStringUtf8(), gatheredHtml.getStringUtf8());
        filename = dumper.dumpPage(gatheredUrl.getStringUtf8(), spider.lastHtml());

        Crawler::visitedPages_mutex.lock();
            Crawler::editVisitedPage(currSite.url, filename);            
        Crawler::visitedPages_mutex.unlock();

        for(int x=0; x<spider.get_NumOutboundLinks(); x++){
            if(Crawler::done)
                break;

            if(!spider.GetOutboundLink(x, gatheredUrl)) 
                continue;
            std::string outUrl = gatheredUrl.getStringUtf8();
            CkString auxS = spider.getUrlDomain(outUrl.c_str());
            std::string outDomain = auxS.getStringUtf8();
            
            //!
            bool alreadyVisitedUrl;
            bool alreadyVisitedDomain;
            
            Crawler::visitedPages_mutex.lock();
                alreadyVisitedUrl = Crawler::checkVisitedPage(outUrl);
                alreadyVisitedDomain = Crawler::checkVisitedPage(outDomain);
            Crawler::visitedPages_mutex.unlock();

            Crawler::longTermScheduler_mutex.lock();
                if(!alreadyVisitedUrl)    LongTermScheduler::pushSite(Site(outUrl));
                if(!alreadyVisitedDomain) LongTermScheduler::pushSite(Site(outDomain));
            Crawler::longTermScheduler_mutex.unlock();
        }

        for(int w=0; w<unspidered; w++){
            if(Crawler::done)
                break;

            auto t1 = std::chrono::high_resolution_clock::now();
                spider.CrawlNext();
            auto t2 = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>( t2 - t1 ).count();

            spider.get_LastUrl(gatheredUrl);
            spider.get_LastHtml(gatheredHtml);
                            
            bool visited = false;

            Crawler::longTermScheduler_mutex.lock();
                visited = Crawler::checkVisitedPage(gatheredUrl.getStringUtf8());
            Crawler::longTermScheduler_mutex.unlock();
            if(visited){
                continue;
            }
            
            // filename = dumper.dumpPage(gatheredUrl.getStringUtf8(), gatheredHtml.getStringUtf8());
            filename = dumper.dumpPage(gatheredUrl.getStringUtf8(), spider.lastHtml());
            Crawler::visitedPages_mutex.lock();
                Crawler::insertVisitedPage(gatheredUrl.getStringUtf8(), filename);
            Crawler::visitedPages_mutex.unlock();

            int auxN; 
            Crawler::numGathered_mutex.lock();
                Crawler::numGathered+=1;
                auxN = Crawler::numGathered;
                if(numGathered>=GATHER_LIMIT)
                    Crawler::done = true;
            Crawler::numGathered_mutex.unlock();
            
            std::cout << LongTermScheduler::queueSize() << "  " << auxN << " I " << gatheredUrl.getStringUtf8() << std::endl;
            spider.SleepMs(100);
        }        

        spider.ClearSpideredUrls();
        spider.ClearOutboundLinks();
        spider.ClearFailedUrls();

        Crawler::numThreadsGathering_mutex.lock();
            Crawler::numThreadsGathering--;
        Crawler::numThreadsGathering_mutex.unlock();
    }
}
