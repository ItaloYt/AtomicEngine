#include "atomic/util.h"
#include <stdio.h>

#pragma weak log_file
FILE *log_file = 0x0;

enum AtomicError create_log_file(const char *path) 
{
    log_file = fopen(path, "w");

    if(!log_file) return CREATE_ERROR_FILE_ERROR;

    return SUCCESS;
}

enum AtomicError close_log_file()
{
    if(!log_file) return SUCCESS;

    (void) fclose(log_file);

    return SUCCESS;
}
