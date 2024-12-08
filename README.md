# File Organizer and Tagging System

This program organizes files based on their types, provides a tagging system for file management, and allows easy searching of files by name or tag. It is designed to run on Windows and simplifies file organization and retrieval tasks.

## Features

1. **File Sorting by Type**:
   - Automatically organizes files in the "Files" folder by their extensions.
   - Creates new folders for each file type, named after the file extensions.
   - Provides options for optional or continuous naming.

2. **Tagging System**:
   - Allows users to assign up to 10 tags per file.
   - Tags are stored in a `tags.txt` file in an unencrypted format for future searches.
   - Provides an option to add, remove, or edit tags for files.

3. **File Search**:
   - Search files by name: Finds files without requiring the extension.
   - Search files by tag: Retrieves files based on their associated tags.
   - Both search methods transfer matched files to a "Search" folder.

4. **Editing System**:
   - Edit files name: Finds files and edits their name.
   - Edit tags: Modify, remove, or add new tags to files.

5. **Dynamic Folder Management**:
   - Ensures "Files" and "Search" folders are created at runtime if they don’t exist.

6. **Seamless Return to Original Location**:
   - Files moved to the "Search" folder during a search are restored to their original locations upon completion.

## Usage

### 1. Clone the repository to your local machine.
```
git clone https://github.com/Umoig/File-Shorter.git
cd File-Shorter
```

### 2. Prepare the Environment:
   - Place the files to be organized in a folder named Files in the same directory as the compiled executable.

### 3. Compile the code:
```
gcc File_Shorter_WIN.c -o main
```

### 4. Run the executable:
```
./main
```

The program will automatically organize files by type within the "Files" folder, moving each to a folder named after its extension.

## Code Overview

1. **Sorting**:
   - Scans the Files folder for all files.
   - Creates folders for new extensions and moves files accordingly.
     
2. **Tagging**:
   - Allows users to add tags to files.
   - Updates tags.txt for persistent tag storage.

3. **Searching**:
   - Searches files by name or tags.
   - Matches are moved to the Search folder.
     
4. **Editing**:
   - Users can edit file names or modify tags.
   - Provides options to remove or edit existing tags.
   
5. **Restore Functionality**:
   - Files moved to the `Search` folder can be returned to their original locations.

## License
This project is open-source and available under the MIT [LICENSE](LICENSE).
