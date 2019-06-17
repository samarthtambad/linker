// requires C++11 or greater

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <regex>
#include <iostream>
#include <vector>
#include <map>

using namespace std;

char delim[] = " \n\t";
char *line = NULL;
size_t linecap = 0;
ssize_t linelen;
int line_num = 0;
int col_num = 0;

void parseerror(int errcode){
    static string errstr[] = {
        "NUM_EXPECTED", // [0] Number expect
        "SYM_EXPECTED", // [1] Symbol Expected
        "ADDR_EXPECTED", // [2] Addressing Expectedwhich is A/E/I/R  
        "SYM_TOO_LONG", // [3] Symbol Name is toolong
        "TOO_MANY_DEF_IN_MODULE", // [4] > 16 
        "TOO_MANY_USE_IN_MODULE", // [5] > 16
        "TOO_MANY_INSTR" // [6] total num_instr exceeds memory size (512)  
    };
    printf("Parse Error line %d offset %d: %s\n",line_num, col_num, errstr[errcode].c_str());
}

void printError(int errcode){
    static string errstr[] = {
        "Error: Absolute address exceeds machine size; zero used", // [0]
        "Error: Relative address exceeds module size; zero used", // [1]
        "Error: External address exceeds length of uselist; treated as immediate", // [2]  
        "Error: This variable is multiple times defined; first value used", // [3] 
        "TOO_MANY_DEF_IN_MODULE", // [4] > 16 
        "TOO_MANY_USE_IN_MODULE", // [5] > 16
        "TOO_MANY_INSTR" // [6] total num_instr exceeds memory size (512)  
    };
    printf("Parse Error line %d offset %d: %s\n",line_num, col_num, errstr[errcode].c_str());
}


struct Module{
    int base_address;
    int defcount;
    int usecount;
    int codecount;
};

struct Symbol{
    string txt;
    int position;
    bool is_mult_def;
    bool is_used;
    int module_num;
};

vector<Module> modules;
map<string, Symbol> symbol_table;

void createSymbol(string txt, int val, int module_num){
    if(symbol_table.find(txt) == symbol_table.end()){ // not already defined
        Symbol symbol;
        symbol.txt = txt;
        symbol.position = val;
        symbol.module_num = module_num;
        symbol.is_mult_def = false;
        symbol.is_used = false;
        symbol_table.insert(pair<string, Symbol>(txt, symbol));
    } else { // symbol already defined
        symbol_table[txt].is_mult_def = true;
    }
}

bool isNumber(string str){
    for(int i = 0; i < str.length(); i++){
        if(!isdigit(str[i])) return false;
    }
    return true;
}

bool isSymbol(string str){
    return regex_match(str, regex("[a-z|A-Z][a-z|A-Z|0-9]*"));
}

bool isAddress(string str){
    return (str == "A" | str == "E" | str == "I" | str == "R");
}

// read next token from the file
char* getToken(FILE *fptr){
    char *ptr;
    ssize_t len;
    ptr = strtok(NULL, delim);
    if(ptr == NULL && !feof(fptr)){ //line is completely scanned. load a new line.
        len = getline(&line, &linecap, fptr);
        if(len > 0){
            linelen = len;
            line_num++;
            ptr = strtok(line, delim);
        }
    }
    if(ptr != NULL){
        col_num = ptr - line + 1;
        // printf("Token: %d:%d : %s \n", line_num, col_num, ptr);
    }else{
        col_num = linelen;
        // printf("Final Spot in File : line=%d offset=%d\n", line_num, col_num);
    }
    return ptr;
}

// read next token from the file, check if valid integer and return
int readInt(FILE *fptr){
    char* tkn = getToken(fptr);
    if(tkn == NULL && !feof(fptr)){
        // parseerror(0);
        // exit(0);
        return -1;
    }
    if(tkn == NULL && feof(fptr)){
        return -1;
    }
    if(!isNumber(tkn)) {
        parseerror(0);
        exit(0);
    }
    int i = atoi(tkn);
    return i;
}

// read next token from the file, check if valid number (address) and return
int readNumber(FILE *fptr){
    char* tkn = getToken(fptr);
    if(tkn == NULL){
        printf("number expected but not found");
        return -1;
    }
    if(!isNumber(tkn)) {
        parseerror(0);
        exit(0);
    }
    int i = atoi(tkn);
    return i;
}

// read next token from the file, check if valid symbol and return
string readSymbol(FILE *fptr){
    string tkn = getToken(fptr);
    if(tkn.size() <= 0){
        printf("symbol expected but not found");
        return NULL;
    }
    if(!isSymbol(tkn)){
        parseerror(1);
        exit(0);
    } else if(tkn.size() > 16){
        parseerror(3);
        exit(0);
    }
    return tkn;
}

// read next token from the file, check if address mode integer and return
string readIEAR(FILE *fptr){
    string tkn = getToken(fptr);
    if(tkn.size() <= 0){
        printf("IEAR expected but not found");
        return NULL;
    }
    if(!isAddress(tkn)) {
        parseerror(2);
        exit(0);
    }
    return tkn;
}

void pass1(char* input_file){
    FILE *fptr;
    fptr = fopen(input_file, "r");
    if(fptr == NULL){
        printf("cannot open file");
    }
    
    int offset = 0;
    int total_code = 0;
    int module_num = 0;

    while(!feof(fptr)){
        Module module;
        module_num++;
        
        // def list - defcount pairs of (S, R)
        int defcount = readInt(fptr);
        if(defcount >= 0 && defcount <= 16){
            module.defcount = defcount;
            // printf("%d\n", defcount);
            for(int i = 0; i < defcount; i++) {
                string symbol = readSymbol(fptr);
                int val = readInt(fptr);
                createSymbol(symbol, val + offset, module_num);
                // printf("(%s, %d)\n", symbol.c_str(), val);
            }
        } else if(defcount > 16){
            parseerror(4);
            exit(0);
        }
        
        // use list - usecount symbols
        int usecount = readInt(fptr);
        if(usecount >= 0 && usecount <=16){
            module.usecount = usecount;
            // printf("%d\n", usecount);
            for (int i = 0; i < usecount; i++) {
                string symbol = readSymbol(fptr);
                // printf("%s\n", symbol.c_str());
            }
        } else if(usecount > 16){
            parseerror(5);
            exit(0);
        }
        
        // program text - codecount pairs of (type, instr)
        int codecount = readInt(fptr);
        // assert(codecount >= 0);
        total_code += codecount;
        if(codecount >= 0 && total_code <= 512){
            module.codecount = codecount;
            module.base_address = offset;
            // printf("%d\n", codecount);
            for (int i = 0; i < codecount; i++) {
                string addressmode = readIEAR(fptr);
                int instr = readNumber(fptr);
                // printf("(%s, %d)\n", addressmode.c_str(), instr);
            }
            offset += codecount;
            modules.push_back(module);
        }
        else if(total_code > 512){
            parseerror(6);
            exit(0);
        }
    }

    // iterate and print symbol table
    // cout << symbol_table.size() << endl;
    printf("Symbol Table\n");
    for(map<string, Symbol>::iterator i = symbol_table.begin(); i != symbol_table.end(); ++i){
        if(!(*i).second.is_mult_def){
            printf("%s=%d\n", (*i).first.c_str(), (*i).second.position);
        } else {
            printf("%s=%d Error: This variable is multiple times defined; first value used\n", (*i).first.c_str(), (*i).second.position);
        }
    }

    // printf("Final Spot in File : line=%d offset=%d\n", line_num, col_num);
    fclose(fptr);
}

void pass2(char* input_file){
    FILE *fptr;
    fptr = fopen(input_file, "r");
    if(fptr == NULL){
        printf("cannot open file");
    }

    int count = 0;
    int module_num = 0;

    printf("\nMemory Map\n");

    while(!feof(fptr)){

        vector<string> uselist;
        
        // def list - defcount pairs of (S, R)
        int defcount = readInt(fptr);
        if(defcount >= 0){
            // printf("%d\n", defcount);
            for(int i = 0; i < defcount; i++) {
                string symbol = readSymbol(fptr);
                int val = readInt(fptr);
                // printf("(%s, %d)\n", symbol.c_str(), val);
            }
        }
        
        // use list - usecount symbols
        int usecount = readInt(fptr);
        if(usecount >= 0){
            // printf("%d\n", usecount);
            for (int i = 0; i < usecount; i++) {
                string symbol = readSymbol(fptr);
                uselist.push_back(symbol);
                symbol_table.at(symbol).is_used = true;
                // printf("%s\n", symbol.c_str());
            }
        }
        
        // program text - codecount pairs of (type, instr)
        int codecount = readInt(fptr);
        if(codecount >= 0){
            // printf("%d\n", codecount);
            for (int i = 0; i < codecount; i++) {
                string addressmode = readIEAR(fptr);
                int  instr = readNumber(fptr);
                int opcode = instr / 1000;
                int operand = instr % 1000;
                // printf("(%s, %d)\n", addressmode.c_str(), instr);
                // make relevant modifications to instr
                if(!strcmp(addressmode.c_str(), "I")){
                    printf("%0.3d: %d\n", count, instr);
                } 
                else if(!strcmp(addressmode.c_str(), "A")){
                    // doesn't change (but check for <= 512 otherwise error)
                    if(operand <= 512){
                        printf("%0.3d: %d\n", count, instr);
                    } else {
                        operand = 0;
                        instr = (opcode * 1000) + operand;
                        printf("%0.3d: %d Error: Absolute address exceeds machine size; zero used\n", count, instr);
                    } 
                }
                else if(!strcmp(addressmode.c_str(), "E")){
                    string sym = uselist[operand];
                    Symbol s = symbol_table.at(sym);
                    instr = (opcode * 1000) +  (s.position);
                    printf("%0.3d: %d\n", count, instr);
                }
                 else if(!strcmp(addressmode.c_str(), "R")){
                    if(operand <= 512){
                        instr = (opcode * 1000) +  (operand + modules[module_num].base_address);
                        printf("%0.3d: %d\n", count, instr);
                    } else {
                        operand = 0;
                        instr = (opcode * 1000) +  (operand + modules[module_num].base_address);
                        printf("%0.3d: %d Error: Relative address exceeds module size; zero used\n", count, instr);
                    }
                }
                count++;
            }
            module_num++;
        }
    }

    // warning [4]
    // cout << symbol_table.size() << endl;
    for(map<string, Symbol>::iterator i = symbol_table.begin(); i != symbol_table.end(); ++i){
        if(!(*i).second.is_used) printf("Warning: Module %d: %s was defined but never used\n", (*i).second.module_num, (*i).second.txt.c_str());
    }

    fclose(fptr);
}

int main(int argc, char *argv[]){

    if(argc==1) printf("\nNo Extra Command Line Argument Passed Other Than Program Name"); 
    char *filename = argv[1];

    pass1(filename);
    pass2(filename);

    // FILE *fptr;
    // fptr = fopen(filename, "r");
    // if(fptr == NULL){
    //     printf("cannot open file");
    // }
    // while(getToken(fptr) != NULL);

    // cout << isSymbol("X21");

    return 0;
}