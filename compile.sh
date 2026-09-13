clang -Wall -c file.c -o file.o
clang -Wall -c string.c -o string.o
#clang -Wall -std=c17 -fPIC -fPIE -c main.c -o main.o
clang -Wall -shared -o libstdgm.so file.o string.o