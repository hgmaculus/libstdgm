#include <stdio.h>
#include "../file.h"

int main(int argc, char const *argv[])
{
    if(file_delete("test.txt")) {
        printf("File deleted successfully.\n");
    } else {
        printf("Failed to delete file.\n");
    }
    return 0;
}

