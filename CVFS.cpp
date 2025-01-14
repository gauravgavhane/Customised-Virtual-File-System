
///////////////////////////////////////////////////////////////////////////////////////////////////
//
//    Customized Virtual File System (CVFS)
//
//    Description:
//        This project implements a virtual file system with core functionalities, including:
//        - File creation, deletion, reading, and writing.
//        - File state management using inodes, file tables, and a superblock.
//
//    Features:
//        - Support for multiple open files via the UFDT (Universal File Descriptor Table).
//        - Permissions for Read, Write, and Read+Write operations.
//        - Efficient inode-based management for up to 50 files.
//
//    Author: Gaurav Gavhane
//    Date: 1 Jan 2025
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <iostream>

#define MAXINODE 50

#define READ 1
#define WRITE 2

#define MAXFILESIZE 2048

#define REGULAR 1
#define SPECIAL 2

#define START 0
#define CURRENT 1
#define END 2

///////////////////////////////////////////////////////////////////////////////////////////////////
//
//    Structure Name : SUPERBLOCK
//    Description    : States the availability of inodes.
//    Fields         : int TotalInodes  - Total inodes in the file system.
//                     int FreeInode    - Number of available inodes.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct superblock
{
    int TotalInodes;
    int FreeInode;
} SUPERBLOCK, *PSUPERBLOCK;

///////////////////////////////////////////////////////////////////////////////////////////////////
//
//    Structure Name : INODE
//    Description    : Represents a file in the file system, containing metadata and a data buffer.
//    Fields         : char FileName[50]    - Name of the file.
//                     int InodeNumber      - Unique inode number.
//                     int FileSize         - Maximum file size.
//                     int FileActualSize   - Current size of the file.
//                     int FileType         - Type of file (REGULAR or SPECIAL).
//                     char *Buffer         - Data buffer.
//                     int LinkCount        - Number of links to this file.
//                     int ReferenceCount   - Number of active references to this file.
//                     int permission       - Permissions (READ, WRITE, or READ+WRITE).
//                     struct inode *next   - Pointer to the next inode in the linked list.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct inode
{
    char FileName[50];
    int InodeNumber;
    int FileSize;
    int FileActualSize;
    int FileType;
    char *Buffer;
    int LinkCount;
    int ReferenceCount;
    int permission;
    struct inode *next;
} INODE, *PINODE, **PPINODE;

///////////////////////////////////////////////////////////////////////////////////////////////////
//
//    Structure Name : FILETABLE
//    Description    : Represents an open file, maintaining its state and position.
//    Fields         : int readoffset      - Current read offset in the file.
//                     int writeoffset     - Current write offset in the file.
//                     int count           - Count of active operations on this file.
//                     int mode            - Mode of the file (READ, WRITE, or READ+WRITE).
//                     PINODE ptrinode     - Pointer to the inode associated with the file.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct filetable
{
    int readoffset;
    int writeoffset;
    int count;
    int mode;
    PINODE ptrinode;
} FILETABLE, *PFILETABLE;

///////////////////////////////////////////////////////////////////////////////////////////////////
//
//    Structure Name : UFDT
//    Description    : Table to keep track of open files in the system.
//    Fields         : PFILETABLE ptrfiletable - Pointer to the file table of the open file.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct ufdt
{
    PFILETABLE ptrfiletable;
} UFDT;

UFDT UFDTArr[50];
SUPERBLOCK SUPERBLOCKobj;
PINODE head = NULL;

///////////////////////////////////////////////////////////////////////////////////////////////////
//
//    Function Name : man
//    Description   : Provides command usage and descriptions for system operations.
//    Input         : char* name - Command for which manual entry is required.
//    Output        : None
//
///////////////////////////////////////////////////////////////////////////////////////////////////

void man(char *name)
{
    if (name == NULL)
        return;

    if (strcmp(name, "create") == 0)
    {
        printf("Description : Used to create new regular file\n");
        printf("Usage : create File_name permission\n");
    }
    else if (strcmp(name, "read") == 0)
    {
        printf("Description : Used to read data from regular file\n");
        printf("Usage : read File_name No_Of_Bytes_To_Read\n");
    }
    else if (strcmp(name, "write") == 0)
    {
        printf("Description : Used to write into regular file\n");
        printf("Usage : write File_name\nAfter this write the data that we want to write\n");
    }
    else if (strcmp(name, "ls") == 0)
    {
        printf("Description : Used to list all the information of file\n");
        printf("Usage : ls\n");
    }
    else if (strcmp(name, "stat") == 0)
    {
        printf("Description : Used to display information of file\n");
        printf("Usage : stat File_name\n");
    }
    else if (strcmp(name, "fstat") == 0)
    {
        printf("Description : Used to display information of file\n");
        printf("Usage : fstat File_Descriptor\n");
    }
    else if (strcmp(name, "truncate") == 0)
    {
        printf("Description : Used to remove data from file\n");
        printf("Usage : truncate File_name\n");
    }
    else if (strcmp(name, "open") == 0)
    {
        printf("Description : Used to open existing file\n");
        printf("Usage : open File_name mode\n");
    }
    else if (strcmp(name, "close") == 0)
    {
        printf("Description : Used to close opened file\n");
        printf("Usage : close File_name\n");
    }
    else if (strcmp(name, "closeall") == 0)
    {
        printf("Description : Used to close all opened file\n");
        printf("Usage : closeall\n");
    }
    else if (strcmp(name, "lseek") == 0)
    {
        printf("Description : Used to change file offset\n");
        printf("Usage : lseek File_name ChangeinOffset StartPoint\n");
    }
    else if (strcmp(name, "rm") == 0)
    {
        printf("Description : Used to delete the file\n");
        printf("Usage : rm File_name\n");
    }
    else
    {
        printf("Error : No manual entry available.\n");
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
//    Function Name : DisplayHelp
//    Description   : Displays the list of available commands and their brief descriptions.
//    Input         : None
//    Output        : None
//
///////////////////////////////////////////////////////////////////////////////////////////////////

void DisplayHelp()
{
    printf("ls : To list out all the files\n");
    printf("clear : To clear console\n");
    printf("open : To open the file\n");
    printf("close : To close the file\n");
    printf("closeall : To close all opened file\n");
    printf("read : To Read the contents from file\n");
    printf("write : To write the contents into the file\n");
    printf("exit : To Terminate the file system\n");
    printf("stat : To Display information of file using name\n");
    printf("fstat : To Display information of file using file descriptor\n");
    printf("truncate : To remove all data the file\n");
    printf("rm : To delete the file\n");
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
//    Function Name : GetFDFromName
//    Description   : Retrieves the file descriptor for a file given its name.
//    Input         : char* name - Name of the file.
//    Output        : int       - File descriptor if found, or -1 if not found.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

int GetFDFromName(char *name)
{
    int i = 0;

    while (i < 50)
    {
        if (UFDTArr[i].ptrfiletable != NULL)
            if (strcmp((UFDTArr[i].ptrfiletable->ptrinode->FileName), name) == 0)
                break;
        i++;
    }

    if (i == 50)
        return -1;
    else
        return i;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
//    Function Name : Get_Inode
//    Description   : Retrieves the inode structure for a given file name.
//    Input         : char* name - Name of the file.
//    Output        : PINODE    - Pointer to the inode if found, or NULL if not found.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

PINODE Get_Inode(char *name)
{
    PINODE temp = head;
    int i = 0;

    if (name == NULL)
        return NULL;

    while (temp != NULL)
    {
        if (strcmp(name, temp->FileName) == 0)
            break;
        temp = temp->next;
    }
    return temp;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
//    Function Name : CreateDILB
//    Description   : Creates the Disk Inode List Block (DILB), initializing all inodes.
//    Input         : None
//    Output        : None
//
///////////////////////////////////////////////////////////////////////////////////////////////////

void CreateDILB()
{
    int i = 1;
    PINODE newn = NULL;
    PINODE temp = head;

    while (i <= MAXINODE)
    {
        newn = (PINODE)malloc(sizeof(INODE));

        newn->LinkCount = 0;
        newn->ReferenceCount = 0;
        newn->FileType = 0;
        newn->FileSize = 0;

        newn->Buffer = NULL;
        newn->next = NULL;

        newn->InodeNumber = i;

        if (temp == NULL)
        {
            head = newn;
            temp = head;
        }
        else
        {
            temp->next = newn;
            temp = temp->next;
        }
        i++;
    }
    printf("DILB created successfully\n");
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
//    Function Name : InitialiseSuperBlock
//    Description   : Initializes the superblock structure, setting up the system's inode capacity
//                    and marking all inodes as available.
//    Input         : None
//    Output        : None
//
///////////////////////////////////////////////////////////////////////////////////////////////////

void InitialiseSuperBlock()
{
    int i = 0;
    while (i < MAXINODE)
    {
        UFDTArr[i].ptrfiletable = NULL;
        i++;
    }

    SUPERBLOCKobj.TotalInodes = MAXINODE;
    SUPERBLOCKobj.FreeInode = MAXINODE;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
//    Function Name : CreateFile
//    Description   : Creates a new file with the specified name and permissions.
//    Input         : char* name      - The name of the file to create.
//                    int permission  - Permission settings (1: Read, 2: Write, 3: Read+Write).
//    Output        : int            - File descriptor on success, or error code:
//                                      -1: Invalid parameters
//                                      -2: No available inodes
//                                      -3: File already exists
//                                      -4: Memory allocation failure
//
///////////////////////////////////////////////////////////////////////////////////////////////////

int CreateFile(char *name, int permission)
{
    int i = 0;
    PINODE temp = head;

    if ((name == NULL) || (permission == 0) || (permission > 3))
        return -1;

    if (SUPERBLOCKobj.FreeInode == 0)
        return -2;

    (SUPERBLOCKobj.FreeInode)--;

    if (Get_Inode(name) != NULL)
        return -3;

    while (temp != NULL)
    {
        if (temp->FileType == 0)
            break;
        temp = temp->next;
    }

    while (i < 50)
    {
        if (UFDTArr[i].ptrfiletable == NULL)
            break;
        i++;
    }

    UFDTArr[i].ptrfiletable = (PFILETABLE)malloc(sizeof(FILETABLE));

    UFDTArr[i].ptrfiletable->count = 1;
    UFDTArr[i].ptrfiletable->mode = permission;
    UFDTArr[i].ptrfiletable->readoffset = 0;
    UFDTArr[i].ptrfiletable->writeoffset = 0;

    UFDTArr[i].ptrfiletable->ptrinode = temp;

    strcpy(UFDTArr[i].ptrfiletable->ptrinode->FileName, name);
    UFDTArr[i].ptrfiletable->ptrinode->FileType = REGULAR;
    UFDTArr[i].ptrfiletable->ptrinode->ReferenceCount = 1;
    UFDTArr[i].ptrfiletable->ptrinode->LinkCount = 1;
    UFDTArr[i].ptrfiletable->ptrinode->FileSize = MAXFILESIZE;
    UFDTArr[i].ptrfiletable->ptrinode->FileActualSize = 0;
    UFDTArr[i].ptrfiletable->ptrinode->permission = permission;
    UFDTArr[i].ptrfiletable->ptrinode->Buffer = (char *)malloc(MAXFILESIZE);

    return i;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
//    Function Name : rm_File
//    Description   : Removes a file and frees its resources.
//    Input         : char* name - Name of the file to remove.
//    Output        : int       - 0 on success, or -1 if the file is not found.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

int rm_File(char *name)
{
    int fd = 0;

    fd = GetFDFromName(name);
    if (fd == -1)
        return -1;

    (UFDTArr[fd].ptrfiletable->ptrinode->LinkCount)--;

    if (UFDTArr[fd].ptrfiletable->ptrinode->LinkCount == 0)
    {
        UFDTArr[fd].ptrfiletable->ptrinode->FileType = 0;
        free(UFDTArr[fd].ptrfiletable->ptrinode->Buffer);
        free(UFDTArr[fd].ptrfiletable);
    }

    UFDTArr[fd].ptrfiletable = NULL;
    (SUPERBLOCKobj.FreeInode)++;
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
//    Function Name : ReadFile
//    Description   : Reads data from a file into a buffer.
//    Input         : int fd      - File descriptor of the file.
//                    char* arr   - Buffer to store the read data.
//                    int isize   - Number of bytes to read.
//    Output        : int        - Number of bytes read on success, or error code:
//                                  -1: File not open
//                                  -2: Permission denied
//                                  -3: End of file reached
//                                  -4: Not a regular file
//
///////////////////////////////////////////////////////////////////////////////////////////////////

int ReadFile(int fd, char *arr, int isize)
{
    int read_size = 0;

    if (UFDTArr[fd].ptrfiletable == NULL)
        return -1;

    if (UFDTArr[fd].ptrfiletable->mode != READ && UFDTArr[fd].ptrfiletable->mode != READ + WRITE)
        return -2;

    if (UFDTArr[fd].ptrfiletable->ptrinode->permission != READ && UFDTArr[fd].ptrfiletable->ptrinode->permission != READ + WRITE)
        return -2;

    if (UFDTArr[fd].ptrfiletable->readoffset == UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)
        return -3;

    if (UFDTArr[fd].ptrfiletable->ptrinode->FileType != REGULAR)
        return -4;

    read_size = (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) - (UFDTArr[fd].ptrfiletable->readoffset);
    if (read_size < isize)
    {
        strncpy(arr, (UFDTArr[fd].ptrfiletable->ptrinode->Buffer) + (UFDTArr[fd].ptrfiletable->readoffset), read_size);
        UFDTArr[fd].ptrfiletable->readoffset = UFDTArr[fd].ptrfiletable->readoffset + read_size;
    }
    else
    {
        strncpy(arr, (UFDTArr[fd].ptrfiletable->ptrinode->Buffer) + (UFDTArr[fd].ptrfiletable->readoffset), isize);
        UFDTArr[fd].ptrfiletable->readoffset = UFDTArr[fd].ptrfiletable->readoffset + isize;
    }

    return isize;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
//    Function Name : WriteFile
//    Description   : Writes data to a file from a buffer.
//    Input         : int fd      - File descriptor of the file.
//                    char* arr   - Buffer containing the data to write.
//                    int isize   - Number of bytes to write.
//    Output        : int        - Number of bytes written on success, or error code:
//                                  -1: Permission denied
//                                  -2: Insufficient memory
//                                  -3: Not a regular file
//
///////////////////////////////////////////////////////////////////////////////////////////////////

int WriteFile(int fd, char *arr, int isize)
{
    if (((UFDTArr[fd].ptrfiletable->mode) != WRITE) && ((UFDTArr[fd].ptrfiletable->mode) != READ + WRITE))
        return -1;

    if (((UFDTArr[fd].ptrfiletable->ptrinode->permission) != WRITE) && ((UFDTArr[fd].ptrfiletable->ptrinode->permission) != READ + WRITE))
        return -1;

    if ((UFDTArr[fd].ptrfiletable->writeoffset) == MAXFILESIZE)
        return -2;

    if ((UFDTArr[fd].ptrfiletable->ptrinode->FileType) != REGULAR)
        return -3;

    strncpy((UFDTArr[fd].ptrfiletable->ptrinode->Buffer) + (UFDTArr[fd].ptrfiletable->writeoffset), arr, isize);

    (UFDTArr[fd].ptrfiletable->writeoffset) = (UFDTArr[fd].ptrfiletable->writeoffset) + isize;

    (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) = (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) + isize;

    return isize;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
//    Function Name : OpenFile
//    Description   : Opens an existing file for reading or writing.
//    Input         : char* name  - Name of the file to open.
//                    int mode    - Mode to open the file in (READ, WRITE, or READ+WRITE).
//    Output        : int        - File descriptor on success, or error code:
//                                  -1: Invalid parameters
//                                  -2: File not found
//                                  -3: Permission denied
//
///////////////////////////////////////////////////////////////////////////////////////////////////

int OpenFile(char *name, int mode)
{
    int i = 0;
    PINODE temp = NULL;

    if (name == NULL || mode <= 0)
        return -1;

    temp = Get_Inode(name);
    if (temp == NULL)
        return -2;

    if (temp->permission < mode)
        return -3;

    while (i < 50)
    {
        if (UFDTArr[i].ptrfiletable == NULL)
            break;
        i++;
    }

    UFDTArr[i].ptrfiletable = (PFILETABLE)malloc(sizeof(FILETABLE));
    if (UFDTArr[i].ptrfiletable == NULL)
        return -1;
    UFDTArr[i].ptrfiletable->count = 1;
    UFDTArr[i].ptrfiletable->mode = mode;
    if (mode == READ + WRITE)
    {
        UFDTArr[i].ptrfiletable->readoffset = 0;
        UFDTArr[i].ptrfiletable->writeoffset = 0;
    }
    else if (mode == READ)
    {
        UFDTArr[i].ptrfiletable->readoffset = 0;
    }
    else if (mode == WRITE)
    {
        UFDTArr[i].ptrfiletable->writeoffset = 0;
    }
    UFDTArr[i].ptrfiletable->ptrinode = temp;
    (UFDTArr[i].ptrfiletable->ptrinode->ReferenceCount)++;

    return i;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
//    Function Name : CloseFileByName
//    Description   : Closes a specific file by its name.
//    Input         : int fd  - File descriptor of the file to close.
//    Output        : None
//
///////////////////////////////////////////////////////////////////////////////////////////////////

void CloseFileByName(int fd)
{
    UFDTArr[fd].ptrfiletable->readoffset = 0;
    UFDTArr[fd].ptrfiletable->writeoffset = 0;
    (UFDTArr[fd].ptrfiletable->ptrinode->ReferenceCount)--;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
//    Function Name : CloseFileByName
//    Description   : Closes a specific file by its name.
//    Input         : char* name  - Name of the file to close.
//    Output        : int        - 0 on success, or -1 if the file is not found.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

int CloseFileByName(char *name)
{
    int i = 0;
    i = GetFDFromName(name);
    if (i == -1)
        return -1;

    UFDTArr[i].ptrfiletable->readoffset = 0;
    UFDTArr[i].ptrfiletable->writeoffset = 0;
    (UFDTArr[i].ptrfiletable->ptrinode->ReferenceCount)--;

    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
//    Function Name : CloseAllFile
//    Description   : Closes all currently open files.
//    Input         : None
//    Output        : None
//
///////////////////////////////////////////////////////////////////////////////////////////////////

void CloseAllFile()
{
    int i = 0;
    while (i < 50)
    {
        if (UFDTArr[i].ptrfiletable != NULL)
        {
            UFDTArr[i].ptrfiletable->readoffset = 0;
            UFDTArr[i].ptrfiletable->writeoffset = 0;
            (UFDTArr[i].ptrfiletable->ptrinode->ReferenceCount)--;
            break;
        }
        i++;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
//    Function Name : LseekFile
//    Description   : Changes the file offset for reading or writing operations.
//    Input         : int fd      - File descriptor of the file.
//                    int size    - Offset value.
//                    int from    - Reference point (START, CURRENT, END).
//    Output        : int        - 0 on success, or error code:
//                                  -1: Invalid parameters
//
///////////////////////////////////////////////////////////////////////////////////////////////////

int LseekFile(int fd, int size, int from)
{
    if ((fd < 0) || (from > 2))
        return -1;
    if (UFDTArr[fd].ptrfiletable == NULL)
        return -1;

    if ((UFDTArr[fd].ptrfiletable->mode == READ) || (UFDTArr[fd].ptrfiletable->mode == READ + WRITE))
    {
        if (from == CURRENT)
        {
            if (((UFDTArr[fd].ptrfiletable->readoffset) + size) > UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)
                return -1;
            if (((UFDTArr[fd].ptrfiletable->readoffset) + size) < 0)
                return -1;
            (UFDTArr[fd].ptrfiletable->readoffset) = (UFDTArr[fd].ptrfiletable->readoffset) + size;
        }
        else if (from == START)
        {
            if (size > (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize))
                return -1;
            if (size < 0)
                return -1;
            (UFDTArr[fd].ptrfiletable->readoffset) = size;
        }
        else if (from == END)
        {
            if ((UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) + size > MAXFILESIZE)
                return -1;
            if (((UFDTArr[fd].ptrfiletable->readoffset) + size) < 0)
                return -1;
            (UFDTArr[fd].ptrfiletable->readoffset) = (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) + size;
        }
    }
    else if (UFDTArr[fd].ptrfiletable->mode == WRITE)
    {
        if (from == CURRENT)
        {
            if (((UFDTArr[fd].ptrfiletable->writeoffset) + size) > MAXFILESIZE)
                return -1;
            if (((UFDTArr[fd].ptrfiletable->writeoffset) + size) < 0)
                return -1;
            if (((UFDTArr[fd].ptrfiletable->writeoffset) + size) > (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize))
                (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) = (UFDTArr[fd].ptrfiletable->writeoffset) + size;
            (UFDTArr[fd].ptrfiletable->writeoffset) = (UFDTArr[fd].ptrfiletable->writeoffset) + size;
        }
        else if (from == START)
        {
            if (size > MAXFILESIZE)
                return -1;
            if (size < 0)
                return -1;
            if (size > (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize))
                (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) = size;
            (UFDTArr[fd].ptrfiletable->writeoffset) = size;
        }
        else if (from == END)
        {
            if ((UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) + size > MAXFILESIZE)
                return -1;
            if (((UFDTArr[fd].ptrfiletable->writeoffset) + size) < 0)
                return -1;
            (UFDTArr[fd].ptrfiletable->writeoffset) = (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) + size;
        }
    }
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
//    Function Name : ls_file
//    Description   : Lists all files in the system, including their metadata.
//    Input         : None
//    Output        : None
//
///////////////////////////////////////////////////////////////////////////////////////////////////

void ls_file()
{
    int i = 0;
    PINODE temp = head;

    if (SUPERBLOCKobj.FreeInode == MAXINODE)
    {
        printf("Error : There are no files\n");
        return;
    }

    printf("\nFile Name\tInode number\tFile size\tLink count\n");
    printf("-------------------------------------------------------------------\n");
    while (temp != NULL)
    {
        if (temp->FileType != 0)
        {
            printf("%s\t\t%d\t\t%d\t\t%d\n", temp->FileName, temp->InodeNumber, temp->FileActualSize, temp->LinkCount);
        }
        temp = temp->next;
    }
    printf("-------------------------------------------------------------------\n");
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
//    Function Name : fstat_file
//    Description   : Displays metadata for a file based on its file descriptor.
//    Input         : int fd  - File descriptor of the file.
//    Output        : int    - 0 on success, or error code:
//                              -1: Invalid file descriptor
//                              -2: File not found
//
///////////////////////////////////////////////////////////////////////////////////////////////////

int fstat_file(int fd)
{
    PINODE temp = head;

    if (fd < 0)
        return -1;

    if (UFDTArr[fd].ptrfiletable == NULL)
        return -2;

    temp = UFDTArr[fd].ptrfiletable->ptrinode;

    printf("\n---------------Statistical Information about file-------------\n");
    printf("File name : %s\n", temp->FileName);
    printf("Inode Number %d\n", temp->InodeNumber);
    printf("File size : %d\n", temp->FileSize);
    printf("Actual File size : %d\n", temp->FileActualSize);
    printf("Link count : %d\n", temp->LinkCount);
    printf("Reference count : %d\n", temp->ReferenceCount);

    if (temp->permission == 1)
        printf("File Permission : Read only\n");
    else if (temp->permission == 2)
        printf("File Permission : Write\n");
    else if (temp->permission == 3)
        printf("File Permission : Read & Write\n");
    printf("--------------------------------------------------------------\n\n");

    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
//    Function Name : stat_file
//    Description   : Displays metadata for a file based on its name.
//    Input         : char* name  - Name of the file.
//    Output        : int        - 0 on success, or error code:
//                                  -1: File not found
//
///////////////////////////////////////////////////////////////////////////////////////////////////

int stat_file(char *name)
{
    PINODE temp = head;

    if (name == NULL)
        return -1;

    while (temp != NULL)
    {
        if (strcmp(name, temp->FileName) == 0)
            break;
        temp = temp->next;
    }

    if (temp == NULL)
        return -2;

    printf("\n---------------Statistical Information about file-------------\n");
    printf("File name : %s\n", temp->FileName);
    printf("Inode Number %d\n", temp->InodeNumber);
    printf("File size : %d\n", temp->FileSize);
    printf("Actual File size : %d\n", temp->FileActualSize);
    printf("Link count : %d\n", temp->LinkCount);
    printf("Reference count : %d\n", temp->ReferenceCount);

    if (temp->permission == 1)
        printf("File Permission : Read only\n");
    else if (temp->permission == 2)
        printf("File Permission : Write\n");
    else if (temp->permission == 3)
        printf("File Permission : Read & Write\n");
    printf("--------------------------------------------------------------\n\n");

    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
//    Function Name : truncate_File
//    Description   : Removes all data from a specified file.
//    Input         : char* name  - Name of the file to truncate.
//    Output        : int        - 0 on success, or -1 if the file is not found.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

int truncate_File(char *name)
{
    int fd = GetFDFromName(name);
    if (fd == -1)
        return -1;

    memset(UFDTArr[fd].ptrfiletable->ptrinode->Buffer, 0, 1024);
    UFDTArr[fd].ptrfiletable->readoffset = 0;
    UFDTArr[fd].ptrfiletable->writeoffset = 0;
    UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize = 0;
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
//    Function Name : main
//    Description   : Entry point for the CVFS
//    Input         : None
//    Output        : int - Exit status (0 for success).
//
///////////////////////////////////////////////////////////////////////////////////////////////////

int main()
{
    char *ptr = NULL;
    int ret = 0, fd = 0, count = 0;
    char command[4][80], str[80], arr[1024];

    InitialiseSuperBlock();
    CreateDILB();

    while (1)
    {
        fflush(stdin);
        strcpy(str, "");

        printf("\nVFS : > ");

        fgets(str, 80, stdin);
        count = sscanf(str, "%s %s %s %s", command[0], command[1], command[2], command[3]);

        if (count == 1)
        {
            if (strcmp(command[0], "ls") == 0)
            {
                ls_file();
                continue;
            }
            else if (strcmp(command[0], "closeall") == 0)
            {
                CloseAllFile();
                printf("All files closed successfully\n");
                continue;
            }
            else if (strcmp(command[0], "clear") == 0)
            {
                system("clear");
                continue;
            }
            else if (strcmp(command[0], "help") == 0)
            {
                DisplayHelp();
                continue;
            }
            else if (strcmp(command[0], "exit") == 0)
            {
                printf("Terminating the Virtual File System\n");
                break;
            }
            else
            {
                printf("\nERROR : Command not found !!!\n");
                continue;
            }
        }
        else if (count == 2)
        {
            if (strcmp(command[0], "stat") == 0)
            {
                ret = stat_file(command[1]);
                if (ret == -1)
                    printf("ERROR : Incorrect parameters\n");
                if (ret == -2)
                    printf("ERROR : There is no such file\n");
                continue;
            }
            else if (strcmp(command[0], "fstat") == 0)
            {
                ret = fstat_file(atoi(command[1]));
                if (ret == -1)
                    printf("ERROR : Incorrect parameters\n");
                if (ret == -2)
                    printf("ERROR : There is no such file\n");
                continue;
            }
            else if (strcmp(command[0], "close") == 0)
            {
                ret = CloseFileByName(command[1]);
                if (ret == -1)
                    printf("ERROR : There is no such file\n");
                continue;
            }
            else if (strcmp(command[0], "rm") == 0)
            {
                ret = rm_File(command[1]);
                if (ret == -1)
                    printf("ERROR : There is no such file\n");
                continue;
            }
            else if (strcmp(command[0], "man") == 0)
            {
                man(command[1]);
                continue;
            }
            else if (strcmp(command[0], "write") == 0)
            {
                fd = GetFDFromName(command[1]);
                if (fd == -1)
                {
                    printf("ERROR : Incorrect parameter\n");
                    continue;
                }
                printf("Enter the data : \n");
                scanf("%[^\n]", arr);

                ret = strlen(arr);
                if (ret == 0)
                {
                    printf("ERROR : Incorrect parameter\n");
                    continue;
                }
                ret = WriteFile(fd, arr, ret);
                if (ret == -1)
                    printf("ERROR : Permission denied\n");
                if (ret == -2)
                    printf("ERROR : There is no sufficient memory to write\n");
                if (ret == -3)
                    printf("ERROR : It is not a regular file\n");
            }
            else if (strcmp(command[0], "truncate") == 0)
            {
                ret = truncate_File(command[1]);
                if (ret == -1)
                    printf("ERROR : Incorrect parameter\n");
            }
            else
            {
                printf("\nERROR : Command not found !!!\n");
                continue;
            }
        }
        else if (count == 3)
        {
            if (strcmp(command[0], "create") == 0)
            {
                ret = CreateFile(command[1], atoi(command[2]));
                if (ret >= 0)
                    printf("File is successfully created with file descriptor : %d\n", ret);
                if (ret == -1)
                    printf("ERROR : Incorrect parameters\n");
                if (ret == -2)
                    printf("ERROR : There is no inodes\n");
                if (ret == -3)
                    printf("ERROR : File already exists\n");
                if (ret == -4)
                    printf("ERROR : Memory allocation failure\n");
                continue;
            }
            else if (strcmp(command[0], "open") == 0)
            {
                ret = OpenFile(command[1], atoi(command[2]));
                if (ret >= 0)
                    printf("File is successfully opened with file descriptor : %d\n", ret);
                if (ret == -1)
                    printf("ERROR : Incorrect parameters\n");
                if (ret == -2)
                    printf("ERROR : File not present\n");
                if (ret == -3)
                    printf("ERROR : Permission denied\n");
                continue;
            }
            else if (strcmp(command[0], "read") == 0)
            {
                fd = GetFDFromName(command[1]);
                if (fd == -1)
                {
                    printf("ERROR : File not present\n");
                    continue;
                }
                ptr = (char *)malloc(sizeof(atoi(command[2])) + 1);
                if (ptr == NULL)
                {
                    printf("ERROR : Memory allocation failure\n");
                    continue;
                }
                ret = ReadFile(fd, ptr, atoi(command[2]));
                if (ret == -1)
                    printf("ERROR : File not existing\n");
                if (ret == -2)
                    printf("ERROR : Permission denied\n");
                if (ret == -3)
                    printf("ERROR : Reached at end of file\n");
                if (ret == -4)
                    printf("ERROR : It is not a regular file\n");
                if (ret == 0)
                    printf("ERROR : File empty\n");
                if (ret > 0)
                {
                    write(2, ptr, ret);
                }
                continue;
            }
            else
            {
                printf("\nERROR : Command not found !!!\n");
                continue;
            }
        }
        else if (count == 4)
        {
            if (strcmp(command[0], "lseek") == 0)
            {
                fd = GetFDFromName(command[1]);
                if (fd == -1)
                {
                    printf("ERROR : Incorrect parameter\n");
                    continue;
                }
                ret = LseekFile(fd, atoi(command[2]), atoi(command[3]));
                if (ret == -1)
                {
                    printf("ERROR : Unable to perform lseek\n");
                }
            }
            else
            {
                printf("\nERROR : Command not found !!!\n");
                continue;
            }
        }
        else
        {
            continue;
        }
    }
    return 0;
}
