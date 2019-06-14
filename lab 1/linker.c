
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
    if(ptr == NULL && feof(fptr) <= 0){ //line is completely scanned. load a new line.
        len = getline(&line, &linecap, fptr);
        if(len > 0){
            linelen = len;
            line_num++;
            ptr = strtok(line, delim);
        }
    }
    if(ptr != NULL){
        col_num = ptr - line + 1;
        printf("Token: %d:%d : %s \n", line_num, col_num, ptr);
    }else{
        col_num = linelen;
        printf("Final Spot in File : line=%d offset=%d\n", line_num, col_num);
    }
    return ptr;
}

int readInt(){
    return 0;
}

int main(int argc, char *argv[]){

    if(argc==1) printf("\nNo Extra Command Line Argument Passed Other Than Program Name"); 
    char *filename = argv[1];

    // tokenizer(filename);
    FILE *fptr;
    fptr = fopen(filename, "r");
    if(fptr == NULL){
        printf("cannot open file");
    }
    
    while(getToken(fptr) != NULL);
    return 0;
}