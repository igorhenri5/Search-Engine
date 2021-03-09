#include "util.hpp"

#include <CkString.h>
#include <iostream>
#include <regex>

using namespace std;

std::string Util::html_pre_processing(std::string content){
    std::regex rex("[.\\$\\#\\%\\,\\&\\!\\?\\(\\)\\\"\\'\\-\\*\\+\\_\\/]");
    std::string result;

    if(content.find("<body")== std::string::npos && content.find("<html")== std::string::npos){
        return "";
    } 

    search_and_destroy(content, "<head>", "</head>");
    search_and_destroy(content, "<script", "</script>");
    search_and_destroy(content, "<style", "</style>");
    search_and_destroy(content, "<", ">");

    clear_unicode(content);
    to_lower(content);
    
    std::string _r;
    std::regex_replace(std::back_inserter(_r), content.begin(), content.end(), rex, " ");


    return _r;
}

void Util::to_lower(std::string& str){
    CkString aux;
    aux.setStringUtf8(str.c_str());
    aux.entityDecode();
    aux.toLowerCase(); 
    str = aux.getStringUtf8();
}

void Util::regex_special_token_clear(std::string& str){
    std::string _r;
    std::regex rex("[.\\$\\#\\%\\,\\&\\!\\?\\(\\)\\\"\\'\\-\\*\\+\\_\\/]");
    std::regex_replace(std::back_inserter(_r), str.begin(), str.end(), rex, " ");
    str = _r;
}

void Util::search_and_destroy(std::string &str, std::string begin, std::string end){

    int cursorBegin, cursorEnd;
    while(true){
        cursorBegin = str.find(begin);
        if(cursorBegin == std::string::npos) break;
        cursorEnd = str.find(end);   
        if(cursorEnd == std::string::npos){
            str.replace(cursorBegin,str.length()-cursorBegin," ");
        }else{
            str.replace(cursorBegin,cursorEnd-cursorBegin+end.length()," ");
        }
    }
}

bool Util::is_supported_char(unsigned short c){
    // std::cout << std::hex << c << std::dec << " " << c ;
    if( 
        // (c >= 0xc380 && c <= 0xcaaf ) || 
        // (c >= 0xc380 && c <= 0xc897 ) || 
        (c >= 0xc380 && c <= 0xc3bc ) || 
        (c == 0x0020 ) || 
        // (c == 0x0020 || c==0x0024) || 
        (c >= 0x0030 && c <= 0x0039) || 
        (c >= 0x0041 && c <= 0x005A) || 
        (c >= 0x0061 && c <= 0x007A)
        ){
        return true;
    }
    // std::cout << "UNS " << c << std::endl;
    return false;
}

void Util::clear_unicode(std::string &str){
    for(size_t i = 0; i < str.length();){
        int cplen = 1;
        if((str[i] & 0xf8) == 0xf0) cplen = 4;
        else if((str[i] & 0xf0) == 0xe0) cplen = 3;
        else if((str[i] & 0xe0) == 0xc0) cplen = 2;
        if((i + cplen) > str.length()) cplen = 1;
        
        // cout << str.substr(i, cplen) << " ";
        unsigned short ch;
        switch(cplen){
            case 2:
                ch = (((short)str[i]) << 8) | (0x00ff & str[i+1]);
                // std::cout << std::hex << ch << std::dec << " ";
                if(!is_supported_char(ch)){
                    str.replace(i,cplen," ");
                    // cout << " not supported 2" << endl;
                }else{
                    // cout << " supported 2" << endl;
                }
                break;
            case 1:
                ch = ((short)str[i]) & (0x00ff);
                // std::cout << std::hex << ch << std::dec;
                if(!is_supported_char(ch)){
                    // cout << " not supported 1" << endl;
                    str.replace(i,cplen,"  ");
                }else{
                    // cout << " supported 1 " << endl;
                }
                break;
            default:
                // cout << " not supported 3+" << endl;
                str.replace(i,cplen,"    ");
                break;
        }
        i += cplen;
    }
}

