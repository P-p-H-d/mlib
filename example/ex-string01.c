#include <stdio.h>
#include <string.h>

#include "m-string.h"

#define MAX_COLUMN 80

/* M*LIB format program: 
    read a text file and reformat it so that lines are at most MAX_COLUMN characters long,
    using '\' as a continuation marker. The input file is expected to be a C header file, 
    and the output is intended to be a reformatted version of the same header file. 
    The program reads each line of the input file, 
    checks if it ends with a continuation marker ('\'), and if so, 
    it pads the line with spaces to align the continuation markers visually near MAX_COLUMN. 
    If a line exceeds MAX_COLUMN characters 
    and ends with a continuation marker, it trims trailing spaces
    and ensures that there is exactly one space before the continuation marker. 
    Finally, it prints the reformatted lines to standard output. */
static void format(FILE *f)
{
    string_t s;
    string_init(s);
    // Read input one logical line at a time into an mlib string.
    // We will discard the final \n character too.
    while (string_fgets(s, f, STRING_READ_PURE_LINE) == true)
    {
        size_t n = string_size(s);
        // If a line already ends with '\\' and is still short, pad it so
        // continuations align visually near MAX_COLUMN.
        if (n > 0 && string_get_char(s, n-1) == '\\' && n < MAX_COLUMN) {
            string_left(s, n-1);
            while (n < MAX_COLUMN-1) {
                string_push_back(s, ' ');
                n++;
            }
            string_push_back(s, '\\');
        }
        // If the continuation marker is too far to the right, pull it back:
        // trim trailing spaces, then keep exactly one space before '\\'.
        if (n >= MAX_COLUMN && string_get_char(s, n-1) == '\\') {
            n--;
            string_left(s, n);
            while (n >= MAX_COLUMN-2 && string_get_char(s, n-1) == ' ') {
                n--;
                string_left(s, n);
            }
            string_push_back(s, ' ');
            string_push_back(s, '\\');
        }
        printf("%s\n", string_get_cstr(s));
    }
    string_clear(s);
}

int main(int argc, const char *argv[])
{
    // Expect exactly one input file path.
    if (argc < 2)
    {
        fprintf(stderr, "ERROR: Missing argument filename.\nUSAGE: %s filename.h\n", argv[0]);
        exit(1);
    }
    // Open in text mode and reformat each line before printing to stdout.
    FILE *f = fopen(argv[1], "rt");
    if (!f) {
        fprintf(stderr, "ERROR: Cannot open filename '%s' .h\n", argv[1]);
        exit(2);
    }
    format(f);
    fclose(f);
    exit(0);
}
