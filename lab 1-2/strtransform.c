#include <stdio.h>
#include <stdlib.h>
#include "lib_transform.h"

int main(){
    char buffer[1024];
    
    while (fgets(buffer, sizeof(buffer), stdin) != NULL){
        transform_line(buffer);
        printf("%s", buffer);
    }
    
    return 0;
}