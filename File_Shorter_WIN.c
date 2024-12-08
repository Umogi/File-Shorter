#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <conio.h>
#include <windows.h>
#include <unistd.h>

#define MAX_TAGS 10
#define MAX_TAG_LENGTH 50
#define MAX_FILES 10000

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

int maxTags = 10;
int contFlagTag = 0;
int tagPrime = 0;
char tags[10][50]; // Up to 10 tags
char contTags[10][50]; // Up to 10 continuous tags

// Create folder if it doesn't exist
void createFolder(const char *folderName) {
    if (mkdir(folderName) != 0) {
        
    }
    return;
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

    // Read the tags from the file
    while (fscanf(tagFile, "%s %d", fileTags[fileTagCount].filePath, &fileTags[fileTagCount].tagCount) == 2) {
        for (int i = 0; i < fileTags[fileTagCount].tagCount; i++) {
            fscanf(tagFile, "%s", fileTags[fileTagCount].tags[i]);
        }
        fileTagCount++;
    }

    fclose(tagFile);
}

// Add tags to a file and save in a text file
void tagFile(const char *filePath, int fileCount) {
    int tagCount = tagPrime;
    const char *fileName = strrchr(filePath, '\\');
    fileName = (fileName) ? fileName + 1 : filePath;

    char newFilePath[MAX_PATH];
    strcpy(newFilePath, filePath);

    // Adding files
    system("cls||clear");

    char choise;
    if(contFlagTag == 0){
        printf("Any continuous tags? (y/n): ");
        scanf(" %c", &choise);
        system("cls||clear");

        if(choise == 'y' || choise == 'Y'){
            
            contFlagTag = 1;

            printf("Enter the continious tags for %s. Max %d tags, type 'done' to finish: \n", fileName, maxTags);
            printf("Tags that will have also continious numbering add to them a '*'\n");
            
            while (tagCount < maxTags) {
                printf("Tag %d: ", tagCount + 1);
                scanf("%49s", contTags[tagCount]);
                if(strcmp(contTags[tagCount], "done") == 0) break;
                tagCount++;
            }
            
            tagPrime = tagCount;
        }
    }else if(contFlagTag == 1){
        printf("Continue the same tags? (y/n): ");
        scanf(" %c", &choise);
        system("cls||clear");

        if(choise == 'n' || choise == 'N'){
            contFlagTag = 0;
            tagPrime = 0;
        }
    }

    int Count = 0;
    while (Count < tagCount + 1) {
        if(strchr(contTags[Count], '*')){
            sprintf(tags[Count], "%s", contTags[Count]);
            size_t len = strlen(tags[Count]);
            
            tags[Count][len - 1] = '\0';

            sprintf(tags[Count], "%s%d", tags[Count], fileCount);
        }else{
            sprintf(tags[Count], "%s", contTags[Count]);
        }
        Count++;
    }

    printf("Enter tags for %s. Max %d tags, type 'done' to finish: \n", fileName, maxTags-tagCount);
    while (tagCount < maxTags) {
        printf("Tag %d: ", tagCount + 1);
        scanf("%49s", tags[tagCount]);
        if (strcmp(tags[tagCount], "done") == 0) break;
        tagCount++;
    }

    // Reomoves the * and adds 
    int numIncrim = 0;

    // Add to memory
    strcpy(fileTags[fileTagCount].filePath, newFilePath);
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
        Sleep(1);
        return;
    }

    fprintf(tagFile, "%s %d", newFilePath, tagCount);
    for (int i = 0; i < tagCount; i++) {
        fprintf(tagFile, " %s", tags[i]);
    }
    fprintf(tagFile, "\n");
    fclose(tagFile);
}

// Sort files by extension and add tags
void sortFiles() {
    system("cls||clear");

    printf("Put your files in the 'Files' folder and press Enter to start sorting.\n");
    getchar();
    system("cls||clear");

    DIR *dir = opendir(filesFolder);
    if (!dir) {
        perror("Error opening 'Files' folder");
        sleep(1);
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        char entryPath[MAX_PATH];
        sprintf(entryPath, "%s\\%s", filesFolder, entry->d_name);

        char *ext = strrchr(entry->d_name, '.');
        if (ext) {
            char extFolder[MAX_PATH];
            sprintf(extFolder, "%s\\%s", exePath, ext + 1);

            createFolder(extFolder);

            // Prompt for renaming
            char newName[MAX_PATH];
            printf("Would you like to rename %s ? (y/n): ", entry->d_name);
            char choice;
            scanf(" %c", &choice);
            getchar();
            system("cls||clear");

            if (choice == 'y' || choice == 'Y') {
                printf("Enter new name: ");
                fgets(newName, sizeof(newName), stdin);

                // Remove newline character from fgets
                char *newline = strchr(newName, '\n');
                if (newline) *newline = '\0';
                
                strcat(newName, ext);
            }else{
                strcpy(newName, entry->d_name);
            }

            // Move file
            char destPath[MAX_PATH];
            sprintf(destPath, "%s\\%s", extFolder, newName);
            if (rename(entryPath, destPath) != 0) {
                perror("\nError renaming/moving file");
                sleep(1);

                int counter = 1;
                while (access(destPath, F_OK) == 0) { // Check if file exists
                    char uniqueName[MAX_PATH];
                    char baseName[MAX_PATH];
                    strncpy(baseName, newName, ext - entry->d_name); // Extract base name
                    baseName[ext - entry->d_name] = '\0';

                    sprintf(uniqueName, "%s%d%s", baseName, counter, ext);
                    sprintf(destPath, "%s\\%s", extFolder, uniqueName);
                    counter++;
                }
                rename(entryPath, destPath);
            }

            // Add tags
            tagFile(destPath, 0);
        }
    }
    closedir(dir);

    printf("Sorting complete!\n");
    Sleep(1);
}

void contNameSort(){
    system("cls||clear");

    printf("Put your files in the 'Files' folder and press Enter to start sorting.\n");
    getchar();
    system("cls||clear");

    DIR *dir = opendir(filesFolder);
    if(!dir) {
        perror("Error opening 'Files' folder");
        Sleep(1);
        return;
    }

    struct dirent *entry;
    int fileCount = 0;
    
    char newName[128];
    printf("What will the countinuous name will be: ");
    scanf(" %128s", &newName);

    while((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        char entryPath[MAX_PATH];
        sprintf(entryPath, "%s\\%s", filesFolder, entry->d_name);
        
        char *ext = strrchr(entry->d_name, '.');
        if(ext){
            char extFolder[MAX_PATH];
            sprintf(extFolder, "%s\\%s", exePath, ext + 1);

            createFolder(extFolder);

            // Countinuous naming system
            fileCount++;
            char newFileName[MAX_PATH];
            sprintf(newFileName, "%s%d", newName, fileCount);
            strcat(newFileName, ext);

            // Move file
            char destPath[MAX_PATH];
            sprintf(destPath, "%s\\%s", extFolder, newFileName);
            if (rename(entryPath, destPath) != 0) {
                perror("Error renaming/moving file");
                Sleep(1);
            }

            // Add tags
            tagFile(destPath, fileCount);

            // Qustion !
            system("cls||clear");
            printf("Countinue the same name? (y/n): ");
            char choice;
            scanf(" %c", &choice);
            if(choice == 'n' || choice == 'N') {
                fileCount = 0;

                system("cls||clear");
                printf("What will the new countinuous name will be: ");
                scanf(" %128s", &newName);
            }
            
        }
    }
    closedir(dir);

    printf("Sorting complete!\n");
    Sleep(1);
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

    DIR *dir = opendir(exePath);
    if (!dir) {
        perror("Error opening parent folder");
        return;
    }

    int found = 0;
    for (int i = 0; i < fileTagCount; i++) {
        for (int j = 0; j < fileTags[i].tagCount; j++) {
            if (strcmp(searchTag, fileTags[i].tags[j]) == 0) {
                printf("%s\n", fileTags[i].filePath);
                transferFile(fileTags[i].filePath, searchFolder);
                found = 1;
                break;
            }
        }
    }

    if (!found) {
        printf("No files found with the tag '%s'.\n", searchTag);
        Sleep(1);
    } else {
        printf("Search complete. Files moved to 'Search' folder.\n");
        Sleep(1);
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
        Sleep(1);
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, "Files") != 0 && strcmp(entry->d_name, "Search") != 0) {
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

    printf("\nSearch complete. Files moved to 'Search' folder.\n");
    Sleep(1);
}

void editName(){
    char searchName[100];
    
    system("cls||clear");
    printf("Enter file name for renaming (without extension): ");
    scanf("%99s", searchName);

    char tagFilePath[MAX_PATH];
    sprintf(tagFilePath, "%s\\tags.txt", exePath);

    // Traverse through extension folders
    FILE *tagFile = fopen(tagFilePath, "r");
    if (!tagFile) {
        perror("Error opening tags file");
        Sleep(1);
        return;
    }

    FileTag tempTags[MAX_FILES];
    int tempFileTagCount = 0;
    while (fscanf(tagFile, "%s %d", tempTags[tempFileTagCount].filePath, &tempTags[tempFileTagCount].tagCount) == 2) {
        for (int i = 0; i < tempTags[tempFileTagCount].tagCount; i++) {
            fscanf(tagFile, "%s", tempTags[tempFileTagCount].tags[i]);
        }
        tempFileTagCount++;
    }
    fclose(tagFile);

    // Search for the file
    int found = 0;
    for (int i = 0; i < tempFileTagCount; i++) {
        const char *fileName = strrchr(tempTags[i].filePath, '\\');
        fileName = (fileName) ? fileName + 1 : tempTags[i].filePath;

        // Extract the name without extension
        char baseName[100], *ext = strrchr(fileName, '.');
        if (ext) {
            strncpy(baseName, fileName, ext - fileName);
            baseName[ext - fileName] = '\0';
        } else {
            strcpy(baseName, fileName);
        }

        if (strstr(baseName, searchName) == 0) {
            found = 1;

            // Prompt for new name
            char newFileName[100];
            system("cls||clear");
            printf("File found: %s\n", fileName);
            printf("Enter the new name (without extension): ");
            scanf("%99s", newFileName);

            char folderPath[MAX_PATH];
            strncpy(folderPath, tempTags[i].filePath, strrchr(tempTags[i].filePath, '\\') - tempTags[i].filePath);
            folderPath[strrchr(tempTags[i].filePath, '\\') - tempTags[i].filePath] = '\0';

            // Construct the new file path
            char newFilePath[MAX_PATH];
            sprintf(newFilePath, "%s\\%s\\%s%s", exePath, folderPath + strlen(exePath) + 1, newFileName, ext);

            // Rename the file on disk
            if (rename(tempTags[i].filePath, newFilePath) != 0) {
                perror("Error renaming file");
                Sleep(1);
                return;
            }

            // Update the memory
            strcpy(tempTags[i].filePath, newFilePath); 

            printf("\nFile renamed to: %s\n", newFilePath);
            Sleep(1);
            break;
        }
    }

    if (!found) {
        printf("File '%s' not found.\n", searchName);
        Sleep(1);
        return;
    }

    // Save updated data back to tags.txt
    tagFile = fopen(tagFilePath, "w");
    if (!tagFile) {
        perror("Error writing tags file");
        Sleep(1);
        return;
    }

    for (int i = 0; i < tempFileTagCount; i++) {
        fprintf(tagFile, "%s %d", tempTags[i].filePath, tempTags[i].tagCount);
        for (int j = 0; j < tempTags[i].tagCount; j++) {
            fprintf(tagFile, " %s", tempTags[i].tags[j]);
        }
        fprintf(tagFile, "\n");
    }
    fclose(tagFile);

    printf("\nFile name updated successfully.\n");
    Sleep(1);
}

void editTags() {
    char searchName[MAX_PATH];
    system("cls||clear");
    printf("Enter the name of the file to edit tags (with extension): ");
    scanf("%s", searchName);

    char tagFilePath[MAX_PATH];
    sprintf(tagFilePath, "%s\\tags.txt", exePath);

    // Traverse through extension folders
    FILE *tagFile = fopen(tagFilePath, "r");
    if (!tagFile) {
        perror("Error opening tags file");
        Sleep(1);
        return;
    }

    FileTag tempTags[MAX_FILES];
    int tempFileTagCount = 0;
    while (fscanf(tagFile, "%s %d", tempTags[tempFileTagCount].filePath, &tempTags[tempFileTagCount].tagCount) == 2) {
        for (int i = 0; i < tempTags[tempFileTagCount].tagCount; i++) {
            fscanf(tagFile, "%s", tempTags[tempFileTagCount].tags[i]);
        }
        tempFileTagCount++;
    }
    fclose(tagFile);
    
    int found = 0;
    for (int i = 0; i < tempFileTagCount; i++) {

        const char *fileName = strrchr(tempTags[i].filePath, '\\');
        fileName = (fileName) ? fileName + 1 : tempTags[i].filePath;

        // Extract the name without extension
        char baseName[100], *ext = strrchr(fileName, '.');
        if (ext) {
            strncpy(baseName, fileName, ext - fileName);
            baseName[ext - fileName] = '\0';
        } else {
            strcpy(baseName, fileName);
        }

        if (strcmp(baseName, searchName) == 0) {
            found = 1;


            // Provide options to edit tags
            while (1) {

                // Display tags
                system("cls||clear");
                printf("Current tags for %s:\n", searchName);
                for (int y = 0; y < fileTags[i].tagCount; y++) {
                    printf("%d. %s\n", y + 1, fileTags[i].tags[y]);
                }

                printf("\nWhat would you like to do?\n");
                printf("1. Add a new tag\n");
                printf("2. Remove a tag\n");
                printf("3. Edit a tag\n");
                printf("4. Finish editing\n");
                printf("Choice: ");
                int choice;
                scanf("%d", &choice);

                if (choice == 1) {
                    // Add a new tag
                    if (fileTags[i].tagCount < MAX_TAGS) {
                        printf("Enter the new tag: ");
                        scanf(" %s", fileTags[i].tags[fileTags[i].tagCount]);
                        fileTags[i].tagCount++;
                        printf("Tag added successfully.\n");
                    } else {
                        printf("Maximum number of tags reached. Cannot add more.\n");
                    }
                } else if (choice == 2) {
                    // Remove a tag
                    printf("Enter the number of the tag to remove: ");
                    int tagNum;
                    scanf(" %d", &tagNum);

                    if (tagNum > 0 && tagNum <= fileTags[i].tagCount) {
                        for (int y = tagNum - 1; y < fileTags[i].tagCount - 1; y++) {
                            strcpy(fileTags[i].tags[y], fileTags[i].tags[y + 1]);
                        }
                        fileTags[i].tagCount--;
                        printf("Tag removed successfully.\n");
                    } else {
                        printf("Invalid tag number.\n");
                    }
                } else if (choice == 3) {
                    // Edit an existing tag
                    printf("Enter the number of the tag to edit: ");
                    int tagNum;
                    scanf(" %d", &tagNum);

                    if (tagNum > 0 && tagNum <= fileTags[i].tagCount) {
                        printf("Enter the new value for tag %d: ", tagNum);
                        scanf(" %s", fileTags[i].tags[tagNum - 1]);
                        printf("Tag updated successfully.\n");
                    } else {
                        printf("Invalid tag number.\n");
                    }
                } else if (choice == 4) {
                    // Finish editing
                    break;
                } else {
                    printf("Invalid choice. Please try again.\n");
                }
            }
        }
    }

    if (!found) {
        printf("File '%s' not found.\n", searchName);
        Sleep(1);
        return;
    }

    tagFile = fopen(tagFilePath, "w");
    if (!tagFile) {
        perror("Error opening tags file");
        Sleep(1);
        return;
    }

    for (int i = 0; i < tempFileTagCount; i++) {
        fprintf(tagFile, "%s %d", tempTags[i].filePath, tempTags[i].tagCount);
        for (int j = 0; j < tempTags[i].tagCount; j++) {
            fprintf(tagFile, " %s", tempTags[i].tags[j]);
        }
        fprintf(tagFile, "\n");
    }

    fclose(tagFile);
    printf("Tags updated successfully.\n");
    Sleep(1);
}


// Return files to their original folders
void returnFiles() {
    DIR *dir = opendir(searchFolder);
    if (!dir) {
        perror("Error opening 'Search' folder");
        Sleep(1);
        return;
    }

    struct dirent *entry;
    
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        char filePath[MAX_PATH];
        sprintf(filePath, "%s\\%s", searchFolder, entry->d_name);

        // Read original path from tags.txt
        char tagFilePath[MAX_PATH];
        sprintf(tagFilePath, "%s\\tags.txt", exePath);

        FILE *tagFile = fopen(tagFilePath, "r");
        if (!tagFile) continue;

        char line[MAX_PATH * 2];  // Buffer to hold each line
        int found = 0;

        while (fgets(line, sizeof(line), tagFile)) {
            char origFilePath[MAX_PATH];
            // Extract the first token (the file path)
            if (sscanf(line, "%s", origFilePath) == 1) {
                // Compare only the file names
                const char *fileNameInTag = strrchr(origFilePath, '\\');
                if (!fileNameInTag) fileNameInTag = origFilePath;
                else fileNameInTag++;

                if (strcmp(fileNameInTag, entry->d_name) == 0) {
                    
                    rename(filePath, origFilePath);
                    found = 1;
                    break;
                }
            }
        }
    }
    closedir(dir);

    printf("\nFiles returned to their original folders.\n");
    Sleep(1);
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

void editFiles(){
    int choice;
    system("cls||clear");
    printf("Edit:\n");
    printf("1.File Name\n");
    printf("2.File Tags\n");
    printf("Choice: ");
    scanf("%d", &choice);
    getchar(); // Clear input buffer
    if (choice == 1) {
        editName();
    } else if (choice == 2) {
        editTags();
    } else {
        printf("Invalid choice!\n");
    }
}

void sortOptionsFiles(){
    int choice;
    tagPrime = 0;
    contFlagTag = 0;
    system("cls||clear");
    printf("Naming Options:\n");
    printf("1. Optional naming\n");
    printf("2. Continuous naming\n");
    printf("Choice: ");
    scanf("%d", &choice);
    getchar(); // Clear input buffer
    if (choice == 1) {
        sortFiles();
    } else if (choice == 2) {
        contNameSort();
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
        printf("3. Edit\n");
        printf("4. Return\n");
        printf("5. Exit\n");
        printf("Choice: ");
        scanf(" %d", &choice);
        getchar(); // Clear input buffer

        switch (choice) {
            case 1:
                sortOptionsFiles();
                break;
            case 2:
                searchFiles();
                break;
            case 3:
                editFiles();
                break;
            case 4:
                returnFiles();
                break;
            case 5:
                exit(0);
                break;
            default:
                printf("Invalid choice! Try again.\n");
                break;
        }
        
    }

    return 0;
}
