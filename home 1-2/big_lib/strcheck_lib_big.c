#include "../strcheck_lib.h"

int check_line(const char *line){
    int count = 0;
    for (int i=0; line[i] != '\0'; i++)
    {
        if(line[i] >= 'A' && line[i] <= 'Z')
            count++;
    }

    return count;
}