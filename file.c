#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>

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
        fputc(63, fp);
        putchar('.');
    }
    fclose(fp);
    return true;
}
/*
    synchronize two files writting only when chars differs
*/
int file_sync(const char *sname, const char *dname){
  { // Truncate destination file to same size of source
    size_t sname_size = file_size(sname);
    if(sname_size != file_size(dname)) {
      FILE *f = fopen(dname, "w");
      if(f) fclose(f);
    }
  }
  FILE *s = fopen(sname, "r");

  if(!s) {
    return -1; // -1 source not found or can't read
  }

  FILE *d = fopen(dname, "r+");

  if(!d){
    fclose(s);
    return -2; // -2 destination not found or can't read
  } 
  {
    char a, b;
    while(!feof(s)) {
      a = fgetc(s);
      b = fgetc(d);
      if(a != b ) { // difference
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
  file_copy_oneshot do the entire file copy
  with one read and one write
  all file must be fill in memory assigned with malloc
 */
int file_copy_oneshot(const char *sname, const char *dname){
  size_t sname_size = file_size(sname);
  FILE *s = fopen(sname, "r");

  if(!s) {
    return -1; // -1 source not found or can't read
  }

  FILE *d = fopen(dname, "w");

  if(!d){
    fclose(s);
    return -2; // -2 destination not found or can't read
  } 

  char *buffer = malloc(sname_size + 1);
  if(!buffer) {
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
        if (feof(s) && feof(d))
        {
            fclose(s);
            fclose(d);
            return 0;
        }
        else
        {
            fclose(s);
            fclose(d);
            return 3;
        }
    }
}

int file_trunc_to_size(const char *filename, size_t s) {
  if(file_exist(filename)) {
    truncate(filename, s);
    return 0;
  } else {
    return 1;
  }
}

int file_insert_string(const char *filename, const off_t offset, const char *string) {
  FILE *fp = fopen(filename, "r+");
  if(!fp) return -1;
  fseek(fp, 0, SEEK_END);
  size_t s=ftell(fp); // file size
  if(s < offset) {
    fclose(fp);
    return -2;
  }
  fseek(fp, offset, SEEK_SET); // printf("Seeking file to: %ld\n", ftell(fp));
  fputs(string, fp);
  fclose(fp);
  return 0;
}