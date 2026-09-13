#ifndef _FILE_H_
#define _FILE_H_

#include <stdbool.h>
#include <stddef.h>

bool file_delete(const char *filename);
bool file_create(const char *filename);
bool file_exist(const char *filename);
size_t file_size(const char *filename);
int file_copy_continue(const char *source, const char *destination);
bool file_splash(const char *restrict filename);

#endif // _FILE_H


