#include <stdio.h>
#include "m-bitset.h"

static void print_bitset(const char *label, m_bitset_t bitset)
{
    printf("%s", label);
    for (size_t i = 0; i < m_bitset_size(bitset); i++) {
        printf("%d ", m_bitset_get(bitset, i));
    }
    printf("\n");
}

int main(void)
{
    /* m_bitset_t is a stack-allocated container that must be initialized first. */
    m_bitset_t bitset;
    m_bitset_init(bitset);

    /* Grow the bitset to 10 bits. New bits are initialized to 0. */
    m_bitset_resize(bitset, 10);

    /* Set a few individual bits to 1. Indexing starts at 0. */
    m_bitset_set_at(bitset, 3, true);
    m_bitset_set_at(bitset, 5, true);
    m_bitset_set_at(bitset, 7, true);

    print_bitset("Bitset: ", bitset);

    /* Update one bit back to 0 without changing the size of the bitset. */
    m_bitset_set_at(bitset, 5, false);
    print_bitset("After clearing bit 5: ", bitset);

    /* Push, insert, flip and pop operations can also change the bitset. */
    m_bitset_push_back(bitset, true);
    m_bitset_push_at(bitset, 2, true);
    m_bitset_flip_at(bitset, 0);
    print_bitset("After push_back/push_at/flip_at: ", bitset);

    bool removed = false;
    bool last = false;
    m_bitset_pop_at(&removed, bitset, 4);
    m_bitset_pop_back(&last, bitset);
    printf("Removed bit at index 4: %d\n", removed);
    printf("Removed last bit: %d\n", last);
    print_bitset("After pop_at/pop_back: ", bitset);

    /* Iterators let you walk through the bitset without dealing with indexes. */
    printf("Forward iterator: ");
    m_bitset_it_t it;
    for (m_bitset_it(it, bitset); !m_bitset_end_p(it); m_bitset_next(it)) {
        printf("%d ", *m_bitset_cref(it));
    }
    printf("\n");

    printf("Backward iterator: ");
    for(m_bitset_it_last(it, bitset); !m_bitset_last_p(it); m_bitset_previous(it)) {
        printf("%d ", *m_bitset_cref(it));
    }
    printf("\n");

    /* Text helpers make it easy to save or rebuild a bitset from a string. */
    m_bitset_t parsed;
    m_bitset_init(parsed);
    if (m_bitset_parse_str(parsed, "[101001]", NULL)) {
        print_bitset("Parsed from string: ", parsed);
    }
    if (m_bitset_set_str(parsed, "[111000]")) {
        print_bitset("After set_str: ", parsed);
    }

    /* The same text format can be written to and read back from a FILE. */
    FILE *file = tmpfile();
    if (file == NULL) {
        fprintf(stderr, "Cannot create temporary file for bitset I/O demo.\n");
        m_bitset_clear(parsed);
        m_bitset_clear(bitset);
        return 1;
    }
    m_bitset_out_str(file, bitset);
    rewind(file);
    if (m_bitset_in_str(parsed, file)) {
        print_bitset("Round-trip through FILE: ", parsed);
    }
    fclose(file);
    
    /* Release any memory owned by the bitset before leaving. */
    m_bitset_clear(parsed);
    m_bitset_clear(bitset);
    return 0;
}