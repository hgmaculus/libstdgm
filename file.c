#include <stdio.h>
#include <stdbool.h>

// return false if file not exist, or true if file exists
bool fileexist(const char *filename) {
  FILE *f = fopen(filename, "r");
  if(f) {
    fclose(f);
    return true;
  } else {
    return false;
  }
}

size_t filesize(const char *filename) {
  FILE *f = fopen(filename, "r");
  if(!f) {
    fprintf(stderr, "filesize(): file not found %s\n", filename);
    return 0;
  }
  
  fseek(f, 0, SEEK_END);
  size_t fsize = ftell(f);
  fclose(f);
  return fsize;
}
