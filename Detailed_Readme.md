
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

## Flow of the project:  
### Most UNIX filesystem types have a similar general structure, although the exact details vary quite a bit. The central concepts are superblock, inode , data block, directory block , and indirection block. The superblock contains information about the filesystem as a whole, such as its size (the exact information here depends on the filesystem). An inode contains all information about a file, except its name(in our case we store name also). The name is stored in the directory, together with the number of the inode. A directory entry consists of a filename and the number of the inode which represents the file. The inode contains the numbers of several data blocks, which are used to store the data in the file. There is space only for a few data block numbers in the inode, however, and if more are needed, more space for pointers to the data blocks is allocated dynamically. These dynamically allocated blocks are indirect blocks; the name indicates that in order to find the data block, one has to find its number in the indirect block first.   As this project fully functions on the primary memory, we are creating the data structure as linked list (Singly Linear). So we are creating the Superblock 
### which contains the information of the inodes that i.e. total number of inodes and number of free inodes. These inodes are created in the DILB block i.e. Data Inode List Block. We created the singly linear linked list of fifty inodes in which each inode has a unique inode number and these inode contains the information of the files which are stored in the data block. The information of each file gets stored in the separate inode that means for number of inodes we can create number of files (i.e. for fifty inodes, we can create fifty files).   When the command prompt opens, user will have to enter the username and password for the valid authentication. When user will enter the command, that command will be searched in the program then the further operations will happen.   When the user creates the file , firstly, the filename is get searched in the DILB block for duplication, means, for the existence of same file name. If file name is not exists then the separate inode gets allocated for that file. Firstly, the memory gets allocated for that file then the inode in the DILB block will be initialise for that file. That inode contains the file information like, name of the file, file permission, link count, reference count, etc. When user will enter some data or text in the file, for that data, the memory gets allocated in the file Buffer and all the text or data will be put in that buffer. When user will read the data giving the size of the bytes, how many data user wants to read. Then that bytes of data (if exists that number of bytes) will be shown to the user. User can write that data upto 1024 bytes (as our file size is 1024 bytes) and can also read the 1024 bytes of data.    When user enters ‘truncate’ command, the data which is stored in the file which contains the text will gets erased or gets deleted.

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
