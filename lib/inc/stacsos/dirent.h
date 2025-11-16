#pragma once

#define MAX_FILE_NAME_LENGTH 64

struct dirent {
    char name[MAX_FILE_NAME_LENGTH];
    char type; // 'd' for directory or 'f' for file.
    unsigned int size; //Size of the file in bytes. (0 for directories).
};