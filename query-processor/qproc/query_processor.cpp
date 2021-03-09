#include "query_processor.hpp" 

#define INNER_JOIN false

QueryProcessor::QueryProcessor( std::string indexFilePath, 
                                std::string vocabularyFilePath,
                                std::string urlsFilePath){

    this->indexFilePath = indexFilePath;
    this->vocabularyFilePath = vocabularyFilePath;
    this->urlsFilePath = urlsFilePath;

    this->docsWeightMap = new std::unordered_map<int, std::unordered_map<std::string, double>*>;
    this->vocabulary = new std::unordered_map<std::string, std::pair<double, unsigned int>>;
    this->urlMap = new std::unordered_map<int, std::string>;

    load_urls();
    load_vocabulary();
    read_index_fetch_weight();
}

QueryProcessor::~QueryProcessor(){
    delete this->vocabulary;
    delete this->urlMap;
    for(auto it=this->docsWeightMap->begin(); it!=this->docsWeightMap->end(); ++it){
        delete it->second;
    }
    delete this->docsWeightMap;
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

void QueryProcessor::read_index_fetch_weight(){   
    std::cout << "RIFW" << std::endl; 
    this->indexFile.open(indexFilePath);
    if(!this->indexFile.is_open()){
       std::cerr << "Could not open index file" << std::endl;
       exit(2); 
    }

    std::string line;
    std::string term;
    unsigned int docId, position;
    while(getline(this->indexFile, line)){
        std::stringstream* ss = new std::stringstream(line);
        *ss >> term >> docId >> position;
        if(!(this->docsWeightMap->find(docId) == this->docsWeightMap->end())){
            if(!(this->docsWeightMap->at(docId)->find(term) == this->docsWeightMap->at(docId)->end())){
                this->docsWeightMap->at(docId)->at(term) += 1.0;
            }else{
                this->docsWeightMap->at(docId)->insert({term, 1.0});
            }
        }else{
            this->docsWeightMap->insert({docId, new std::unordered_map<std::string, double>});
            this->docsWeightMap->at(docId)->insert({term, 1.0});
        }
        delete ss;
    } 

    this->indexFile.close();
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

    // std::cout << "\n----------------QueryWM------------------" << std::endl; 
    // for(auto it=this->queryWeightMap->begin(); it!=this->queryWeightMap->end(); ++it){
    //     std::cout << it->first << " :: " << it->second << '\n';
    //     if(i>=100) break; i++;
    // }
    // std::cout << "------------------------------------------" << std::endl; 
    
    i = 0;
    std::cout << "\n----------------DocsWM------------------" << std::endl; 
    for(auto it=this->docsWeightMap->begin(); it!=this->docsWeightMap->end(); ++it){
        for(auto it2 = it->second->begin(); it2 != it->second->end(); ++it2){
            std::cout << it->first << " -> " << it2->first << " :: " << it2->second << '\n';
        }
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

        auto t1 = std::chrono::high_resolution_clock::now();
    std::unordered_map<int, double>* scores = new std::unordered_map<int, double>; //document score map

    /*
        para a lista invertida de cada termo
            insert todos os documentos se ainda nao existir
    */
        this->indexFile.open(indexFilePath);
        if(!this->indexFile.is_open()){
            std::cerr << "Could not open index file" << std::endl;
            exit(2); 
        }

        std::string line;
        std::string term, currTerm; 
        int qTermCount = 0;
        unsigned int docId, position, currLine, hopSize;
        currLine = 0;
        currTerm = "";
        
        for(auto qTerm=this->queryWeightMap->begin(); qTerm!=this->queryWeightMap->end(); ++qTerm){
            //1 termo = termo atual da query
            //2 se nao tiver no vocabulario, vai pra prox
            //3 pula n linhas (suficiente pra levar até vocabulary->at(term)->second) (vocabulary->at(term)->second - currLine)
            //4 le até o termo nao ser mais igual ao último
            //repete

            //1
            term = qTerm->first;

            //2
            if(this->vocabulary->find(term) == this->vocabulary->end()){
                // visitedQueryTerm.insert({qTerm->first, -1});
                continue;
            }
            //3
            hopSize = vocabulary->at(term).second - currLine;
            jump_lines(this->indexFile, hopSize);

            //4
            if(qTerm == this->queryWeightMap->begin() && currTerm!=term){
                if(!getline(this->indexFile, line)){
                    break;
                }
                currLine++;
            }
            std::stringstream* ss = new std::stringstream(line);
            *ss >> currTerm >> docId >> position;
            delete ss;
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
                if(!getline(this->indexFile, line)){
                    break;
                }
                currLine++;
                std::stringstream* ss = new std::stringstream(line);
                *ss >> currTerm >> docId >> position;
                delete ss;
            }
            qTermCount++;
        }
        this->indexFile.close(); 


        if(INNER_JOIN){
            // std::cout << "QTC " << qTermCount << std::endl;
            for (auto doc=scores->begin(); doc!=scores->end(); ){
                if(doc->second != qTermCount){
                    scores->erase(doc++);  
                }
                else{
                    // std::cout << " i-dc " << doc->first << std::endl;
                    scores->at(doc->first) = 0.0;
                    ++doc;
                }
            }
        }

        auto t2 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = t2 - t1;
        std::cout << "intersect Elapsed Time: " << elapsed.count() << " milliseconds" << std::endl;

    //calculate scores
        t1 = std::chrono::high_resolution_clock::now();        

    int i = 1;
    int currdocId;
    double numerator, termIdf, docTermTfIdf, queryTermTfIdf, denominator, termWeight, aux;
    for(auto doc=scores->begin(); doc!=scores->end(); ++doc){
        currdocId = doc->first;

        numerator = 0.0;
        for(auto qTerm=this->queryWeightMap->begin(); qTerm!=this->queryWeightMap->end(); ++qTerm){
            if(this->vocabulary->find(qTerm->first) == this->vocabulary->end())
                continue;

            termIdf = this->vocabulary->at(qTerm->first).first;

            if(!(this->docsWeightMap->at(currdocId)->find(qTerm->first) == this->docsWeightMap->at(currdocId)->end())){
                docTermTfIdf = (1.0 + log2(this->docsWeightMap->at(currdocId)->at(qTerm->first))) * termIdf;
            }else{
                docTermTfIdf = 0.0;
            }
            queryTermTfIdf = (1.0 + log2(qTerm->second)) * termIdf;
            numerator +=  docTermTfIdf * queryTermTfIdf;
        }

        denominator = 0.0;
        for(auto entry=this->docsWeightMap->at(currdocId)->begin(); entry!=this->docsWeightMap->at(currdocId)->end(); ++entry){
            if(this->vocabulary->find(entry->first) == this->vocabulary->end())
                continue;
            termWeight = (1.0 + log2(entry->second)) * this->vocabulary->at(entry->first).first;
            denominator += termWeight*termWeight;
        }
        denominator = sqrt(denominator);

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

    //build vector and sort
    std::vector<std::pair<int, double>> results;
    for(auto entry = scores->begin(); entry != scores->end(); ++entry){
        results.push_back(std::make_pair(entry->first, entry->second));
    }
    sort(results.begin(), results.end(), compare_score_greater);


    //print k first results
    std::cout << "\n----------------SCORES-----------------" << std::endl; 
    for(auto it=results.begin(); it!=results.end() && k>0; ++it){
        std::cout << this->urlMap->at(it->first) << " :: " << it->second << '\n';
        // std::cout << it->first << " :: " << it->second << '\n';
        k--;
    }
    std::cout << "------------------------------------------" << std::endl; 

    //clear query term weight map 
    delete scores;
    delete this->queryWeightMap;
}