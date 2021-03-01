#include "util.hpp"

#include <CkString.h>
#include <iostream>
using namespace std;

std::string Util::htmlPreProcessing(std::string url){
    // std::regex rex("[\\\"\\“\\–\\—\\\\”\\'\\ \\‘\\’\\.\\,&\\!\\;\\=\\/\\-\\,\\?\\(\\)]");
    std::regex rex("[.\\,\\&\\!\\?\\(\\)\\\"\\'\\-\\*\\+\\_\\/]");
    std::string result;

    if(url.find("<body")== std::string::npos && url.find("<html")== std::string::npos){
        // cout << "Not HTML" << endl;
        cout << ". ";
        return "";
    } 

    searchAndDestroy(url, "<head>", "</head>");
    searchAndDestroy(url, "<script", "</script>");
    searchAndDestroy(url, "<style", "</style>");
    searchAndDestroy(url, "<", ">");


    clearUnicode(url);
    
    CkString aux;
    aux.setStringUtf8(url.c_str());
    aux.entityDecode();
    aux.toLowerCase(); 
    result = aux.getStringUtf8();

    std::string _r;
    std::regex_replace(std::back_inserter(_r), result.begin(), result.end(), rex, " ");


    return _r;
    // return result;
}

void Util::searchAndDestroy(std::string &str, std::string begin, std::string end){

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

void Util::replaceToken(std::string &str, std::string token, std::string newToken){

    while(true){
        int cursorBegin = str.find(token);
        if(!(cursorBegin == std::string::npos)){
            str.replace(cursorBegin,token.length(),newToken);
        }else{
            break;
        }
    }
}

bool Util::isSupportedChar(unsigned short c){
    // std::cout << std::hex << c << std::dec << " " << c ;
    if( 
        // (c >= 0xc380 && c <= 0xcaaf ) || 
        (c >= 0xc380 && c <= 0xc897 ) || 
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

void Util::clearUnicode(std::string &str){
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
                if(!isSupportedChar(ch)){
                    str.replace(i,cplen," ");
                    // cout << " not supported 2" << endl;
                }else{
                    // cout << " supported 2" << endl;
                }
                break;
            case 1:
                ch = ((short)str[i]) & (0x00ff);
                // std::cout << std::hex << ch << std::dec;
                if(!isSupportedChar(ch)){
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

