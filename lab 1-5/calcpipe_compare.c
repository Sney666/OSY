#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int main(int argc, char* argv[]){
    if(argc < 3){
        fprintf(stderr, "Error: missing arguments");
        return 1;
    }

    int pipeEven1[2], pipeEven2[2], pipeEven3[2], pipeEven4[2];
    int pipeOdd1[2], pipeOdd2[2], pipeOdd3[2], pipeOdd4[2];
    pipe(pipeEven1); pipe(pipeEven2); pipe(pipeEven3); pipe(pipeEven4);
    pipe(pipeOdd1); pipe(pipeOdd2); pipe(pipeOdd3); pipe(pipeOdd4);

    if (fork() == 0){
        dup2(pipeEven1[1], STDOUT_FILENO);

        close(pipeEven1[0]); close(pipeEven1[1]);
        close(pipeEven2[0]); close(pipeEven2[1]);
        close(pipeEven3[0]); close(pipeEven3[1]);
        close(pipeEven4[0]); close(pipeEven4[1]);
        close(pipeOdd1[0]); close(pipeOdd1[1]);
        close(pipeOdd2[0]); close(pipeOdd2[1]);
        close(pipeOdd3[0]); close(pipeOdd3[1]);
        close(pipeOdd4[0]); close(pipeOdd4[1]);

        char* args[] = {"./gen", argv[1], NULL};
        execvp(args[0], args);
        _exit(0);
    }

    if (fork() == 0){
        dup2(pipeEven1[0], STDIN_FILENO);
        dup2(pipeEven2[1], STDOUT_FILENO);

        close(pipeEven1[0]); close(pipeEven1[1]);
        close(pipeEven2[0]); close(pipeEven2[1]);
        close(pipeEven3[0]); close(pipeEven3[1]);
        close(pipeEven4[0]); close(pipeEven4[1]);
        close(pipeOdd1[0]); close(pipeOdd1[1]);
        close(pipeOdd2[0]); close(pipeOdd2[1]);
        close(pipeOdd3[0]); close(pipeOdd3[1]);
        close(pipeOdd4[0]); close(pipeOdd4[1]);

        char* args[] = {"./filter", "even", NULL};
        execvp(args[0], args);
        _exit(0);
    }

    if (fork() == 0){
        dup2(pipeEven2[0], STDIN_FILENO);
        dup2(pipeEven3[1], STDOUT_FILENO);

        close(pipeEven1[0]); close(pipeEven1[1]);
        close(pipeEven2[0]); close(pipeEven2[1]);
        close(pipeEven3[0]); close(pipeEven3[1]);
        close(pipeEven4[0]); close(pipeEven4[1]);
        close(pipeOdd1[0]); close(pipeOdd1[1]);
        close(pipeOdd2[0]); close(pipeOdd2[1]);
        close(pipeOdd3[0]); close(pipeOdd3[1]);
        close(pipeOdd4[0]); close(pipeOdd4[1]);

        char* args[] = {"./transform", argv[2], NULL};
        execvp(args[0], args);
        _exit(0);
    }

    if (fork() == 0){
        dup2(pipeEven3[0], STDIN_FILENO);
        dup2(pipeEven4[1], STDOUT_FILENO);

        close(pipeEven1[0]); close(pipeEven1[1]);
        close(pipeEven2[0]); close(pipeEven2[1]);
        close(pipeEven3[0]); close(pipeEven3[1]);
        close(pipeEven4[0]); close(pipeEven4[1]);
        close(pipeOdd1[0]); close(pipeOdd1[1]);
        close(pipeOdd2[0]); close(pipeOdd2[1]);
        close(pipeOdd3[0]); close(pipeOdd3[1]);
        close(pipeOdd4[0]); close(pipeOdd4[1]);

        char* args[] = {"./sum", NULL};
        execvp(args[0], args);
        _exit(0);
    }

    if (fork() == 0){
        dup2(pipeOdd1[1], STDOUT_FILENO);

        close(pipeEven1[0]); close(pipeEven1[1]);
        close(pipeEven2[0]); close(pipeEven2[1]);
        close(pipeEven3[0]); close(pipeEven3[1]);
        close(pipeEven4[0]); close(pipeEven4[1]);
        close(pipeOdd1[0]); close(pipeOdd1[1]);
        close(pipeOdd2[0]); close(pipeOdd2[1]);
        close(pipeOdd3[0]); close(pipeOdd3[1]);
        close(pipeOdd4[0]); close(pipeOdd4[1]);

        char* args[] = {"./gen", argv[1], NULL};
        execvp(args[0], args);
        _exit(0);
    }

    if (fork() == 0){
        dup2(pipeOdd1[0], STDIN_FILENO);
        dup2(pipeOdd2[1], STDOUT_FILENO);

        close(pipeEven1[0]); close(pipeEven1[1]);
        close(pipeEven2[0]); close(pipeEven2[1]);
        close(pipeEven3[0]); close(pipeEven3[1]);
        close(pipeEven4[0]); close(pipeEven4[1]);
        close(pipeOdd1[0]); close(pipeOdd1[1]);
        close(pipeOdd2[0]); close(pipeOdd2[1]);
        close(pipeOdd3[0]); close(pipeOdd3[1]);
        close(pipeOdd4[0]); close(pipeOdd4[1]);

        char* args[] = {"./filter", "odd", NULL};
        execvp(args[0], args);
        _exit(0);
    }

    if (fork() == 0){
        dup2(pipeOdd2[0], STDIN_FILENO);
        dup2(pipeOdd3[1], STDOUT_FILENO);

        close(pipeEven1[0]); close(pipeEven1[1]);
        close(pipeEven2[0]); close(pipeEven2[1]);
        close(pipeEven3[0]); close(pipeEven3[1]);
        close(pipeEven4[0]); close(pipeEven4[1]);
        close(pipeOdd1[0]); close(pipeOdd1[1]);
        close(pipeOdd2[0]); close(pipeOdd2[1]);
        close(pipeOdd3[0]); close(pipeOdd3[1]);
        close(pipeOdd4[0]); close(pipeOdd4[1]);

        char* args[] = {"./transform", argv[2], NULL};
        execvp(args[0], args);
        _exit(0);
    }

    if (fork() == 0){
        dup2(pipeOdd3[0], STDIN_FILENO);
        dup2(pipeOdd4[1], STDOUT_FILENO);

        close(pipeEven1[0]); close(pipeEven1[1]);
        close(pipeEven2[0]); close(pipeEven2[1]);
        close(pipeEven3[0]); close(pipeEven3[1]);
        close(pipeEven4[0]); close(pipeEven4[1]);
        close(pipeOdd1[0]); close(pipeOdd1[1]);
        close(pipeOdd2[0]); close(pipeOdd2[1]);
        close(pipeOdd3[0]); close(pipeOdd3[1]);
        close(pipeOdd4[0]); close(pipeOdd4[1]);

        char* args[] = {"./sum", NULL};
        execvp(args[0], args);
        _exit(0);
    }

    close(pipeEven1[0]); close(pipeEven1[1]);
    close(pipeEven2[0]); close(pipeEven2[1]);
    close(pipeEven3[0]); close(pipeEven3[1]);
    close(pipeEven4[1]);
    close(pipeOdd1[0]); close(pipeOdd1[1]);
    close(pipeOdd2[0]); close(pipeOdd2[1]);
    close(pipeOdd3[0]); close(pipeOdd3[1]);
    close(pipeOdd4[1]);

    char final_even[16] = {0};
    char final_odd[16] = {0};
    read(pipeEven4[0], final_even, sizeof(final_even));
    read(pipeOdd4[0], final_odd, sizeof(final_odd));

    close(pipeEven4[0]);
    close(pipeOdd4[0]);

    int even = atoi(final_even);
    int odd = atoi(final_odd);

    printf("Even:\t%d\n", even);
    printf("Odd:\t%d\n", odd);
    printf("Total:\t%d\n", even + odd);
    printf("Difference:\t%d\n", even - odd);

    for (int i = 0; i < 8; i++)
    {
        wait(NULL);
    }

    return 0;
}