#include <stdio.h>
#include "../file.h"

int main(int argc, char const *argv[])
{
    if(file_exist("test.txt")) {
        printf("File exists.\n");
    } else {
        printf("File does not exist.\n");
    }
    return 0;
}

