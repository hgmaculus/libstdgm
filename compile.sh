clang -fPIC -c file.c -o file.o
clang -fPIC -c string.c -o string.o
clang -shared -o libstdgm.so file.o string.o