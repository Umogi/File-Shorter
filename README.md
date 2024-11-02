# File Organizer by Type

This program organizes files within a given directory ("Files" folder) based on their file type. It scans all files in the "Files" folder, including those nested within subdirectories, and moves them into newly created directories based on their file extensions. If a file with a new extension is found, the program creates a corresponding folder automatically.

## Features

- File Type Detection: Detects the file extension of each file in the specified folder.

- Automatic Folder Creation: Creates a new folder for any previously undetected file type.

- Recursive Folder Traversal: Moves files out of subdirectories within the "Files" folder.

- Error Handling: Handles errors in file renaming, directory creation, and directory access with descriptive output.


## Usage

1. Clone the repository to your local machine.


2. Place the files you want to organize into a folder named Files.


3. Compile the code:
```
gcc main.c -o main
```

4. Run the executable:
```
./main
```

The program will automatically organize files by type within the "Files" folder, moving each to a folder named after its extension.

## Code Overview

The main components of this program include:

- File Type Management:

   - addExt() - Adds new file types to an array if not previously encountered.

   - lookForExt() - Searches for an existing file type in the array; adds if not found.


- File and Directory Operations:

   - move_file() - Moves files between directories.

   - file_name_finder() - Finds files in the specified directory and categorizes them by type.

   - move_files() - Recursively moves files from subdirectories to the base "Files" directory.


- Main Program:

    - Sets up the base directory, scans files, organizes them by type, and creates necessary folders.
