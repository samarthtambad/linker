
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char delim[] = " \n\t";
char *ptr;
char *line = NULL;
size_t linecap = 0;
ssize_t linelen;
int line_num = 0;
int col_num = 0;

void tokenizer(char* input_file){
    FILE *fptr;
    fptr = fopen(input_file, "r");
    if(fptr == NULL){
        printf("cannot open file");
    }
    char *line = NULL;
    size_t linecap = 0;
    ssize_t linelen;
    int line_num = 0;
    int col_num = 0;
    char *ptr;
    char delim[] = " \n\t";
    
    while ((linelen = getline(&line, &linecap, fptr)) > 0){
        line_num++;
        ptr = strtok(line, delim);
        while(ptr != NULL){
            col_num = ptr - line + 1;
            printf("Token: %d:%d : %s \n", line_num, col_num, ptr);
            ptr = strtok(NULL, delim);
        }
        col_num = linelen;
    }
    printf("Final Spot in File : line=%d offset=%d\n", line_num, col_num);
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
    if(tkn == NULL){
        // printf("integer expected but not found");
        return -1;
    }
    int i = atoi(tkn);
    return i;
}

// read next token from the file, check if valid symbol and return
char* readSymbol(FILE *fptr){
    char* tkn = getToken(fptr);
    if(tkn == NULL){
        // printf("symbol expected but not found");
        return NULL;
    } 
    return tkn;
}

// read next token from the file, check if address mode integer and return
char* readIEAR(FILE *fptr){
    char* tkn = getToken(fptr);
    if(tkn == NULL){
        // printf("IEAR expected but not found");
        return NULL;
    }
    return tkn;
}

void pass1(char* input_file){
    FILE *fptr;
    fptr = fopen(input_file, "r");
    if(fptr == NULL){
        printf("cannot open file");
    }
    
    while(!feof(fptr)){
        // printf("%d", feof(fptr));
        // def list - defcount pairs of (S, R)
        int defcount = readInt(fptr);
        printf("%d", feof(fptr));
        // printf("[%ld]", linelen);
        printf("%d\n", defcount);
        for(int i = 0; i < defcount; i++) {
            char* symbol = readSymbol(fptr);
            int val = readInt(fptr);
            printf("(%s, %d)\n", symbol, val);
        }
        // use list - usecount symbols
        int usecount = readInt(fptr);
        printf("%d\n", usecount);
        for (int i = 0; i < usecount; i++) {
            char* symbol = readSymbol(fptr);
            printf("%s\n", symbol);
        }
        // program text - codecount pairs of (type, instr)
        int codecount = readInt(fptr);
        printf("%d\n", codecount);
        for (int i = 0; i < codecount; i++) {
            char* addressmode = readIEAR(fptr);
            int  operand = readInt(fptr);
            printf("(%s, %d)\n", addressmode, operand);
        }
    }
    // while(getToken(fptr) != NULL);
    // printf("%d", feof(fptr));
    fclose(fptr);
}

int main(int argc, char *argv[]){

    if(argc==1) printf("\nNo Extra Command Line Argument Passed Other Than Program Name"); 
    char *filename = argv[1];

    pass1(filename);

    // tokenizer(filename);
    // FILE *fptr;
    // fptr = fopen(filename, "r");
    // if(fptr == NULL){
    //     printf("cannot open file");
    // }
    
    // printf("%d", readInt(fptr));

    // while(getToken(fptr) != NULL);
    return 0;
}