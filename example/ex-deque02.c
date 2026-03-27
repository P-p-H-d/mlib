#include <stdio.h>
#include "m-deque.h"
#include "m-string.h"

/* Create a deque of strings */
DEQUE_DEF(deque_str, m_string_t)
/* Register the oplist for the deque of strings by associating its type to its oplist */
#define M_OPL_deque_str_t() DEQUE_OPLIST(deque_str, M_STRING_OPLIST)

int main(void)
{
    /*
     * M_LET manages lifetime automatically:
     * - `str` is a reusable temporary string
     * - `deque` stores copies of strings and is cleared at scope exit
     * You can also use M_LET for individual variables, but here we group them for better readability.
     * If you prefer, you can also use _init and _clear methods explicitly without M_LET, but then you need to manage the lifetime of each variable manually.
     */
    M_LET(str, m_string_t)
    M_LET(deque, deque_str_t) {
        /* Build 10 strings and push each one at the back of the deque. */
        for (int i = 0; i < 10; i++) {
            m_string_printf(str, "String %d", i);
            deque_str_push_back(deque, str);
        }

        /* Random-access read with deque_str_get(index). */
        for (size_t i = 0; i < deque_str_size(deque); i++) {
            printf("%s\n", m_string_get_cstr(*deque_str_get(deque, i)));
        }

        /* Pop from both ends and move removed values into `first` and `last`. */
        M_LET(first, last, m_string_t) {
            deque_str_pop_front(&first, deque);
            deque_str_pop_back(&last, deque);
            printf("Popped front: %s\n", m_string_get_cstr(first));
            printf("Popped back: %s\n", m_string_get_cstr(last));
        }

        /*
         * Emplace constructs elements directly in the deque from C strings,
         * so no temporary m_string_t variable is needed for these inserts.
         */
        deque_str_emplace_front(deque, "First (emplace)");
        deque_str_emplace_back(deque, "Last (emplace)");

        printf("\nDeque after pop_* and emplace_*:\n");
        for (size_t i = 0; i < deque_str_size(deque); i++) {
            printf("%s\n", m_string_get_cstr(*deque_str_get(deque, i)));
        }
        
    }
    return 0;
}
