
# Customized Virtual File System (CVFS)

## Description
The Customized Virtual File System (CVFS) provides multiple functionalities that are similar to the UNIX/Linux File Subsystem. It provides necessary commands and system call implementations of the file system through customized shell. All necessary data structures of file system like Incore Inode Table, File Table, User File Descriptor Table are implemented. The entire file manipulation occurs on RAM and does not affect any secondary storage device.

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


## Commands implemented using this project
Command | Description
------- | ------------------------------------------
ls      | To list out all the files
clear   | To clear the console
create  | Create a new file
open    | Open specific file
close   | Close specific file
closeall| Close all the opened files
read    | To read contents from the file
write   | To write contents into the file
truncate| To remove all the data from the file
rm      | To delete the file
stat    | Display information about the file
fstat   | Display information using the File Descriptor
exit    | To terminate the File System

## How to Run
1. Compile the project using a C++ compiler.
   ```
   g++ -o CVFS CVFS.cpp
   ```
2. Run the executable.
   ```
   ./CVFS
   ```
   
#### Reference
Linux System Programming by Robert Love


