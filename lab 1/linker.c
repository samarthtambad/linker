
#include <stdio.h>
#include <string.h>

char buff[1024];

void getToken(char* input_file){
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
    int len = 0;
    char *ptr;
    char delim[] = " \n\t";
    while ((linelen = getline(&line, &linecap, fptr)) > 0){
        line_num++;
        len = strlen(line);
        ptr = strtok(line, delim);
        while(ptr != NULL){
            col_num = ptr - line + 1;
            printf("Token: %d:%d : %s \n", line_num, col_num, ptr);
            ptr = strtok(NULL, delim);
        }
        col_num = len;
    }
    printf("Final Spot in File : line=%d offset=%d\n", line_num, col_num);
}

int main(int argc, char *argv[]){

    if(argc==1) printf("\nNo Extra Command Line Argument Passed Other Than Program Name"); 
    char *filename = argv[1];

    getToken(filename);

    return 0;
}