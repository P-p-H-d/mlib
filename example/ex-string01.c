#include <stdio.h>
#include <string.h>

#include "m-string.h"

#define MAX_COLUMN 80

static void format(FILE *f)
{
    string_t s;
    string_init(s);
    while (M_F(string, fgets)(s, f, STRING_READ_PURE_LINE) == true)
    {
        size_t n = string_size(s);
        if (n > 0 && M_F(string, get_char)(s, n-1) == '\\' && n < MAX_COLUMN) {
            string_left(s, n-1);
            while (n < MAX_COLUMN-1) {
                M_F(string, push_back)(s, ' ');
                n++;
            }
            M_F(string, push_back)(s, '\\');
        }
        if (n >= MAX_COLUMN && M_F(string, get_char)(s, n-1) == '\\') {
            n--;
            string_left(s, n);
            while (n >= MAX_COLUMN-2 && M_F(string, get_char)(s, n-1) == ' ') {
                n--;
                string_left(s, n);
            }
            M_F(string, push_back)(s, ' ');
            M_F(string, push_back)(s, '\\');
        }
        printf("%s\n", M_F(string, get_cstr)(s));
    }
    string_clear(s);
}

int main(int argc, const char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "ERROR: Missing argument filename.\nUSAGE: %s filename.h\n", argv[0]);
        exit(1);
    }
    FILE *f = m_core_fopen(argv[1], "rt");
    if (!f) {
        fprintf(stderr, "ERROR: Cannot open filename '%s' .h\n", argv[1]);
        exit(2);
    }
    format(f);
    fclose(f);
    exit(0);
}
