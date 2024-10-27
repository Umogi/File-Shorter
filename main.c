#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <errno.h> 

#define MAX_HASHTAGS 5
#define MAX_FILES 1000
#define MAX_FILE_TYPES 100

int size = 0;
char FileTypes[MAX_FILE_TYPES][10];

struct file_data {
    char filename[256];
    long filesize;
    char filetype[10];
    time_t date_modified;
};

void addExt(const char *ext) {
    if (size < MAX_FILE_TYPES) {
        strcpy(FileTypes[size], ext);
        printf("Added extension '%s' at index %d\n", ext, size);
        size++;
    } else {
        printf("Error: Maximum number of file types reached.\n");
    }
}

int lookForExt(const char *ext){
    for (int i = 0; i < size; i++) {
        if (strcmp(FileTypes[i], ext) == 0) {
            return i; 
        }
    }
    addExt(ext);
    return size - 1;
}

const char* get_file_extension(const char* filename) {
    const char *dot = strrchr(filename, '.');
    if (!dot || dot == filename) return "";
    return dot + 1;
}

void move_file(const char *source, const char *destination) {
    if (rename(source, destination) == 0) {
        printf("Moved %s to %s\n", source, destination);
    } else {
        printf("Error moving file %s to %s: %s\n", source, destination, strerror(errno));
    }
}

void file_name_finder(const char *dir_path){
    struct dirent *entry;
    DIR *dir = opendir(dir_path);

    if (!dir){
        printf("Error openting file!");
        return;
    }
    
    while((entry = readdir(dir)) != NULL){
        if(entry->d_type == DT_REG){
            const char *ext = get_file_extension(entry->d_name);

            printf("File: %s, Type: %s\n", entry->d_name, ext);

            char source[512] = ".\\Files\\";
            snprintf(source, sizeof(source), "%s\\%s", dir_path, entry->d_name);
            const char *destination_dir;

            char destination[512];

            int extLocation = lookForExt(ext);
            
            struct stat st = {0};
            
            destination_dir = FileTypes[extLocation];
            
            printf("%s\n", destination_dir);

            if (stat(destination_dir, &st) == -1) {
                if (_mkdir(destination_dir) == 0) {
                    printf("Folder created successfully.\n");
                } else {
                    perror("Error creating folder");
                }
            } else {
                printf("Folder already exists.\n");
            }

            snprintf(destination, sizeof(destination), "%s\\%s", destination_dir, entry->d_name);

            move_file(source, destination);
            
        }
    }

    closedir(dir);
}

void move_files(const char *base_dir, const char *dest_dir) {
    struct dirent *entry;
    DIR *dir = opendir(base_dir);
    if (!dir) {
        perror("Could not open base directory");
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        char src_path[1024];
        snprintf(src_path, sizeof(src_path), "%s/%s", base_dir, entry->d_name);

        struct stat entry_stat;
        if (stat(src_path, &entry_stat) == -1) {
            perror("Error accessing entry");
            continue;
        }

        if (S_ISDIR(entry_stat.st_mode)) {
            move_files(src_path, dest_dir);
        } else {
            char dest_path[1024];
            snprintf(dest_path, sizeof(dest_path), "%s/%s", dest_dir, entry->d_name);

            if (rename(src_path, dest_path) == 0) {
                printf("Moved: %s -> %s\n", src_path, dest_path);
            } else {
                perror("Error moving file");
            }
        }
    }

    closedir(dir);
}

int main(){
    struct file_data files[MAX_FILES];
    int file_count = 0;
    const char *base_directory = ".\\Files";
    const char *destination_directory = ".\\Files";
    const char *dir_path = ".\\Files";

    struct stat st = {0};

    if (stat(dir_path, &st) == -1) {
        if (_mkdir(dir_path) == 0) {
            printf("Folder created successfully.\n");
        } else {
            perror("Error creating folder");
        }
    } else {
        printf("Folder already exists.\n");
    }

    move_files(base_directory, destination_directory);
    file_name_finder(dir_path);

    return 0;
}