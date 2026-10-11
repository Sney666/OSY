#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <sys/wait.h>

int main(int argc, char* argv[]){
    if(argc < 2){
        fprintf(stderr, "Error: File is missing\n");
        return 1;
    }

    char* file = argv[1];
    FILE* f;

    char buffer[256];

    int pipe1[2], pipe2[2], pipe3[2];
    pipe(pipe1); 
    pipe(pipe2); 
    pipe(pipe3);

    printf("[rodič] Spouštím potomka 1\n");
    if (fork() == 0) {
        close(pipe1[1]); 
        close(pipe2[0]);
        close(pipe3[0]); 
        close(pipe3[1]);

        char line[256];
        char line2[512];
        int num = 0;

        while(read(pipe1[0], line, sizeof(line)) > 0){
            num++;
            printf("[potomek1] Přijal: \"%s\"\n", line);

            for (int i = 0; line[i] != '\0' ; i++) {
                line[i] = tolower(line[i]);
            }
            
            sprintf(line2, "%d. %s", num, line);
            printf("[potomek1] Posílám: \"%s\"\n", line2);
            
            write(pipe2[1], line2, strlen(line2) + 1);
        }
        
        close(pipe1[0]);
        close(pipe2[1]);
        exit(0);
    }

    printf("[rodič] Spouštím potomka 2\n");
    if (fork() == 0) {
        close(pipe1[0]);
        close(pipe1[1]);
        close(pipe2[1]); 
        close(pipe3[0]);

        char line[512];
        char line3[512];

        while(read(pipe2[0], line, sizeof(line)) > 0){
            printf("[potomek2] Přijal: \"%s\"\n", line);

            char *word = strchr(line, ' ');
            int len = 0;
            if (word != NULL) {
                len = strlen(word + 1);
            }

            sprintf(line3, "%s (%d)", line, len);
            printf("[potomek2] Posílám: \"%s\"\n", line3);
            
            write(pipe3[1], line3, strlen(line3) + 1);
        }

        close(pipe2[0]);
        close(pipe3[1]);
        exit(0);
    }

    printf("[rodič] Spouštím potomka 3\n");
    if (fork() == 0) {
        close(pipe1[0]);
        close(pipe1[1]);
        close(pipe2[0]);
        close(pipe2[1]);
        close(pipe3[1]);

        char line[512];

        while(read(pipe3[0], line, sizeof(line)) > 0){
            printf("[potomek3] Přijal: \"%s\"\n", line);
            
            char *word = strchr(line, ' ');
            if (word != NULL && *(word + 1) != '\0') {
                *(word + 1) = toupper(*(word + 1));
            }

            printf("[potomek3] Výstup: \"%s\"\n", line);
        }

        close(pipe3[0]);
        exit(0);
    }

    close(pipe1[0]);
    close(pipe2[0]);
    close(pipe2[1]);
    close(pipe3[0]);
    close(pipe3[1]);

    f = fopen(file, "r");
    if (!f) return 1;

    while(fgets(buffer, sizeof(buffer), f) != NULL){
        buffer[strcspn(buffer, "\n")] = 0;
        printf("[rodič] Čtu ze souboru: \"%s\"\n", buffer);
        write(pipe1[1], buffer, strlen(buffer) + 1);
        usleep(50000);
    }

    close(pipe1[1]);
    fclose(f);

    wait(NULL);
    wait(NULL);
    wait(NULL);

    return 0;
}