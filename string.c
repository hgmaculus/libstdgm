#include <termios.h>
#include <unistd.h>
#include <stdio.h>

// Concatena la cadena apuntada por src al final de la cadena dst
int string_concat(char *restrict dst, char *restrict src)
{
    while (*dst != '\0')
    {
        dst++;
    } // encuentra el final de la cadena
    while (*src != '\0')
    {
        *dst = *src;
        src++;
        dst++;
    }
    *dst = '\0';
    return 0;
}

// Determina el tamano de una cadena
size_t string_size(const char *restrict word)
{
    size_t count = 0;
    while (*word != '\0')
    {
        word++;
        count++;
    }
    return count;
}

int string_copy(const char *restrict src, char *restrict dst)
{
    while (*src != '\0')
    {
        *dst = *src;
        src++;
        dst++;
    }
    dst++;
    *dst = '\0';
    return 0;
}

// Encuentra en que posicion (byte) del archivo se encuentra la palabra _word_
size_t string_find(const char *restrict word, FILE *restrict fd)
{
    size_t location = 0;
    size_t ws = stringsize(word);
    size_t word_count = 0;
    char c;
    while (!feof(fd))
    {
        c = fgetc(fd);
        if (c == *word)
        {                         // primera coincidencia
            location = ftell(fd); // guarda la ubicacion de la primera coincidencia
            const char *restrict target = word;
            word_count++;
            for (size_t i = 1; i < ws; i++)
            { // iniciar busqueda de palabra completa
                c = fgetc(fd);
                target++;
                if (c == *target)
                {
                    word_count++;
                    if (word_count == ws)
                        return location;
                }
                else
                {
                    word_count = 0;
                    break; // cancela busqueda, no coincide
                }
            }
        }
    }
    return location;
}

/* lee una tecla presionada sin mostrar en la salida estandar */
int getch(void)
{
    struct termios oldattr, newattr;
    tcgetattr(STDIN_FILENO, &oldattr);
    newattr = oldattr;
    newattr.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newattr);
    int ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldattr);
    return ch;
}

/* lee una tecla presionada y la muestra en la salida estandar */
int getche(void)
{
    struct termios oldattr, newattr;
    tcgetattr(STDIN_FILENO, &oldattr);
    newattr = oldattr;
    newattr.c_lflag &= ~(ICANON);
    tcsetattr(STDIN_FILENO, TCSANOW, &newattr);
    int ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldattr);
    return ch;
}

/* Lee una secuencia de hasta n caracteres y
   lo guarda en la cadena apuntada por str, si
   se ingresa el caracter de stop sale inmediatamente
   La cadena apuntada por str debe ser mayor que n
*/
void stringgete(char *str, size_t n, char stop)
{
    struct termios oldattr, newattr;
    tcgetattr(STDIN_FILENO, &oldattr);
    newattr = oldattr;
    newattr.c_lflag &= ~(ICANON);
    tcsetattr(STDIN_FILENO, TCSANOW, &newattr);
    while (n)
    {
        *str = getchar();
        if (*str == stop)
            break;
        str++;
        n--;
    }
    *str = '\0';
    tcsetattr(STDIN_FILENO, TCSANOW, &oldattr);
}

/* Lee una secuencia de hasta n caracteres y
   lo guarda en la cadena apuntada por str, si
   se ingresa el caracter de stop sale inmediatamente
   Sin mostrar los caracteres ingresados en la pantalla,
   se usa para el ingreso de datos sensibles

*/

void string_get(char *str, size_t n, char stop)
{
    struct termios oldattr, newattr;
    tcgetattr(STDIN_FILENO, &oldattr);
    newattr = oldattr;
    newattr.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newattr);
    while (n)
    {
        *str = getchar();
        if (*str == stop)
            break;
        str++;
        n--;
    }
    *str = '\0';
    tcsetattr(STDIN_FILENO, TCSANOW, &oldattr);
}
