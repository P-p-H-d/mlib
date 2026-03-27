#include <stdio.h>

#include "m-bstring.h"

int main(void)
{
    /* bstring_t is a dynamic byte buffer (not a text string). */
    bstring_t b;
    bstring_init(b);

    /* Push 10 single bytes: 00, 01, ... 09. */
    for(int i = 0 ; i < 10; i++) {
        /* Unlike C strings, a byte string can safely store zero bytes. */
        bstring_push_back(b, i);
    }

    /* Push a full raw byte array in one call. */
    unsigned char tab[] = { 14, 26, 28, 98, 1};
    bstring_push_back_bytes(b, sizeof(tab), tab);

    /* Inspect content as hexadecimal values. */
    printf("After push back: ");
    for(size_t i = 0; i < bstring_size(b); i++) {
        printf("%02x ", bstring_get_byte(b, i));
    }
    printf("\n");
    printf("Size of byte string is %zu\n", bstring_size(b));

    /*
     * Pop 10 bytes from the front into tmp.
     * This both copies removed bytes to tmp and shrinks the source bstring.
     */
    unsigned char tmp[10];
    bstring_pop_front_bytes(10, tmp, b);
    printf("tmp = ");
    for(size_t i = 0; i < 10; i++) {
        printf("%02x ", tmp[i]);
    }
    printf("\n");

    printf("After pop front: ");
    for(size_t i = 0; i < bstring_size(b); i++) {
        printf("%02x ", bstring_get_byte(b, i));
    }
    printf("\n");
    printf("Size of byte string is %zu\n", bstring_size(b));

    /* Release memory owned by the byte string. */
    bstring_clear(b);
    return 0;
}
