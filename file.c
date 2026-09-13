#define _FILE_OFFSET_BITS 64

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/statfs.h>
#include <sys/sendfile.h>
#include <linux/magic.h>


// Delete a file with the given filename
bool file_delete(const char *filename)
{
  if (remove(filename) == 0)
  {
    return true;
  }
  else
  {
    return false;
  }
}

// Create a new file with the given filename
bool file_create(const char *filename)
{
  FILE *f = fopen(filename, "w");
  if (f)
  {
    fclose(f);
    return true;
  }
  else
  {
    return false;
  }
}

// return false if file not exist, or true if file exists
bool file_exist(const char *filename)
{
  FILE *f = fopen(filename, "r");
  if (f)
  {
    fclose(f);
    return true;
  }
  else
  {
    return false;
  }
}

/*
Return the size of a file in bytes, or 0 if the file does not exist
*/
size_t file_size(const char *filename)
{
  FILE *f = fopen(filename, "r");
  if (!f)
  {
    fprintf(stderr, "filesize(): file not found %s\n", filename);
    return 0;
  }

  fseek(f, 0, SEEK_END);
  size_t fsize = ftell(f);
  fclose(f);
  return fsize;
}
/*
Copy source file to destination file or continue incomplete copy
*/
int file_copy_continue(const char *source, const char *destination)
{
  if (!file_exist(source))
  {
    return -1;
  }
  FILE *fd, *fs;
  if (file_exist(destination))
  { // continue previous copy
    if (file_size(source) == file_size(destination))
    {
      return 0; // files are equal
    }
    fd = fopen(destination, "a");
    if (fd == NULL)
      return 1;
  }
  else
  {
    fd = fopen(destination, "w"); // new file and new copy
    if (fd == NULL)
    {
      return 2;
    }
  }
  fs = fopen(source, "r");
  if (fs == NULL)
  {
    fclose(fd);
    return 3;
  }
  long position = ftell(fd);
  fseek(fs, position, SEEK_SET);
  int c;
  while (!feof(fs))
  {
    c = fgetc(fs);
    if (c != EOF)
      fputc(c, fd);
  }

  fclose(fs);
  fclose(fd);
  return 0;
}
/*
write ? char in every byte of file
*/
bool file_splash(const char *restrict filename)
{
  FILE *fp = fopen(filename, "r+");
  if (!fp)
    return false;
  fseek(fp, 0, SEEK_END);
  size_t sz = ftell(fp);
  fseek(fp, 0, SEEK_SET);
  for (size_t t = sz; t != 0; t--)
  {
    fputc(63, fp); // write '?' char in every byte of file
    putchar('.');  // show progress
  }
  fclose(fp);
  return true;
}
/*
    synchronize two files writting only when chars differs
*/
int file_sync(const char *sname, const char *dname)
{
  { // Truncate destination file to same size of source
    size_t sname_size = file_size(sname);
    if (sname_size != file_size(dname))
    {
      FILE *f = fopen(dname, "w");
      if (f)
        fclose(f);
    }
  }
  FILE *s = fopen(sname, "r");

  if (!s)
  {
    return -1; // -1 source not found or can't read
  }

  FILE *d = fopen(dname, "r+");

  if (!d)
  {
    fclose(s);
    return -2; // -2 destination not found or can't read
  }
  {
    char a, b;
    while (!feof(s))
    {
      a = fgetc(s);
      b = fgetc(d);
      if (a != b)
      { // difference
        printf("Fixing difference at: %ld\n", ftell(s));
        fseek(d, -1, SEEK_CUR);
        fputc(a, d);
      }
    }
  }
  fclose(s);
  fclose(d);
  return 0;
}

/*
Copy source file to destination file using sendfile system call (in kernel space)
*/
int file_copy_sendfile(const char *sname, const char *dname)
{
  size_t sname_size = file_size(sname);
  FILE *s = fopen(sname, "r");

  if (!s)
  {
    return -1; // -1 source not found or can't read
  }

  FILE *d = fopen(dname, "w");

  if (!d)
  {
    fclose(s);
    return -2; // -2 destination not found or can't read
  }

  off_t offset = 0;
  ssize_t sent_bytes = sendfile(fileno(d), fileno(s), &offset, sname_size);

  if (sent_bytes == -1)
  {
    fclose(s);
    fclose(d);
    return -3; // sendfile failed
  }

  fclose(s);
  fclose(d);
  return 0;
}

/*
  file_copy_oneshot do the entire file copy
  with one read and one write
  all file must be fill in memory assigned with malloc
 */
int file_copy_oneshot(const char *sname, const char *dname)
{
  size_t sname_size = file_size(sname);
  FILE *s = fopen(sname, "r");

  if (!s)
  {
    return -1; // -1 source not found or can't read
  }

  FILE *d = fopen(dname, "w");

  if (!d)
  {
    fclose(s);
    return -2; // -2 destination not found or can't read
  }

  char *buffer = malloc(sname_size + 1);
  if (!buffer)
  {
    fclose(s);
    fclose(d);
    return -3; // malloc failed
  }

  fread(buffer, sname_size, 1, s);
  fwrite(buffer, sname_size, 1, d);

  free(buffer);
  fclose(s);
  fclose(d);
  return 0;
}
/*
file_compare return 0 if two files are equals
otherwise return:
1 first file not found
2 second file not found
3 files differ
*/
int file_compare(const char *sname, const char *dname)
{
  FILE *s = fopen(sname, "r");

  if (!s)
    return 1; // 1 source not found or can't read

  FILE *d = fopen(dname, "r");

  if (!d)
  {
    fclose(s);
    return 2; // 2 destination not found or can't read
  }

  {
    char sa, db;
    while (!feof(s) && !feof(d))
    {
      sa = fgetc(s);
      db = fgetc(d);
      if (sa != db)
        break;
    }
    if (feof(s) && feof(d) && sa == db)
    {
      fclose(s);
      fclose(d);
      return 0; // files are equal
    }
    else
    {
      fclose(s);
      fclose(d);
      return 3; // files differ
    }
  }
}

int file_trunc_to_size(const char *filename, size_t s)
{
  if (file_exist(filename))
  {
    truncate(filename, s);
    return 0;
  }
  else
  {
    return 1;
  }
}

int file_insert_string(const char *filename, const off_t offset, const char *string)
{
  FILE *fp = fopen(filename, "r+");
  if (!fp)
    return -1;
  fseek(fp, 0, SEEK_END);
  size_t s = ftell(fp); // file size
  if (s < offset)
  {
    fclose(fp);
    return -2;
  }
  fseek(fp, offset, SEEK_SET); // printf("Seeking file to: %ld\n", ftell(fp));
  fputs(string, fp);
  fclose(fp);
  return 0;
}

int file_detect_fs(const char *filename)
{

  struct statfs fsinfo;
  if (statfs(filename, &fsinfo) != 0)
  {
    return -1; // error
  }

  switch (fsinfo.f_type)
  {
  case BTRFS_SUPER_MAGIC:
    return 1; // BTRFS
    break;

  case XFS_SUPER_MAGIC:
    return 2; // XFS
    break;

  case EXT4_SUPER_MAGIC:
    return 3; // EXT4
    break;

  case F2FS_SUPER_MAGIC:
    return 4; // F2FS
    break;

  default:
    return 0; // unknown
    break;
  }

  return 0; // success
}

int file_to_stdout(const char *filename, size_t delay)
{
  FILE *fp = fopen(filename, "r");
  if (!fp)
    return -1;

  int c;
  while ((c = fgetc(fp)) != EOF)
  {
    fputc(c, stdout);
    usleep(delay);
    fflush(stdout);
  }

  fclose(fp);
  return 0;
}

// Get the permissions of a file
// Return the permissions as a mode_t value, or (mode_t)-1 on error
mode_t file_get_perms(const char *filename)
{
  struct stat st;
  if (stat(filename, &st) != 0)
  {
    perror("file_get_perms: stat failed");
    return (mode_t)-1;
  }
  return st.st_mode;
}

// Set the permissions of a file
// Return 0 on success, -1 on error
int file_set_perms(const char *filename, mode_t perms)
{
  if (chmod(filename, perms) != 0)
  {
    perror("file_set_perms: chmod failed");
    return -1;
  }
  return 0;
}

// Copy permissions from src file to dst file
// Return 0 on success, -1 on error
int file_copy_perms(const char *src, const char *dst)
{
  struct stat st;
  if (stat(src, &st) != 0)
  {
    perror("src stat failed");
    return -1;
  }
  if (chmod(dst, st.st_mode) != 0)
  {
    perror("dst chmod failed");
    return -2;
  }
  return 0;
}

// Check if two files have the same permissions
// Return 0 if they have the same permissions, 1 if they differ, and -1 on error
int file_compare_perms(const char *src, const char *dst)
{
  struct stat src_stat, dst_stat;
  if (stat(src, &src_stat) != 0)
  {
    perror("src stat failed");
    return -1;
  }

  if (stat(dst, &dst_stat) != 0)
  {
    perror("dst stat failed");
    return -2;
  }

  if (src_stat.st_mode != dst_stat.st_mode)
  {
    return 1;
  }
  return 0;
}

bool mkdir_recursive(const char *path, mode_t mode_create)
{
  const char *init = path;
  const char *start = path;
  const char *end;

  char dir_to_create[4096];
  while ((end = strchr(start, '/')) != NULL)
  {
    sprintf(dir_to_create, "%.*s", (int)(end - init), init);
    /*
    this progressively creates directories for each level in the path, for example:
    mkdir("first_level_dir")
    mkdir("first_level_dir/second_level_dir")
    mkdir("first_level_dir/second_level_dir/third_level_dir")
    */
    // printf("mkdir(%s)\n", dir_to_create);
    if (mkdir(dir_to_create, mode_create) == -1)
    {
      perror("mkdir_recursive(): failed");
    }
    start = end++; // Move past the '/' and increment end to point to the next character
  }

  // Print the last component if there is one, file.txt
  /*
  if (*start != '\0')
  {
      printf("filename: %s\n", start);
  }
  */
  return true;
}