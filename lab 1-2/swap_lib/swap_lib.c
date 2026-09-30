#include "../lib_transform.h"
#include "../libcharutil.h"

void transform_line(char *line){
    for (int i = 0; line[i] != '\0'; i++){
        if(is_lower(line[i]) == 1){
            line[i] = to_upper(line[i]); 
        }
        else
            line[i] = to_lower(line[i]);
    }
}