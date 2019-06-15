#include <stdlib.h>
#include <stdio.h>
#include <string.h>
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

struct Module{
    int base_address;
    int defcount;
    int usecount;
    int codecount;
};

struct Symbol{
    string txt;
    int position;
};

vector<Module> modules;
map<string, Symbol> symbol_table;

void createSymbol(string txt, int val){
    Symbol symbol;
    symbol.txt = txt;
    symbol.position = val;
    symbol_table.insert(pair<string, Symbol>(txt, symbol));
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
        printf("integer expected but not found");
        return -1;
    }
    if(tkn == NULL && feof(fptr)){
        return -1;
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
    return tkn;
}

// read next token from the file, check if address mode integer and return
string readIEAR(FILE *fptr){
    string tkn = getToken(fptr);
    if(tkn.size() <= 0){
        printf("IEAR expected but not found");
        return NULL;
    }
    if(tkn.size() > 1) {
        printf("wrong. length > 1");
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

    while(!feof(fptr)){
        Module module;
        
        // def list - defcount pairs of (S, R)
        int defcount = readInt(fptr);
        if(defcount >= 0){
            module.defcount = defcount;
            // printf("%d\n", defcount);
            for(int i = 0; i < defcount; i++) {
                string symbol = readSymbol(fptr);
                int val = readInt(fptr);
                createSymbol(symbol, val + offset);
                // printf("(%s, %d)\n", symbol.c_str(), val);
            }
        }
        
        // use list - usecount symbols
        int usecount = readInt(fptr);
        if(usecount >= 0){
            module.usecount = usecount;
            // printf("%d\n", usecount);
            for (int i = 0; i < usecount; i++) {
                string symbol = readSymbol(fptr);
                // printf("%s\n", symbol.c_str());
            }
        }
        
        // program text - codecount pairs of (type, instr)
        int codecount = readInt(fptr);
        if(codecount >= 0){
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
    }

    // iterate/print symbol table
    cout << "Symbol Table" << endl;
    for(map<string, Symbol>::iterator i = symbol_table.begin(); i != symbol_table.end(); ++i){
        cout << (*i).first << "=" << (*i).second.position << endl;
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
                    // doesn't change (but check for it <= 512)
                    printf("%0.3d: %d\n", count, instr);
                }
                else if(!strcmp(addressmode.c_str(), "E")){
                    string sym = uselist[operand];
                    instr = (opcode * 1000) +  (symbol_table[sym].position);
                    printf("%0.3d: %d\n", count, instr);
                }
                 else if(!strcmp(addressmode.c_str(), "R")){
                    instr = (opcode * 1000) +  (operand + modules[module_num].base_address);
                    printf("%0.3d: %d\n", count, instr);
                }
                count++;
            }
            module_num++;
        }
    }
    fclose(fptr);
}

int main(int argc, char *argv[]){

    if(argc==1) printf("\nNo Extra Command Line Argument Passed Other Than Program Name"); 
    char *filename = argv[1];

    pass1(filename);
    pass2(filename);

    return 0;
}