# Custom FAT File System -- README

## Overview

This project implements a simplified **File Allocation Table
(FAT)**-based file system inside a virtual disk (`virtual_disk.bin`).\
The user interacts through a custom shell to perform file and directory
operations.\
All operations work on a **virtual 64MB disk** using **1KB blocks**,
with FAT managing block allocation.

------------------------------------------------------------------------

## Specifications (As Required)

### **1. Basic Settings**

  Setting          Value
|-----------------|-------------------|
| Directory Size  | 128 entries       |
| Max File Name   | 64 bytes          |
| Max File Size   | 128 blocks        |
| Block Size      | 1KB               |
| Disk Size       | 64MB              |
| Documentation   | Included in code  |
---------------------------------------

## **2. Required Operations**

### **A. File Operations**

-   **Create a file**
-   **Delete a file**
-   **Read a file**
-   **Write to a file**
-   **Truncate a file**

These operations must use: - FAT block allocation - Block-level
read/write only - Directory entry updates

------------------------------------------------------------------------

### **B. Directory Operations**

-   **Create a directory**
-   **Delete a directory**
-   **List all files/directories**

Directories are treated similarly to files: - Each directory has its own
table (128 entries) - Has a starting FAT block - Stored on disk like
file data

------------------------------------------------------------------------

### **C. Partition Operation**

-   **Create & Format Partition**

Formatting must: 1. Create a 64MB disk file\
2. Initialize FAT (65536 entries)\
3. Initialize root directory (128 entries)\
4. Mark all blocks as FREE except FAT + root\
5. Allow system to be ready for CRUD operations

------------------------------------------------------------------------

### **D. Low-Level Functions**

These must be used for ALL disk operations:

    ReadBlock(blockNumber, buffer)
    WriteBlock(blockNumber, buffer)

No direct file writes. All operations must use these functions to
simulate a real block device.

------------------------------------------------------------------------

## **3. Additional Required Features**

-   Maintain FAT table
-   Maintain directory table
-   Provide a command-line shell interface
-   Implement only block-based I/O operations

------------------------------------------------------------------------

## **4. Bonus Features**

  -----------------------------------------------------------------------
  Bonus Feature                          Description
  -------------------------------------- --------------------------------
  Variable File Name Size                Allow up to 64 bytes dynamically

  Parameterized Settings                 User can specify block size,
                                         disk size, FAT size

  Encrypted Partition                    Disk blocks are encrypted before
                                         writing
  -----------------------------------------------------------------------

------------------------------------------------------------------------

## **5. Expected Folder Structure**

    /project
      main.cpp
      fat.h / fat.cpp
      directory.h / directory.cpp
      shell.cpp
      virtual_disk.bin  (created after format)
      README.md

------------------------------------------------------------------------

## **6. How the File System Works (Conceptually)**

### **Disk Layout**

    Block 0                 → Boot / Reserved
    Block 1 – FAT_END       → FAT Table (256 blocks)
    Block FAT_END+1 – ...   → Root Directory (10 blocks)
    Remaining Blocks        → File & Directory Data

### **FAT Values**

  Value   Meaning
  ------- ------------------
  -1      FREE
  -2      END of chain
  n       next block index

------------------------------------------------------------------------

## **7. Shell Example**

    > format
    > mkdir docs
    > cd docs
    > create notes.txt
    > write notes.txt
    > read notes.txt
    > ls
    > truncate notes.txt 20
    > delete notes.txt
    > exit

------------------------------------------------------------------------

## **8. Submission Requirements**

-   Source code
-   README (this file)
-   Explanation of approach
-   Mention of any bonus features completed

------------------------------------------------------------------------

## **9. Conclusion**

This assignment teaches core concepts of file system design: - FAT
structure - Block allocation - Directory management - Low-level disk
I/O - Shell interfaces

Completing this fully demonstrates strong understanding of operating
systems internals.
