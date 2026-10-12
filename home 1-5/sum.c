#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]){
    int sum = 0, cislo;

    while(scanf("%d", &cislo) == 1){
        sum += cislo;
    }

    printf("%d", sum);

    return 0;
}