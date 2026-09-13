#include <stdio.h>
#include "../file.h"

int main(int argc, char const *argv[])
{
    if(file_create("test.txt")) {
        printf("File created successfully.\n");
    } else {
        printf("Failed to create file.\n");
    }
    return 0;
}

