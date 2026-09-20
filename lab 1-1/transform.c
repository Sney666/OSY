#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]){
    int is_add = (strstr(argv[0], "add") != NULL);
    int is_inv = (strstr(argv[0], "inv") != NULL);
    if (!is_add && !is_inv){
        fprintf(stderr, "Error: invalid program name\n");
        return 1;
    }

    int cap = 100, len = 0, has_char = 0, has_num = 0, input;
    char *buffer = malloc(cap);

    while ((input = getchar()) != EOF){
        if ((input >= 'a' && input <= 'z') || (input >= 'A' && input <= 'Z'))
            has_char = 1;
        if (input >= '0' && input <= '9')
            has_num = 1;
        if (has_char && has_num){
            fprintf(stderr, "Error: mixed input types\n");
            free(buffer);
            return 1;
        }

        buffer[len] = (char)input;
        len++;
        if (len == cap){
            cap *= 2;
            buffer = realloc(buffer, cap);
        }
    }
    buffer[len] = '\0';

    char *token = strtok(buffer, " \t\n");
    while (token != NULL){
        if (has_num){
            int num = atoi(token);
            if (is_add)
                printf("%d ", num + 1);
            else if (is_inv)
                printf("%d ", -num);
        }
        else if (has_char){
            for (int i = 0; token[i] != '\0'; i++){
                char c = token[i];
                if (is_add){
                    if (c == 'z')
                        printf("a");
                    else if (c == 'Z')
                        printf("A");
                    else
                        printf("%c", c + 1);
                }
                else if (is_inv){
                    if (c >= 'a' && c <= 'z')
                        printf("%c", c - 32);
                    else if (c >= 'A' && c <= 'Z')
                        printf("%c", c + 32);
                }
            }
            printf(" ");
        }
        token = strtok(NULL, " \t\n");
    }

    free(buffer);
    return 0;
}