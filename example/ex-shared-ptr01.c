#include <stdio.h>

// This example demonstrates the "opaque type" pattern: users include the
// header and manipulate a shared_pod_t, but the real pod_t definition stays in
// this .c file. The shared pointer is weak (non-atomic), which is sufficient
// for a single-threaded demonstration.
#include "ex-shared-ptr01.h"

static void test1(void)
{
  // First scenario: use the shared pointer exactly like a stack interface,
  // without exposing the real storage type.
  // Use the public API before pod_t is even defined in this file. This shows
  // that callers only need the declaration from the header.
  shared_pod_t *ptr = shared_pod_new();

  // Push the letters A..J into the opaque container.
  for(int i =0 ; i < 10; i++) {
    char c = (char) (i + 65);
    shared_pod_push(ptr, c);
  }
  printf("Shared pointer use after push: ");
  // Pop returns the most recently stored character because the private pod_t
  // implementation below behaves like a simple stack.
  for(int i =0 ; i < 10; i++) {
    char c;
    shared_pod_pop(&c, ptr);
    printf("%c ", c);
  }
  printf("\n");
  shared_pod_clear(ptr); // Can also use shared_ptr_release for this.
}

static void subtst(shared_pod_t *ptr)
{
    // Keep one extra shared reference between calls so test2 can demonstrate
    // that the object survives after the original owner releases it.
    static shared_pod_t *last_ptr = NULL;

    if (last_ptr == NULL) {
        // First call: remember the acquired reference and return immediately.
        last_ptr = ptr;
        return;
    } else {
        // Second call: ignore the input argument and reuse the saved handle.
        ptr = last_ptr;
    }

    // This function can also use the public API, even though it doesn't know
    // anything about pod_t. The shared pointer's reference counting and
    // operations work without exposing the underlying type.
    char c;
    shared_pod_pop(&c, ptr);
    printf("Subtest pop: %c\n", c);
    shared_pod_release(ptr);            // Release shared pointed now!
}

static void test2(void)
{
    // Second scenario: show the difference between owning one reference and
    // sharing the same object through multiple references.
    shared_pod_t *ptr = shared_pod_new();
    shared_pod_push(ptr, 'A');
    // Acquire creates a second owner for the same shared object.
    subtst(shared_pod_acquire(ptr));
    // Drop our local reference.
    // Since subtst still keeps one, the shared object is not destroyed yet.
    shared_pod_release(ptr);
    // Call subtst again to consume the saved reference and prove the object is
    // still alive even after the original owner released its handle.
    subtst(NULL);
}

// Private representation hidden from the header. A zero byte marks an empty
// slot, and non-zero bytes contain stored characters.
typedef struct {
  char buffer[52];
} pod_t;

static void pod_init(pod_t *p)
{
  // Start with all slots empty.
  memset(p->buffer, 0, 52);
}

static void pod_push(pod_t *p, char c)
{
  // Store into the first free slot.
  for(int i = 0; i < 52; i++) {
    if (p->buffer[i] == 0) {
      p->buffer[i] = c;
      return;
    }
  }
}

static void pod_pop(char *c, pod_t *p)
{
  // Remove from the last occupied slot, giving the container LIFO behavior.
  for(int i = 51; i >= 0; i--) {
    if (p->buffer[i] != 0) {
      *c = p->buffer[i];
      p->buffer[i] = 0;
      return;
    }
  }
}

// Bind the public shared_pod API to the private pod_t type and its hooks.
// After this macro expands, the functions declared in the header operate on
// pod_t instances without exposing pod_t itself.
SHARED_WEAK_PTR_DEF_EXTERN(shared_pod, pod_t, 
    M_OPEXTEND(M_POD_OPLIST, INIT(API_2(pod_init)), PUSH(API_2(pod_push)), FULL_P(M_FALSE_DEFAULT), POP(API(pod_pop, NONE, ARG1, ARGPTR2)), EMPTY_P(M_FALSE_DEFAULT), SUBTYPE(char)))

int main(void)
{
    test1();
    test2();
    return 0;
}
