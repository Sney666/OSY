#include <stdio.h>
#include <stdlib.h>
#include "genstr_lib.h"

void generate_line(int m){
    int stop = (rand() % m) + 1;

    for (int i = 0; i < stop; i++){
        int length = (rand() % 5) + 2;
        
        for (int j = 0; j < length; j++){
            int c;
            if(rand() % 2 == 0)
                c = (rand() % 26) + 'a';
            else 
                c = (rand() % 26) + 'A';
            printf("%c", c);
        }
        if (i < length - 1) {
            printf(" ");
        }
    }
    printf("\n");
}