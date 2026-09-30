#include "libcharutil.h"

int is_lower(char c){
    if(c >= 'a' && c <= 'z') 
        return 1;
    return 0;
}
int is_upper(char c){
    if(c >= 'A' && c <= 'Z')
        return 1;
    return 0;
}
char to_lower(char c){
    if(c >= 'A' && c <= 'Z')
        return c + 32;
    return c;
}
char to_upper(char c){
    if(c >= 'a' && c <= 'z') 
        return c - 32;
    return c;
}