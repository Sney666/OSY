#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char* argv[]){
    if(argc < 2){
        fprintf(stderr, "Error: missing file name.\n");
        return 1;
    }

    int useFSeek = 0;
    char* file;

    if (strcmp(argv[1], "-f") == 0) {
        if (argc < 3) {
            fprintf(stderr, "Error: missing file name after -f.\n");
            return 1;
        }
        useFSeek = 1;
        file = argv[2];
    } 
    else
        file = argv[1];

    FILE *f = NULL;
    int fd = -1;

    if (useFSeek) {
        f = fopen(file, "r");
        if (f == NULL) {
            perror("fopen failed");
            return 1;
        }
        fd = fileno(f); 
    }
    else {
        fd = open(file, O_RDONLY);
        if (fd == -1) {
            perror("open failed");
            return 1;
        }
    }

    struct stat sb;
    if (fstat(fd, &sb) == -1) {
        perror("fstat failed");
        return 1;
    }
    
    off_t last_size = sb.st_size;

    if(useFSeek)
        fseek(f, 0, SEEK_END);
    else
        lseek(fd, 0, SEEK_END);
        
    while (1){
        sleep(1);
        fstat(fd, &sb);
        if (sb.st_size > last_size){
            char buffer[1024];
            if (useFSeek) {
                size_t bytes;
                while ((bytes = fread(buffer, 1, sizeof(buffer), f)) > 0) {
                    fwrite(buffer, 1, bytes, stdout);
                    fflush(stdout);
                }
            } else {
                ssize_t bytes;
                while ((bytes = read(fd, buffer, sizeof(buffer))) > 0) {
                    write(STDOUT_FILENO, buffer, bytes);
                }
            }
            last_size = sb.st_size;
        }
        else if(sb.st_size < last_size){
            if(useFSeek)
                fseek(f, 0, SEEK_SET);
            else
                lseek(fd, 0, SEEK_SET);
            last_size = 0;
        }
    }

    return 0;
}