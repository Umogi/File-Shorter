#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <windows.h>

#define MAX_TAGS 10
#define MAX_TAG_LENGTH 50
#define MAX_FILES 100

typedef struct {
    char filePath[MAX_PATH];
    int tagCount;
    char tags[MAX_TAGS][MAX_TAG_LENGTH];
} FileTag;

FileTag fileTags[MAX_FILES];
int fileTagCount = 0;

char exePath[MAX_PATH];
char filesFolder[MAX_PATH];
char searchFolder[MAX_PATH];

// Create folder if it doesn't exist
void createFolder(const char *folderName) {
    if (mkdir(folderName) != 0) {

    }
}

// Load tags save file
void loadTags() {
    char tagFilePath[MAX_PATH];
    sprintf(tagFilePath, "%s\\tags.txt", exePath);

    FILE *tagFile = fopen(tagFilePath, "r");
    if (!tagFile) {
        // Create tags.txt if it doesn't exist
        tagFile = fopen(tagFilePath, "w");
        if (!tagFile) {
            perror("Error creating tags file");
            return;
        }
        fclose(tagFile);
        return;
    }

    fileTagCount = 0;
    while (fscanf(tagFile, "%s %d", fileTags[fileTagCount].filePath, &fileTags[fileTagCount].tagCount) == 2) {
        for (int i = 0; i < fileTags[fileTagCount].tagCount; i++) {
            fscanf(tagFile, "%s", fileTags[fileTagCount].tags[i]);
        }
        fileTagCount++;
    }
    fclose(tagFile);
}

// Add tags to a file and save in a text file
void tagFile(const char *filePath) {
    char tags[10][50]; // Up to 10 tags
    int tagCount = 0;

    const char *fileName = strrchr(filePath, '\\');
    fileName = (fileName) ? fileName + 1 : filePath;

    system("cls||clear");
    printf("Enter tags for %s (max 10 tags, type 'done' to finish):\n", fileName);
    while (tagCount < 10) {
        printf("Tag %d: ", tagCount + 1);
        scanf("%49s", tags[tagCount]);
        if (strcmp(tags[tagCount], "done") == 0) break;
        tagCount++;
    }

    // Add to memory
    strcpy(fileTags[fileTagCount].filePath, filePath);
    fileTags[fileTagCount].tagCount = tagCount;
    for (int i = 0; i < tagCount; i++) {
        strcpy(fileTags[fileTagCount].tags[i], tags[i]);
    }
    fileTagCount++;

    // Append to tags.txt
    char tagFilePath[MAX_PATH];
    sprintf(tagFilePath, "%s\\tags.txt", exePath);

    FILE *tagFile = fopen(tagFilePath, "a");
    if (!tagFile) {
        perror("Error opening tags file");
        return;
    }

    fprintf(tagFile, "%s %d", filePath, tagCount);
    for (int i = 0; i < tagCount; i++) {
        fprintf(tagFile, " %s", tags[i]);
    }
    fprintf(tagFile, "\n");
    fclose(tagFile);
}

// Sort files by extension and add tags
void sortFiles() {
    printf("Put your files in the 'Files' folder and press Enter to start sorting.\n");
    getchar();

    DIR *dir = opendir(filesFolder);
    if (!dir) {
        perror("Error opening 'Files' folder");
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) { // Regular file
            char *ext = strrchr(entry->d_name, '.');
            if (ext) {
                char extFolder[MAX_PATH];
                sprintf(extFolder, "%s\\%s", exePath, ext + 1);

                createFolder(extFolder);

                // Move file
                char srcPath[MAX_PATH], destPath[MAX_PATH];
                sprintf(srcPath, "%s\\%s", filesFolder, entry->d_name);
                sprintf(destPath, "%s\\%s", extFolder, entry->d_name);
                rename(srcPath, destPath);

                // Add tags
                tagFile(destPath);
            }
        }
    }
    closedir(dir);

    printf("Sorting complete!\n");
}

// Transfer file to a folder
void transferFile(const char *filePath, const char *destFolder) {
    char *fileName = strrchr(filePath, '\\') + 1;
    char destPath[MAX_PATH];
    sprintf(destPath, "%s\\%s", destFolder, fileName);
    rename(filePath, destPath);
}

// Search files by tag
void searchByTag() {
    char searchTag[50];
    printf("Enter tag: ");
    scanf("%49s", searchTag);

    int found = 0;
    for (int i = 0; i < fileTagCount; i++) {
        for (int j = 0; j < fileTags[i].tagCount; j++) {
            if (strcmp(fileTags[i].tags[j], searchTag) == 0) {
                transferFile(fileTags[i].filePath, searchFolder);
                found = 1;
                break;
            }
        }
    }

    if (!found) {
        printf("No files found with the tag '%s'.\n", searchTag);
    } else {
        printf("Search complete. Files moved to 'Search' folder.\n");
    }
}

// Search files by name
void searchByName() {
    char searchName[100];
    printf("Enter file name (without extension): ");
    scanf("%99s", searchName);

    // Traverse through extension folders
    DIR *dir = opendir(exePath);
    if (!dir) {
        perror("Error opening parent folder");
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR && strcmp(entry->d_name, "Files") != 0 && strcmp(entry->d_name, "Search") != 0) {
            char folderPath[MAX_PATH];
            sprintf(folderPath, "%s\\%s", exePath, entry->d_name);

            DIR *subdir = opendir(folderPath);
            if (!subdir) continue;

            struct dirent *subentry;
            while ((subentry = readdir(subdir)) != NULL) {
                if (strstr(subentry->d_name, searchName)) {
                    char filePath[MAX_PATH];
                    sprintf(filePath, "%s\\%s", folderPath, subentry->d_name);
                    transferFile(filePath, searchFolder);
                }
            }
            closedir(subdir);
        }
    }
    closedir(dir);

    printf("Search complete. Files moved to 'Search' folder.\n");
}

// Return files to their original folders
void returnFiles() {
    DIR *dir = opendir(searchFolder);
    if (!dir) {
        perror("Error opening 'Search' folder");
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) {
            char filePath[MAX_PATH];
            sprintf(filePath, "%s\\%s", searchFolder, entry->d_name);

            // Read original path from tags.txt
            char tagFilePath[MAX_PATH];
            sprintf(tagFilePath, "%s\\tags.txt", exePath);

            FILE *tagFile = fopen(tagFilePath, "r");
            if (!tagFile) continue;

            char origFilePath[MAX_PATH];
            while (fscanf(tagFile, "%s", origFilePath) == 1) {
                if (strstr(origFilePath, entry->d_name)) {
                    rename(filePath, origFilePath);
                    break;
                }
            }
            fclose(tagFile);
        }
    }
    closedir(dir);

    printf("Files returned to their original folders.\n");
}

// Search files by name or tag
void searchFiles() {
    int choice;
    system("cls||clear");
    printf("Search by:\n");
    printf("1. Name\n");
    printf("2. Tag\n");
    printf("Choice: ");
    scanf("%d", &choice);
    getchar(); // Clear input buffer

    if (choice == 1) {
        searchByName();
    } else if (choice == 2) {
        searchByTag();
    } else {
        printf("Invalid choice!\n");
    }
}

int main() {
    // Get the path of the executable
    GetModuleFileName(NULL, exePath, MAX_PATH);
    char *lastSlash = strrchr(exePath, '\\');
    if (lastSlash) *lastSlash = '\0';

    // Define folder paths
    sprintf(filesFolder, "%s\\Files", exePath);
    sprintf(searchFolder, "%s\\Search", exePath);

    // Create required folders
    createFolder(filesFolder);
    createFolder(searchFolder);

    // Load existing tags
    loadTags();

    int choice;
    while (1) {
        system("cls||clear");
        printf("Select an option:\n");
        printf("1. Sort\n");
        printf("2. Search\n");
        printf("3. Return\n");
        printf("4. Exit\n");
        printf("Choice: ");
        scanf("%d", &choice);
        getchar(); // Clear input buffer

        switch (choice) {
            case 1:
                sortFiles();
                break;
            case 2:
                searchFiles();
                break;
            case 3:
                returnFiles();
                break;
            case 4:
                exit(0);
            default:
                printf("Invalid choice! Try again.\n");
        }
    }

    return 0;
}