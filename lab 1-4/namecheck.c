#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <sys/wait.h>

typedef struct {
    int id;
    char name[64];
    int length;
    int matches[3];
} record_t;

int main(int argc, char* argv[]){
    if(argc < 5){
        fprintf(stderr, "Error: Some arguments are missing");
        return 1;
    }

    char* file = argv[1];
    int minLength = atoi(argv[2]);
    char firstLetter = argv[3][0];
    char requiredLetter = argv[4][0];
    FILE* f;
    int num = 0;

    char buffer[64];
    record_t records[64];

    int pipe1[2], pipe2[2], pipe3[2], pipe4[2];
    pipe(pipe1); 
    pipe(pipe2); 
    pipe(pipe3);
    pipe(pipe4);

    if (fork() == 0) {
        close(pipe1[1]); 
        close(pipe2[0]);
        close(pipe3[0]); 
        close(pipe3[1]);
        close(pipe4[0]);
        close(pipe4[1]);

        char line[64];

        while(read(pipe1[0], line, sizeof(line)) > 0){
            num++;
            records[num - 1].id = num;

            for (int i = 0; line[i] != '\0' ; i++) {
                line[i] = tolower(line[i]);
            }

            if(tolower(line[0] == tolower(firstLetter)))
                records[num - 1].matches[0] = 1;
            else
                records[num - 1].matches[0] = 0;
            
            strcpy(records[num - 1].name, line);
            
            write(pipe2[1], &records[num - 1], sizeof(records[num - 1]));
        }
        
        close(pipe1[0]);
        close(pipe2[1]);
        exit(0);
    }

    if (fork() == 0) {
        close(pipe1[0]);
        close(pipe1[1]);
        close(pipe2[1]);
        close(pipe3[0]);
        close(pipe4[0]);
        close(pipe4[1]);

        while(read(pipe2[0], &records[num], sizeof(records[num])) > 0){
            num++;

            int len = strlen(records[num - 1].name);

            records[num - 1].length = len;

            if(len >= minLength)
                records[num - 1].matches[1] = 1;
            else
                records[num - 1].matches[1] = 0;
            
            write(pipe3[1], &records[num - 1], sizeof(records[num - 1]));
        }

        close(pipe2[0]);
        close(pipe3[1]);
        exit(0);
    }

    if (fork() == 0) {
        close(pipe1[0]);
        close(pipe1[1]);
        close(pipe2[0]);
        close(pipe2[1]);
        close(pipe3[1]);
        close(pipe4[0]);

        while(read(pipe3[0], &records[num], sizeof(records[num])) > 0){
            num++;
            
            records[num - 1].name[0] = toupper(records[num - 1].name[0]);
            strcpy(records[num - 1].name, records[num - 1].name);

            records[num - 1].matches[2] = 0;

            for (int i = 0; records[num - 1].name[i] != '\0' ; i++) {
                if(tolower(records[num - 1].name[i] == tolower(requiredLetter))){
                    records[num - 1].matches[2] = 1;
                    break;
                }
            }

            write(pipe4[1], &records[num - 1], sizeof(records[num - 1]));
        }

        close(pipe3[0]);
        close(pipe4[1]);
        exit(0);
    }

    close(pipe1[0]);
    close(pipe2[0]);
    close(pipe2[1]);
    close(pipe3[0]);
    close(pipe3[1]);
    close(pipe4[1]);

    f = fopen(file, "r");
    if (!f) return 1;
    char line[64];
    int matched = 0;

    while(fgets(buffer, sizeof(buffer), f) != NULL){
        buffer[strcspn(buffer, "\n")] = 0;
        write(pipe1[1], buffer, strlen(buffer) + 1);
        usleep(50000);

        read(pipe4[0], &records[num], sizeof(records[num]));
        num++;
        if(records[num - 1].matches[2] == 1 && records[num - 1].matches[1] == 1 && records[num - 1].matches[0] == 1){
            matched++;
            printf("[%d] %s\t(%d) [%d,%d,%d] MATCH\n", records[num - 1].id, records[num - 1].name, records[num - 1].length, records[num - 1].matches[0], records[num - 1].matches[1], records[num - 1].matches[2]);
        }
        else
            printf("[%d] %s\t(%d) [%d,%d,%d]\n", records[num - 1].id, records[num - 1].name, records[num - 1].length, records[num - 1].matches[0], records[num - 1].matches[1], records[num - 1].matches[2]);
    }

    printf("Zpracovano jmen: %d\n", num);
    printf("Vyhovovalo: %d\n", matched);

    close(pipe1[1]);
    close(pipe4[0]);
    fclose(f);

    wait(NULL);
    wait(NULL);
    wait(NULL);

    return 0;
}