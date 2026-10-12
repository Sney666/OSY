#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int main(int argc, char* argv[]){
    if(argc < 4){
        fprintf(stderr, "Error: missing arguments");
        return 1;
    }

    int use_v = 0;
    int offset = 0;
    if (argc == 5 && strcmp(argv[1], "-v") == 0) {
        use_v = 1;
        offset = 1;
    }

    int pipe1[2], pipe2[2], pipe3[2];
    pipe(pipe1);
    pipe(pipe2);
    pipe(pipe3);

    if (fork() == 0){
        if(use_v)
            fprintf(stderr, "[calcpipe] Spouštím ./gen %s\n", argv[1 + offset]);

        close(pipe1[0]);
        close(pipe2[0]);
        close(pipe2[1]);
        close(pipe3[0]);
        close(pipe3[1]);

        dup2(pipe1[1], STDOUT_FILENO);

        close(pipe1[1]);

        char* args[] = {"./gen", argv[1 + offset], NULL};
        execvp(args[0], args);
    }

    if (fork() == 0){
        if(use_v)
            fprintf(stderr, "[calcpipe] Spouštím ./filter %s\n", argv[2 + offset]);

        close(pipe1[1]);
        close(pipe2[0]);
        close(pipe3[0]);
        close(pipe3[1]);

        dup2(pipe1[0], STDIN_FILENO);
        dup2(pipe2[1], STDOUT_FILENO);
        
        close(pipe1[0]);
        close(pipe2[1]);

        char* args[] = {"./filter", argv[2 + offset], NULL};
        execvp(args[0], args);
    }

    if (fork() == 0){
        if(use_v)
            fprintf(stderr, "[calcpipe] Spouštím ./transform %s\n", argv[3 + offset]);

        close(pipe1[0]);
        close(pipe1[1]);
        close(pipe2[1]);
        close(pipe3[0]);

        dup2(pipe2[0], STDIN_FILENO);
        dup2(pipe3[1], STDOUT_FILENO);
        
        close(pipe2[0]);
        close(pipe3[1]);

        char* args[] = {"./transform", argv[3 + offset], NULL};
        execvp(args[0], args);
    }

    if (fork() == 0){
        if(use_v)
            fprintf(stderr, "[calcpipe] Spouštím ./sum\n");

        close(pipe1[0]);
        close(pipe1[1]);
        close(pipe2[0]);
        close(pipe2[1]);
        close(pipe3[1]);

        dup2(pipe3[0], STDIN_FILENO);
        
        close(pipe3[0]);

        char* args[] = {"./sum", NULL};
        execvp(args[0], args);
    }

    close(pipe1[0]);
    close(pipe1[1]);
    close(pipe2[0]);
    close(pipe2[1]);
    close(pipe3[0]);
    close(pipe3[1]);

    wait(NULL);
    wait(NULL);
    wait(NULL);
    wait(NULL);

    return 0;
}