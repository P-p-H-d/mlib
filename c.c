#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*
fscanf  --> %c, %s, %[ have additional parameters (size) MS is unsigned. Std is rsize_t
fprintf --> No %n
snprintf --> No %n
vsnprintf --> No %n, MS prototype incompatible with Standard ! (additional arg) can be added automatically ?
qsort --> Nothing ?
memcpy --> Additional arg buffer alloc
memmove--> Additional arg buffer alloc
strncpy--> Additional arg buffer alloc
*/

/* If Microsoft Visual Studio C Library
 * ==> Use Annex K like functions to avoid warnings
 */
#if defined(_MSC_VER) && defined(__STDC_WANT_LIB_EXT1__) && __STDC_WANT_LIB_EXT1__

#define m_fprintf(...) fprintf_s(__VA_ARGS__)
#define m_snprintf(...) snprintf_s(__VA_ARGS__)
#define m_qsort(...) qsort_s(__VA_ARGS__)
#define m_vsnprintf(s, n, ...) vsnprintf_s(s, n, (n)-1, __VA_ARGS__)
#define m_memcpy(d, s, size) memcpy_s(d, size, s, size)
#define m_memmove(d, s, size) memmove_s(d, size, s, size)
#define m_strncpy(s1, s2, size) strncpy_s(s1, size, s2, size)
#define m_fscanf(...) fscanf_s(__VA_ARGS__)
#define m_fscanf_strarg(arg, size) arg, size

#else

/* Use classic C functions */
#define m_fprintf(...) fprintf(__VA_ARGS__)
#define m_snprintf(...) snprintf(__VA_ARGS__)
#define m_qsort(...) qsort(__VA_ARGS__)
#define m_vsnprintf(...) vsnprintf(__VA_ARGS__)
#define m_memcpy(...) memcpy(__VA_ARGS__)
#define m_memmove(...) memmove(__VA_ARGS__)
#define m_fscanf(...) fscanf(__VA_ARGS__)
#define m_fscanf_strarg(arg, size) arg

#endif

void PrintFError ( const char * format, ... )
{
  char buffer[256];
  va_list args;
  va_start (args, format);
  m_vsnprintf (buffer,256,format, args);
  m_fprintf (stderr, "%s", buffer);
  va_end (args);
}

int main(void)
{
    m_fprintf(stdout, "This is a test %d!\n", 8);
    char buffer[100];
    m_snprintf(buffer, sizeof buffer, "%d", 18);
    m_fprintf(stdout, "%s. Please enter your name:\n", buffer);
    m_fscanf(stdin, " %99s", m_fscanf_strarg(buffer, sizeof buffer));    
    char buffer2[100];
    m_memcpy(buffer2, buffer, sizeof buffer);
    char buffer3[100];
    m_memmove(buffer3, buffer2, sizeof buffer3);
    PrintFError("There is an error, %s\n", buffer3);
    return 0;
}
