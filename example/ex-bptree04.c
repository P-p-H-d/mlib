#include <stdio.h>

#include "m-bptree.h"
#include "m-string.h"

BPTREE_MULTI_DEF(bpt_string_set, 8, string_t)

int main(void)
{
    // Create and initialize a multiset of strings backed by a B+Tree.
    bpt_string_set_t set;
    bpt_string_set_init(set);

    // Insert values, including duplicates (allowed in MULTI mode).
    bpt_string_set_push(set, STRING_CTE("apple"));
    bpt_string_set_push(set, STRING_CTE("orange"));
    bpt_string_set_push(set, STRING_CTE("apple"));
    bpt_string_set_push(set, STRING_CTE("banana"));
    bpt_string_set_push(set, STRING_CTE("apple"));

    printf("Total items in multiset: %zu\n", bpt_string_set_size(set));

    // Count how many times "apple" appears using a key-bounded iterator.
    size_t apple_count = 0;
    bpt_string_set_it_t it;
    for (bpt_string_set_it_from(it, set, STRING_CTE("apple"));
         bpt_string_set_it_while_p(it, STRING_CTE("apple"));
         bpt_string_set_next(it)) {
        apple_count++;
    }
    printf("Occurrences of 'apple': %zu\n", apple_count);

    // Iterate over all items in sorted order (duplicates are kept).
    printf("Sorted multiset content:\n");
    for (bpt_string_set_it(it, set); !bpt_string_set_end_p(it); bpt_string_set_next(it)) {
        const string_t *s = bpt_string_set_cref(it);
        printf("- %s\n", string_get_cstr(*s));
    }

    // Always clear the container to release allocated memory.
    bpt_string_set_clear(set);
    return 0;
}
