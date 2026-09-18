#ifndef _FILE_H_
#define _FILE_H_

#include <sys/stat.h>
#include <stdbool.h>
#include <stddef.h>

bool file_delete(const char *filename);
bool file_create(const char *filename);
bool file_exist(const char *filename);
size_t file_size(const char *filename);
int file_copy_continue(const char *source, const char *destination);
bool file_splash(const char *filename);
int file_sync(const char *filename);
int file_copy_sendfile(const char *sname, const char *dname);
int file_copy_oneshot(const char *sname, const char *dname);
int file_compare(const char *sname, const char *dname);
int file_trunc_to_size(const char *filename, size_t s);
int file_insert(const char *filename, const char *data, size_t position);
int file_detect_fs(const char *filename);
int file_to_stdout(const char *filename, size_t delay);
mode_t file_get_permissions(const char *filename);
int file_set_permissions(const char *filename, mode_t perms);
int file_copy_perms(const char *src, const char *dst);
int file_compare_perms(const char *src, const char *dst);
bool mkdir_recursive(const char *path, mode_t mode);

#endif // _FILE_H


