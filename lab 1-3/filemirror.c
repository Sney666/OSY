#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

void mode_to_string(mode_t mode, char *str);

int main(int argc, char* argv[]){
    if(argc < 3){
        fprintf(stderr, "Error: missing file name.\n");
        return 1;
    }

    char* source;
    char* dest;
    int copyPermissions = 0;

    if (strcmp(argv[1], "-p") == 0) {
        if (argc < 4) {
            fprintf(stderr, "Error: missing source or destination file.\n");
            return 1;
        }
        copyPermissions = 1;
        source = argv[2];
        dest = argv[3];
    } else {
        source = argv[1];
        dest = argv[2];
    }

    struct stat sb;
    if (stat(source, &sb) == -1) {
        fprintf(stderr, "Error loading source file.\n");
        return 1;
    }

    if(!S_ISREG(sb.st_mode)){
        fprintf(stderr, "Error: file is not a regular file\n");
        return 1;
    }

    printf("Source: %s\n", source);
    printf("Size: %ld\n", (long)sb.st_size);
    printf("Inode: %lu\n", (unsigned long)sb.st_ino);
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

    int fd = open(source, O_RDONLY);
    if (fd == -1) {
        fprintf(stderr, "Error opening source file\n");
        return 1;
    }

    mode_t dest_mode = 0644;
    if (copyPermissions) {
        dest_mode = sb.st_mode & 07777;
    }

    int fd2 = open(dest, O_WRONLY | O_CREAT | O_TRUNC, dest_mode);
    if (fd2 == -1) {
        fprintf(stderr, "Error creating destination file\n");
        close(fd);
        return 1;
    }

    char buffer[1024];
    ssize_t bytes;
    size_t initial_copied = 0;

    while ((bytes = read(fd, buffer, sizeof(buffer))) > 0) {
        write(fd2, buffer, bytes);
        initial_copied += bytes;
    }
    printf("Initial copy: %zu bytes\n", initial_copied);

    off_t last_size = sb.st_size;
    unsigned long ino = sb.st_ino;
    mode_t last_mode = sb.st_mode;

    while (1){
        sleep(1);
        if(stat(source, &sb) == -1){
            printf("Source file disappeared\n");

            close(fd);
            close(fd2);

            return 1;
        }        
        if (sb.st_size > last_size){
            size_t newly_copied = 0;
            while ((bytes = read(fd, buffer, sizeof(buffer))) > 0) {
                write(fd2, buffer, bytes);
                newly_copied += bytes;
            }
            printf("+%zu bytes\n", newly_copied);
            last_size = sb.st_size;
        }
        else if(sb.st_size < last_size){
            printf("Source truncated: %ld -> %ld bytes\n", (long)last_size, (long)sb.st_size);

            if (ftruncate(fd2, 0) == -1) {
                fprintf(stderr, "ftruncate failed\n");
                break;
            }

            lseek(fd, 0, SEEK_SET);
            lseek(fd2, 0, SEEK_SET);

            size_t newly_copied = 0;
            while ((bytes = read(fd, buffer, sizeof(buffer))) > 0) {
                write(fd2, buffer, bytes);
                newly_copied += bytes;
            }

            last_size = sb.st_size;
        }
        if(sb.st_ino != ino){
            printf("Source replaced: inode %lu -> %lu \n", (unsigned long)ino, (unsigned long)sb.st_ino);

            close(fd);

            fd = open(source, O_RDONLY);
            if (fd == -1) {
                fprintf(stderr, "Error opening replaced file\n");
                break;
            }

            ftruncate(fd2, 0);
            lseek(fd2, 0, SEEK_SET);

            size_t newly_copied = 0;
            while ((bytes = read(fd, buffer, sizeof(buffer))) > 0) {
                write(fd2, buffer, bytes);
                newly_copied += bytes;
            }

            ino = sb.st_ino;
            last_size = sb.st_size;
        }
        if(copyPermissions && last_mode != sb.st_mode){
            char old_perms[10];
            char new_perms[10];

            mode_to_string(last_mode, old_perms);
            mode_to_string(sb.st_mode, new_perms);

            printf("Permissions changed: %s -> %s\n", old_perms, new_perms);

            fchmod(fd2, sb.st_mode);
            last_mode = sb.st_mode;
        }
    }

    close(fd);
    close(fd2);
    return 0;
}

void mode_to_string(mode_t mode, char *str){
    str[0] = (mode & S_IRUSR) ? 'r' : '-';
    str[1] = (mode & S_IWUSR) ? 'w' : '-';
    str[2] = (mode & S_IXUSR) ? 'x' : '-';
    str[3] = (mode & S_IRGRP) ? 'r' : '-';
    str[4] = (mode & S_IWGRP) ? 'w' : '-';
    str[5] = (mode & S_IXGRP) ? 'x' : '-';
    str[6] = (mode & S_IROTH) ? 'r' : '-';
    str[7] = (mode & S_IWOTH) ? 'w' : '-';
    str[8] = (mode & S_IXOTH) ? 'x' : '-';
    str[9] = '\0';
}