#include <stdio.h>
#include <stdlib.h>
#include "strcheck_lib.h"

int main(){
    char buffer[1024];
    int count = 0;
    
    while (fgets(buffer, sizeof(buffer), stdin) != NULL){
        count += check_line(buffer);
    }
    printf("Total count: %d\n", count);
    
    return 0;
}