#include <stdio.h>
#include <stdbool.h>

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
    if (!fileexist(source))
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