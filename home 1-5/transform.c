#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

int main(int argc, char* argv[]){
    if(argc < 2){
        fprintf(stderr, "Error: missing argument");
        return 1;
    }

    char* transform_mode = argv[1];
    int cislo;

    while(scanf("%d", &cislo) == 1){
        if(strcmp(transform_mode, "square") == 0)
            printf("%d\n", (int)pow(cislo, 2));
        else if(strcmp(transform_mode, "sqrt") == 0)
            printf("%d\n", (int)sqrt(cislo));
        else
            printf("%d\n", cislo * -1);
    }

    return 0;
}