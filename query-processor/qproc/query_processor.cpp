#include "query_processor.hpp" 

#define INNER_JOIN false

QueryProcessor::QueryProcessor( std::string indexFilePath, 
                                std::string vocabularyFilePath,
                                std::string urlsFilePath,
                                std::string normsFilePath){

    this->indexFilePath = indexFilePath;
    this->vocabularyFilePath = vocabularyFilePath;
    this->urlsFilePath = urlsFilePath;
    this->normsFilePath = normsFilePath;

    this->docsNormMap = new std::unordered_map<int, double>;
    this->vocabulary = new std::unordered_map<std::string, std::pair<double, unsigned int>>;
    this->urlMap = new std::unordered_map<int, std::string>;
    this->pageRank = new std::unordered_map<int, double>;
    
    load_urls();
    load_vocabulary();
    load_document_norms();
    load_page_rank();
}

QueryProcessor::~QueryProcessor(){
    delete this->vocabulary;
    delete this->urlMap;
    delete this->docsNormMap;
    delete this->pageRank;
}

std::ifstream& jump_lines(std::ifstream& file, unsigned int num){
    for(int i=0; i < num - 1; ++i){
        file.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
    }
    return file;
}

std::ifstream& go_to_line(std::ifstream& file, unsigned int num){
    file.seekg(std::ios::beg);
    jump_lines(file, num);
    return file;
}

void QueryProcessor::load_vocabulary(){
    std::cout << "LV" << std::endl; 
    this->vocabularyFile.open(vocabularyFilePath);
    if(!this->vocabularyFile.is_open()){
        std::cerr << "Could not open vocabulary file" << std::endl;
        exit(3);
    }

    std::string line;
    std::string term;
    double idf;
    unsigned int offset;
    while(getline(this->vocabularyFile,line)){
        std::stringstream* ss = new std::stringstream(line);
        *ss >> term >> idf >> offset;
        this->vocabulary->insert({term, {idf, offset}});
        delete ss;
    }
    this->vocabularyFile.close();
}

void QueryProcessor::load_urls(){
    std::cout << "LU" << std::endl; 
    this->urlsFile.open(urlsFilePath);
    if(!this->urlsFile.is_open()){
        std::cerr << "Could not open urls file" << std::endl;
        exit(4);
    }

    std::string url;
    int i = 1;
    while(getline(this->urlsFile,url)){
        this->urlMap->insert({i, url});
        i++;
    }
    this->urlsFile.close();
}

void QueryProcessor::load_document_norms(){   
    std::cout << "LDN" << std::endl; 
    this->normFile.open(normsFilePath);
    if(!this->normFile.is_open()){ 
       std::cerr << "Could not open norms file" << std::endl;
       exit(5); 
    }

    std::string line;
    int docId;
    double norm;
    while(getline(this->normFile, line)){
        std::stringstream* ss = new std::stringstream(line);
        *ss >> docId >> norm;
        if(!(this->docsNormMap->find(docId) == this->docsNormMap->end())){
            this->docsNormMap->at(docId) = norm;
        }else{
            this->docsNormMap->insert({docId, norm});
        }
        delete ss;
    } 

    this->normFile.close();
}

void QueryProcessor::load_page_rank(){
    std::cout << "LPR" << std::endl; 

    std::ifstream pageRankFile("./pagerank/pagerank.txt");
    if(!pageRankFile.is_open()){
       std::cerr << "Could not open pgrank file" << std::endl;
       exit(6); 
    }

    std::string line;
    int docId;
    double rank;
    while(getline(pageRankFile, line)){
        std::stringstream* ss = new std::stringstream(line);
        *ss >> docId >> rank;
        if(!(this->pageRank->find(docId) == this->pageRank->end())){
            this->pageRank->at(docId) = rank;
        }else{ 
            this->pageRank->insert({docId, rank});
        }
        delete ss;
    } 

    pageRankFile.close();
}

void QueryProcessor::pre_process_query(std::string query){
    Util::clear_unicode(query);
    Util::to_lower(query);

    std::stringstream ss(query);
    std::string buffer; 

    while(ss >> buffer){
        if(!(this->queryWeightMap->find(buffer) == this->queryWeightMap->end())){
            this->queryWeightMap->at(buffer) += 1.0;
        }else{
            this->queryWeightMap->insert({buffer, 1.0});
        }
    }
}

void QueryProcessor::to_string(){
    int i = 0;

    std::cout << "\n----------------DocsNM------------------" << std::endl; 
    for(auto it=this->docsNormMap->begin(); it!=this->docsNormMap->end(); ++it){
        std::cout << it->first << " :: " << it->second << '\n';
        if(i>=100) break; i++;
    }
    std::cout << "------------------------------------------" << std::endl; 

    i = 0;
    std::cout << "\n--------------VOCAB IDF----------------" << std::endl; 
    for(auto it=this->vocabulary->begin(); it!=this->vocabulary->end(); ++it){
        std::cout << it->first << " :: " << it->second.first << '\n';
        if(i>=100) break; i++;
    }
    std::cout << "------------------------------------------" << std::endl; 

    i = 0;
    std::cout << "\n--------------URLS----------------" << std::endl; 
    for(auto it=this->urlMap->begin(); it!=this->urlMap->end(); ++it){
        std::cout << it->first << " :: " << it->second << '\n';
        if(i>=100) break; i++;
    }
    std::cout << "------------------------------------------" << std::endl; 

    i = 0;
    std::cout << "\n---------------Page Rank----------------" << std::endl; 
    for(auto it=this->pageRank->begin(); it!=this->pageRank->end(); ++it){
        std::cout << it->first << " :: " << it->second << '\n';
        if(i>=100) break; i++;
    }
    std::cout << "------------------------------------------" << std::endl; 
}

bool QueryProcessor::compare_score_greater(const std::pair<int, double> &a, const std::pair<int, double> &b){ 
    return (a.second > b.second); 
}

bool QueryProcessor::compare_doc_id_smaller(const std::pair<int, double> &a, const std::pair<int, double> &b){  
    return (a.first < b.first); 
} 

void QueryProcessor::retrieve_k_first_results(std::string query, int k){ 
    this->queryWeightMap = new std::map< std::string, double>;
    pre_process_query(query);

    std::unordered_map<int, double>* scores = new std::unordered_map<int, double>; //document score map
    std::unordered_map<std::string, std::unordered_map<int, double>*>* tfCache = new std::unordered_map<std::string, std::unordered_map<int, double>*>;

    this->indexFile.open(indexFilePath);
    if(!this->indexFile.is_open()){
        std::cerr << "Could not open index file" << std::endl;
        exit(2); 
    }

    //INTERSECT
    auto t1 = std::chrono::high_resolution_clock::now();

    std::string line;
    std::string term, currTerm; 
    int qTermCount = 0;
    unsigned int docId, position, nextLine, hopSize;
    nextLine = 1;
    currTerm = "";
    
    for(auto qTerm=this->queryWeightMap->begin(); qTerm!=this->queryWeightMap->end(); ++qTerm){
        //1 termo = termo atual da query
        //2 se nao tiver no vocabulario, vai pra prox
        //3 pula n linhas (suficiente pra levar até vocabulary->at(term)->second) (vocabulary->at(term)->second - nextLine)
        //4 le até o termo nao ser mais igual ao último
        //repete

        //1
        term = qTerm->first;

        //2
        if(this->vocabulary->find(term) == this->vocabulary->end()){
            continue;
        }
        tfCache->insert({term, new std::unordered_map<int, double>()});

        //3
        hopSize = vocabulary->at(term).second - (nextLine-1);
        jump_lines(this->indexFile, hopSize);
        nextLine = vocabulary->at(term).second;

        //4
        if(qTerm == this->queryWeightMap->begin() || currTerm!=term){
            if(!getline(this->indexFile, line)){
                break;
            }
            nextLine++;
            std::stringstream* ss = new std::stringstream(line);
            *ss >> currTerm >> docId >> position;
            delete ss;
        }

        while(currTerm == term){
            if(scores->find(docId) == scores->end()){
                if(INNER_JOIN){
                    if(qTerm == this->queryWeightMap->begin()){
                        scores->insert({docId, 1.0});
                    }
                }else{
                    scores->insert({docId, 0.0});
                }
            }else{
                if(INNER_JOIN){
                    if(scores->at(docId) == (double)qTermCount){
                        scores->at(docId) += 1.0;
                    }
                }
            }
            if(tfCache->at(term)->find(docId) == tfCache->at(term)->end()){
                tfCache->at(term)->insert({docId, 1.0});
            }else{
                tfCache->at(term)->at(docId) += 1.0;
            }

            if(!getline(this->indexFile, line)){
                break;
            }
            nextLine++;
            std::stringstream* ss = new std::stringstream(line);
            *ss >> currTerm >> docId >> position;
            delete ss;
        }
        qTermCount++;
    }
    if(INNER_JOIN){
        for (auto doc=scores->begin(); doc!=scores->end(); ){
            if(doc->second != qTermCount){
                scores->erase(doc++);  
            }
            else{
                scores->at(doc->first) = 0.0;
                ++doc;
            }
        }
    }

    auto t2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = t2 - t1;
    std::cout << "intersect Elapsed Time: " << elapsed.count() << " milliseconds" << std::endl;
    this->indexFile.close(); 

    //CALCULATE SCORES
    t1 = std::chrono::high_resolution_clock::now();  

    int i = 1;
    int currDocId;
    double numerator, termIdf, docTermTfIdf, queryTermTfIdf, denominator, termWeight, aux;
    for(auto doc=scores->begin(); doc!=scores->end(); ++doc){
        currDocId = doc->first;

        // E wi,j x wi,q
        numerator = 0.0;
        for(auto qTerm=this->queryWeightMap->begin(); qTerm!=this->queryWeightMap->end(); ++qTerm){
            if(this->vocabulary->find(qTerm->first) == this->vocabulary->end())
                continue;

            termIdf = this->vocabulary->at(qTerm->first).first;
            
            if(!(tfCache->find(qTerm->first) == tfCache->end())){
                if(!(tfCache->at(qTerm->first)->find(currDocId) == tfCache->at(qTerm->first)->end())){
                    docTermTfIdf = (1.0 + log2(tfCache->at(qTerm->first)->at(currDocId))) * termIdf;
                }else{
                    docTermTfIdf = 0.0;
                }
            }

            queryTermTfIdf = (1.0 + log2(qTerm->second)) * termIdf;
            numerator +=  docTermTfIdf * queryTermTfIdf;
        }

        // E ||wj|| x ||wq||
        denominator = 0.0;
        denominator = this->docsNormMap->at(currDocId);

        aux = 0.0;
        for(auto entry=this->queryWeightMap->begin(); entry!=this->queryWeightMap->end(); ++entry){
            if(this->vocabulary->find(entry->first) == this->vocabulary->end())
                continue;
            termWeight = (1.0 + log2(entry->second)) * this->vocabulary->at(entry->first).first;
            aux += termWeight*termWeight;
        }
        aux = sqrt(aux);
        denominator = denominator * aux;
        
        if(!denominator == 0.0)
            doc->second = numerator/denominator; 

        i++;
    }
    t2 = std::chrono::high_resolution_clock::now();
    elapsed = t2 - t1;
    std::cout << "calc. score Elapsed Time: " << elapsed.count() << " milliseconds" << std::endl;

    //build vector and sort + incorporate pagerank
    std::vector<std::pair<int, double>> results;
    double score, alpha;
    alpha = 0.25;
    for(auto entry = scores->begin(); entry != scores->end(); ++entry){
        score = alpha * entry->second + (1.0-alpha) * this->pageRank->at(entry->first);
        // score = entry->second
        results.push_back(std::make_pair(entry->first, score));
    }
    sort(results.begin(), results.end(), compare_score_greater);

    //print k first results
    std::cout << "\n----------------SCORES-----------------" << std::endl; 
    for(auto it=results.begin(); it!=results.end() && k>0; ++it){
        std::cout << this->urlMap->at(it->first) << " :: " << it->second << " " << scores->at(it->first) << '\n';
        // std::cout << it->first << " :: " << it->second << '\n';
        k--;
    }
    std::cout << "------------------------------------------" << std::endl; 

    //clear query term weight map 
    for(auto it = tfCache->begin(); it != tfCache->end(); ++it){
        delete it->second;
    }
    delete tfCache;
    delete scores;
    delete this->queryWeightMap;
}