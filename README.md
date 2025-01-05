
# Customized Virtual File System (CVFS)

## Description
The Customized Virtual File System (CVFS) project implements a virtual file system with the following core functionalities:
- File creation, deletion, reading, and writing.
- File state management using inodes, file tables, and a superblock.

In this project we implement all necessary data structures of file system like Incore Inode
Table, File Table, UAREA, User File Descriptor table(UFDT).

## Technology Used
- Programming Language: C++

## User Interface Used
- Command Line Interface (CLI)

## Platform Required
- Operating System: Linux or Windows
- Additional software: C++ environment set up

## Hardware Requirements
- CPU: Any modern multi-core processor
- RAM: Minimum 4GB (more depending on the size of the file system and operations)
- Storage: Sufficient storage to handle the virtual file system's data and metadata 

## Features
- **File Operations**: Create, read, write, delete, and truncate files.
- **File Types**: Support for regular.
- **Permissions**: Manage file permissions (Read, Write, or Read+Write).
- **Efficient Resource Management**: Uses a superblock to track inodes and manage memory dynamically.
- **Command Interface**: Provides user-friendly commands for file system interaction.

## File Structures
- **SUPERBLOCK**: Tracks the total and free inodes in the system.
- **INODE**: Represents file metadata and buffers for data storage.
- **FILETABLE**: Maintains the state of an open file, including offsets and modes.
- **UFDT**: Keeps track of all open files and their corresponding file tables.

## Command Reference
### General Commands
- `create <FileName> <Permission>`: Creates a new file with specified permissions.
- `read <FileName> <BytesToRead>`: Reads the specified number of bytes from a file.
- `write <FileName>`: Writes data to a file.
- `truncate <FileName>`: Clears all data from the specified file.
- `rm <FileName>`: Deletes the specified file.

### File Management
- `open <FileName> <Mode>`: Opens a file in the specified mode (READ, WRITE, READ+WRITE).
- `close <FileName>`: Closes the specified file.
- `closeall`: Closes all open files.
- `lseek <FileName> <Offset> <StartPoint>`: Changes the file offset.

### Metadata Commands
- `stat <FileName>`: Displays metadata of the specified file.
- `fstat <FileDescriptor>`: Displays metadata of a file using its descriptor.
- `ls`: Lists all files in the system.

### Help and Manual
- `help`: Displays the list of available commands.
- `man <Command>`: Provides a manual entry for the specified command.

## How to Run
1. Compile the project using a C++ compiler.
   ```
   g++ -o CVFS CVFS.cpp
   ```
2. Run the executable.
   ```
   ./CVFS
   ```

## Author
Gaurav Gavhane

## License
This project is licensed under the MIT License.
