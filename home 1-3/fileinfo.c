#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>

int main(int argc, char* argv[]){
    if(argc < 2){
        fprintf(stderr, "Error: missing file name.\n");
        return 1;
    }

    int useLStat = 0, i = 1;
    if(strcmp(argv[1], "-l") == 0){
        useLStat = 1;
        i++;
    }

    for (; i < argc; i++){
        char *file = argv[i];
        struct stat sb;
        const char *type;

        if(useLStat == 0){
            if (stat(file, &sb) == -1) {
                fprintf(stderr, "Error: file loading error.\n");
                continue;
            }
        }
        else{
            if (lstat(file, &sb) == -1) {
                fprintf(stderr, "Error: file loading error.\n");
                continue;
            }
        }

        if(S_ISREG(sb.st_mode))
            type = "regular file";
        else if(S_ISDIR(sb.st_mode))
            type = "directory";
        else
            type = "link";

        printf("Name: %s\n", file);
        printf("Type: %s\n", type);
        printf("Size: %d\n", sb.st_size);
        
        printf("Permissions: ");
        printf((sb.st_mode & S_IRUSR) ? "r" : "-");
        printf((sb.st_mode & S_IWUSR) ? "w" : "-");
        printf((sb.st_mode & S_IXUSR) ? "x" : "-");
        printf((sb.st_mode & S_IRGRP) ? "r" : "-");
        printf((sb.st_mode & S_IWGRP) ? "w" : "-");
        printf((sb.st_mode & S_IXGRP) ? "x" : "-");
        printf((sb.st_mode & S_IROTH) ? "r" : "-");
        printf((sb.st_mode & S_IWOTH) ? "w" : "-");
        printf((sb.st_mode & S_IXOTH) ? "x" : "-");
        printf("\n");

        printf("Inode: %d\n", sb.st_ino);
    }

    return 0;
}