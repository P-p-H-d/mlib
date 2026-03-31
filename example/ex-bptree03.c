#include <stdio.h>

#include "m-bptree.h"
#include "m-string.h"

BPTREE_DEF(bpt_string_set, 8, string_t)

int main(void)
{
    // 1) Create and initialize a B+tree-based set of strings.
    bpt_string_set_t set;
    bpt_string_set_init(set);

    // 2) Insert a few values. Duplicates are ignored by set semantics.
    bpt_string_set_push(set, STRING_CTE("orange"));
    bpt_string_set_push(set, STRING_CTE("apple"));
    bpt_string_set_push(set, STRING_CTE("banana"));
    bpt_string_set_push(set, STRING_CTE("banana"));

    printf("Set size: %zu\n", bpt_string_set_size(set));

    // 3) Lookup: _get returns NULL when key is not present.
    string_t *found = bpt_string_set_get(set, STRING_CTE("apple"));
    if (found != NULL) {
        printf("Found: %s\n", string_get_cstr(*found));
    }

    // 4) Erase one element by key.
    bpt_string_set_erase(set, STRING_CTE("orange"));

    // 5) Iterate in sorted order (B+tree keeps keys ordered).
    printf("Sorted content:\n");
    bpt_string_set_it_t it;
    for (bpt_string_set_it(it, set); !bpt_string_set_end_p(it); bpt_string_set_next(it)) {
        const string_t *s = bpt_string_set_cref(it);
        printf("- %s\n", string_get_cstr(*s));
    }

    // 6) Release all memory owned by the set.
    bpt_string_set_clear(set);
    return 0;
}