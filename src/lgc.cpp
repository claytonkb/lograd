// lgc.cpp
//

#include "lograd.h"
#include "lgc.h"
#include <string>

using namespace std;

//
//
string_vector* lgc_process2(stringstream *lgc_file){

    string buffer = lgc_file->str();
    string_vector* str_tokens = new string_vector;

    char* cbuffer = &buffer[0];

    char *pch;
    pch = strtok(cbuffer," \n");
    bool comment_mode=false;
    string *s;

    while(pch != NULL){

        if(comment_mode){
            if(streq(pch,"--"))
                comment_mode = false;
        }
        else{
            if(streq(pch,"--"))
                comment_mode = true;
            else{
                s = new string(pch);
                str_tokens->push_back(*s);
            }
        }

        pch = strtok(NULL," \n");

    }

    return str_tokens;

}




//
//
cstring_vector* lgc_process(stringstream *lgc_file){

    string buffer = lgc_file->str();
    cstring_vector* str_tokens = new vector<char*>;

//    cout << buffer;

//    const char *cbuffer = buffer.c_str;

//    const char* cbuffer = &buffer[0];
    char* cbuffer = &buffer[0];

//    printf("%s", cbuffer);

    char *pch;
//    int ctr=0;
    pch = strtok(cbuffer," \n");
    bool comment_mode=false;

//    while(pch != NULL && ctr < 50){
    while(pch != NULL){

//        ctr++;

        if(comment_mode){
            if(streq(pch,"--"))
                comment_mode = false;
        }
        else{
            if(streq(pch,"--"))
                comment_mode = true;
            else
                str_tokens->push_back(pch);
//                printf("%s\n",pch);
        }

        pch = strtok(NULL," \n");

    }

//    printf("%s\n", (*str_tokens)[0]);

    return str_tokens;

}

// lgc_decls()
// lgc_defns()
// lgc_loss()



// Clayton Bauman 2018

