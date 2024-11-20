#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <limits.h>

#define MAX_TAGS 10
#define MAX_TAG_LENGTH 50
#define MAX_FILES 100

typedef struct {
    char filePath[PATH_MAX];
    int tagCount;
    char tags[MAX_TAGS][MAX_TAG_LENGTH];
} FileTag;

FileTag fileTags[MAX_FILES];
int fileTagCount = 0;

char exePath[PATH_MAX];
char filesFolder[PATH_MAX];
char searchFolder[PATH_MAX];

// Create folder if it doesn't exist
void createFolder(const char *folderName) {
    if (mkdir(folderName, 0777) != 0) {
        perror("Error creating folder");
    }
}

// Load tags save file
void loadTags() {
    char tagFilePath[PATH_MAX];
    sprintf(tagFilePath, "%s/tags.txt", exePath);

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
    char tags[MAX_TAGS][MAX_TAG_LENGTH]; // Up to 10 tags
    int tagCount = 0;

    const char *fileName = strrchr(filePath, '/');
    fileName = (fileName) ? fileName + 1 : filePath;

    system("clear");
    printf("Enter tags for %s (max 10 tags, type 'done' to finish):\n", fileName);
    while (tagCount < MAX_TAGS) {
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
    char tagFilePath[PATH_MAX];
    sprintf(tagFilePath, "%s/tags.txt", exePath);

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

// Remaining functions remain largely the same, with adjustments for paths.

int main() {
    // Get the path of the executable
    ssize_t len = readlink("/proc/self/exe", exePath, sizeof(exePath) - 1);
    if (len != -1) {
        exePath[len] = '\0';
        char *lastSlash = strrchr(exePath, '/');
        if (lastSlash) *lastSlash = '\0';
    } else {
        perror("Error getting executable path");
        return 1;
    }

    // Define folder paths
    sprintf(filesFolder, "%s/Files", exePath);
    sprintf(searchFolder, "%s/Search", exePath);

    // Create required folders
    createFolder(filesFolder);
    createFolder(searchFolder);

    // Load existing tags
    loadTags();

    int choice;
    while (1) {
        system("clear");
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
