#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "genstr_lib.h"

int main(int argc, char *argv[]){

    if(argc < 3){
        fprintf(stderr, "Error: wrong input\n");
        return 1;
    }

    int n = atoi(argv[1]);
    int m = atoi(argv[2]);

    srand(time(NULL));
    for (int i = 0; i < n; i++) {
        generate_line(m);
    }

    return 0;
}