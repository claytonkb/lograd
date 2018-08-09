// lograd.h
//

#pragma once

#include "cutils.h"

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <iostream>
#include <typeinfo>
#include <fstream>
#include <istream>
#include <sstream>
#include <streambuf>
#include <string>
#include <vector>
#include <map>

using namespace std;

// deferred type-declarations
class cell;

#define streq(x,y)  ( strcmp(x,y) == 0 )
#define first_char_dot(x) ((x).find(".",0,1))
#define not_section_label(x) (((x) == string::npos) || ((x) != 0))

struct cstring_cmp {
    bool operator () (const char* a, const char* b) const {
        return strcmp(a,b)<0;
    } 
};

typedef vector<char*> cstring_vector;
typedef vector<string> string_vector;

typedef map<const char*, const void*, cstring_cmp> cstring_map;
typedef map<string, const void*> string_map;

enum cell_role {INPUT_ROLE, PROPAGATE_ROLE, OUTPUT_ROLE, LOSS_ROLE};
enum pin_init  { INIT_ONE, INIT_ZERO,   INIT_HALF,   INIT_NEG_ONE, INIT_HI, 
                 INIT_LO,  INIT_RAND01, INIT_RAND11, INIT_RAND,
                 INIT_LO1, INIT_HI1, INIT_RAND1,
                 INIT_LO2, INIT_HI2, INIT_RAND2};
enum direction {FORWARD, BACKWARD};
enum pin_state {OVERWRITE, ACCUMULATE};
enum cell_type {VAR_CELL, CONST_CELL, RAND_CELL, ACT_CELL, SOFT_MUX_CELL, LOSS_CELL};
enum pin_type  {INPUT_PIN, OUTPUT_PIN, LOSS_INPUT_PIN};

typedef float (*act_fn_ptr)(float);

class nada{
public:
    nada()=default;
    nada(int this_id);
    ~nada()=default;
    int id;
};


// Clayton Bauman 2018

