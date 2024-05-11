#include <stdlib.h>

#include "m-bptree.h"

BPTREE_MULTI_DEF2(multimap, 5, int, M_BASIC_OPLIST, int, M_BASIC_OPLIST)
#define M_OPL_map_multimap_t() BPTREE_OPLIST2(multimap, M_BASIC_OPLIST, M_BASIC_OPLIST)

// Fill in with multiple value
static void
fill_in(multimap_t map)
{
    for(int i = 1 ; i < 100; i++)
        multimap_set_at(map, i, i);
    for(int i = 1 ; i < 100; i+=2)
        multimap_set_at(map, i, i*i);
    for(int i = 1 ; i < 100; i+=3)
        multimap_set_at(map, i, i*i*i);
}

// Count the number of occurence of a key
static int
count(multimap_t map, int key)
{
    int n = 0;
    multimap_it_t it;
    // it_from is O(n*ln n) complexity
    // it_while_p and next are O(1) complexity
    for(multimap_it_from(it, map, key); multimap_it_while_p(it, key); multimap_next(it))
        n++;
    return n;    
}

// Print the map in details
static void
print(multimap_t map)
{
    for(int i = 0; i < 101;i++) {
        printf("Key=%d Count=%d ", i, count(map, i));
        multimap_it_t it;
        for(multimap_it_from(it, map, i); multimap_it_while_p(it, i); multimap_next(it)) {
            printf("Value=%d ", *multimap_ref(it)->value_ptr );
        }
        printf("\n");
    }
}

int main(void)
{
    multimap_t map;
    multimap_init(map);
    fill_in(map);
    print(map);
    multimap_clear(map);
    exit(0);
}
