#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[]){
    if(argc < 2){
        fprintf(stderr, "Error: missing argument");
        return 1;
    }

    char* filter_mode = argv[1];
    int cislo;

    while(scanf("%d", &cislo) == 1){
        if(strcmp(filter_mode, "even") == 0){
            if(cislo % 2 == 0)
                printf("%d\n", cislo);
        }
        else{
            if(cislo % 2 != 0)
                printf("%d\n", cislo);
        }
    }

    return 0;
}