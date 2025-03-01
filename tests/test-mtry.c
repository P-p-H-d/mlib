/*
 * Copyright (c) 2017-2025, Patrick Pelissier
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * + Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * + Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <stdio.h>
#include <stdbool.h>

#ifdef __GNUC__
#if __GNUC__ >= 12
  /* Issue with GCC 12. It reports a variable as being not initialized
     whereas it is obviously initialized.
     See also https://gcc.gnu.org/bugzilla/show_bug.cgi?id=109530
  */
  _Pragma("GCC diagnostic push")
  _Pragma("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
#endif
#endif

# if !defined(__cplusplus)
// The test shall finish with a raise fatal.
#define M_RAISE_FATAL(...) do {                                         \
    if (test_final_in_progress == true) {                               \
      exit(0);                                                          \
    } else {                                                            \
      fprintf(stderr, "ERROR(M*LIB): " __VA_ARGS__);                    \
      abort();                                                          \
    }                                                                   \
  } while (0)

// Is false by default.
static bool test_final_in_progress;
#endif

#include "test-obj.h"
#include "coverage.h"

#include "m-try.h"

M_TRY_DEF_ONCE()

static void test_throw(void)
{
  M_TRY(protect) {
    M_THROW(67, 56);
  } M_CATCH(protect, 67) {
    assert(protect->num == 1);
    M_THROW(M_ERROR_MEMORY, protect->context[0], 4, 6, 8);
  }
}

// Test of try / catch with throw.
// No memory allocation
static void test1(void)
{
  volatile int flow = 0;

  flow = 0;
  assert(flow ++ == 0);
  M_TRY(test1) {
    assert(flow ++ == 1);
  } M_CATCH(test1, M_ERROR_MEMORY) {
    assert(0);
  }
  assert(flow ++ == 2);

  flow = 0;
  assert(flow ++ == 0);
  M_TRY(test1) {
    assert(flow ++ == 1);
    M_THROW(M_ERROR_MEMORY, 0);
    assert(0);
  } M_CATCH(test1, M_ERROR_MEMORY) {
    assert(flow++ == 2);
  }
  assert(flow ++ == 3);

  flow = 0;
  assert(flow ++ == 0);
  M_TRY(test1) {
    assert(flow ++ == 1);
    M_TRY(test2) {
      assert(flow ++ == 2);
      M_THROW(189, 0);
      assert(0);
    } M_CATCH(test2, 189) {
      assert(flow++ == 3);
    }
  } M_CATCH(test1, M_ERROR_MEMORY) {
    assert(0);
  }
  assert(flow ++ == 4);
  
  flow = 0;
  assert(flow ++ == 0);
  M_TRY(test1) {
    assert(flow ++ == 1);
    M_TRY(test2) {
      assert(flow ++ == 2);
      M_TRY(test3) {
        assert(flow ++ == 3);
      } M_CATCH(test3, 67) {}
      assert(flow ++ == 4);
      M_TRY(test4) {
        assert(flow ++ == 5);
        M_THROW(M_ERROR_ACCESS, 12);
        assert(0);
      } M_CATCH(test4, 67) {}
      assert(0);
    } M_CATCH(test2, M_ERROR_ACCESS) {
      assert(flow++ == 6);
      assert(test2->num == 1);
      assert(test2->context[0] == 12);
    }
  } M_CATCH(test1, M_ERROR_MEMORY) {
    assert(0);
  }
  assert(flow ++ == 7);

  flow = 0;
  assert(flow ++ == 0);
  M_TRY(test1) {
    assert(flow ++ == 1);
    M_TRY(test2) {
      assert(flow ++ == 2);
      M_TRY(test3) {
        assert(flow ++ == 3);
      } M_CATCH(test3, 42) {
        assert(0);
      }
      assert(flow ++ == 4);
      M_TRY(test4) {
        assert(flow ++ == 5);
        M_THROW(42, 1, 3);
        assert(0);
      } M_CATCH(test4, 67) {}
      assert(0);
    } M_CATCH(test2, M_ERROR_ACCESS) {
      assert(0);
    }
  } M_CATCH(test1, M_ERROR_MEMORY) {
    assert(0);
  }  M_CATCH(test1, 42) {
    assert(flow++ == 6);
    assert(test1->num == 2);
    assert(test1->context[0] == 1);
    assert(test1->context[1] == 3);
  }
  assert(flow ++ == 7);

  flow = 0;
  assert(flow ++ == 0);
  M_TRY(test1) {
    assert(flow ++ == 1);
    M_TRY(test2) {
      assert(flow ++ == 2);
      M_TRY(test3) {
        assert(flow ++ == 3);
      } M_CATCH(test3, 42) {
        assert(0);
      }
      assert(flow ++ == 4);
      M_TRY(test4) {
        assert(flow ++ == 5);
      } M_CATCH(test4, 67) {}
      assert(flow ++ == 6);
    } M_CATCH(test2, M_ERROR_ACCESS) {
      assert(0);
    }
  } M_CATCH(test1, M_ERROR_MEMORY) {
    assert(0);
  }  M_CATCH(test1, 42) {
    assert(0);
  }
  assert(flow ++ == 7);

  flow = 0;
  assert(flow ++ == 0);
  M_TRY(test1) {
    assert(flow ++ == 1);
    M_TRY(test2) {
      assert(flow ++ == 2);
      test_throw();
    } M_CATCH(test2, M_ERROR_ACCESS) {
      assert(0);
    }
  } M_CATCH(test1, M_ERROR_MEMORY) {
    assert(flow ++ == 3);
  } M_CATCH(test1, 42) {
    assert(0);
  }
  assert(flow ++ == 4);

  flow = 0;
  assert(flow ++ == 0);
  M_TRY(test1) {
    assert(flow ++ == 1);
    M_TRY(test2) {
      assert(flow ++ == 2);
      test_throw();
    } M_CATCH(test2, M_ERROR_MEMORY) {
      assert(flow ++ == 3);
      M_RETHROW();
    }
  } M_CATCH(test1, M_ERROR_MEMORY) {
    assert(flow ++ == 4);
    assert(test1->num == 4);
    assert(test1->context[0] == 56);
    assert(test1->context[1] == 4);
    assert(test1->context[2] == 6);
    assert(test1->context[3] == 8);
  } M_CATCH(test1, 42) {
    assert(0);
  }
  assert(flow ++ == 5);

  flow = 0;
  assert(flow ++ == 0);
  M_TRY(test1) {
    assert(flow ++ == 1);
    M_TRY(test2) {
      assert(flow ++ == 2);
      M_THROW(23);
      assert(0);
    } M_CATCH(test2, M_ERROR_MEMORY) {
      assert(0);
    }
  } M_CATCH(test1, 42) {
    assert(0);
  } M_CATCH(test1, 0) {
    // Error code 0 is special and catch all errors.
    assert(flow ++ == 3);
#ifndef __cplusplus
    assert(test1->error_code == 23);
    assert(test1->num == 0);
#endif
  } 
  assert(flow ++ == 4);

}

static void test2(void)
{
  M_TRY(test1) {
    M_LET(obj, TESTOBJ_OPLIST) {
      testobj_set_ui(obj, 15);
    }
  } M_CATCH(test1, 0) {
    assert(0);
  }

  M_TRY(test1) {
    M_LET(obj, TESTOBJ_OPLIST) {
      testobj_set_ui(obj, 15);
      M_THROW(1);
      assert(0);
    }
  } M_CATCH(test1, 0) {
    // Nothing to do
  }
  
  M_TRY(test1) {
    M_LET(obj, TESTOBJ_OPLIST) {
      testobj_set_ui(obj, 15);
      M_LET(obj2, TESTOBJ_OPLIST) {
        testobj_set_ui(obj2, 16);
        M_THROW(1);
        assert(0);
      }
    }
  } M_CATCH(test1, 0) {
    // Nothing to do
  }

  M_TRY(test1) {
    M_LET(obj, TESTOBJ_OPLIST) {
      testobj_set_ui(obj, 15);
      M_LET(obj2, TESTOBJ_OPLIST) {
        testobj_set_ui(obj2, 16);
      }
      M_THROW(1);
      assert(0);
    }
  } M_CATCH(test1, 1) {
    // Nothing to do
  }

  M_TRY(test1) {
    M_LET(obj, TESTOBJ_OPLIST) {
      testobj_set_ui(obj, 15);
      M_TRY(test2) {
        M_LET(obj2, TESTOBJ_OPLIST) {
          testobj_set_ui(obj2, 16);
        }
      } M_CATCH(test2, 67) {}
      M_THROW(1);
      assert(0);
    }
  } M_CATCH(test1, 1) {
    // Nothing to do
  }

  M_TRY(test1) {
    M_LET(obj, TESTOBJ_OPLIST) {
      testobj_set_ui(obj, 15);
      M_TRY(test2) {
        M_LET(obj2, TESTOBJ_OPLIST) {
          testobj_set_ui(obj2, 16);
        }
        M_THROW(1);
        assert(0);
      } M_CATCH(test2, 67) {}
    }
  } M_CATCH(test1, 1) {
    // Nothing to do
  }
  
  M_TRY(test1) {
    M_LET(obj, TESTOBJ_OPLIST) {
      testobj_set_ui(obj, 15);
      M_TRY(test2) {
        M_LET(obj2, TESTOBJ_OPLIST) {
          testobj_set_ui(obj2, 16);
          M_THROW(1);
          assert(0);
        }
      } M_CATCH(test2, 1) {
        assert(testobj_cmp_ui(obj, 15) == 0);
        M_THROW(1);
        assert(0);
      }
    }
  } M_CATCH(test1, 1) {
    // Nothing to do
  }
  
  M_TRY(test1) {
    M_LET(obj, TESTOBJ_OPLIST) {
      testobj_set_ui(obj, 15);
      M_TRY(test2) {
        M_LET(obj2, TESTOBJ_OPLIST) {
          testobj_set_ui(obj2, 16);
          M_THROW(1);
          assert(0);
        }
      } M_CATCH(test2, 1) {
        // Nothing to do
        assert(testobj_cmp_ui(obj, 15) == 0);
      }
    }
  } M_CATCH(test1, 1) {
    // Nothing to do
    assert(0);
  }

  M_TRY(test1) {
    M_LET( (obj, (19)), TESTOBJ_OPLIST) {
      M_TRY(test2) {
        M_LET( (obj2, (16)), TESTOBJ_OPLIST) {
          M_THROW(1);
          assert(0);
        }
      } M_CATCH(test2, 1) {
        // Nothing to do
        assert(testobj_cmp_ui(obj, 19) == 0);
      }
    }
  } M_CATCH(test1, 1) {
    // Nothing to do
    assert(0);
  }

  // Test of NOCLEAR property:
  // Destructor is not called on exception.
#define FAIL(a) ((void)a, assert(0))
#define OPL (INIT(M_INIT_BASIC), TYPE(int), CLEAR(FAIL), PROPERTIES((NOCLEAR(1))) )
  M_TRY(test1) {
    M_LET( obj, OPL) {
      M_THROW(1);
      assert(0);
    }
  } M_CATCH(test1, 1) {
    // Nothing to do
  }
}

static void test3(void)
{
  volatile int flow = 0;
  volatile bool init = false;

  flow = 0;
  assert(flow ++ == 0);
  M_TRY(test1) {
    assert(flow ++ == 1);
    M_DEFER( assert(flow++ == 3) ) {
      assert(flow ++ == 2);
    }
  } M_CATCH(test1, M_ERROR_MEMORY) {
    assert(0);
  }
  assert(flow ++ == 4);

  flow = 0;
  assert(flow ++ == 0);
  M_TRY(test1) {
    assert(flow ++ == 1);
    init = true;
    M_DEFER( init = false, assert(flow++ == 3) ) {
      assert(init == true);
      assert(flow ++ == 2);
      M_THROW(1);
    }
  } M_CATCH(test1, 1) {
    assert(init == false);
    assert(flow ++ == 4);
  }
  assert(flow ++ == 5);
  assert(init == false);

  flow = 0;
  assert(flow ++ == 0);
  M_TRY(test1) {
    assert(flow ++ == 1);
    M_LET_IF(init = true, init == true, init = false) {
      assert(init == true);
      assert(flow ++ == 2);
      M_THROW(1);
    }
  } M_CATCH(test1, 1) {
    assert(init == false);
    assert(flow ++ == 3);
  }
  assert(flow ++ == 4);
  assert(init == false);
}

typedef struct {
  testobj_t o1;
  testobj_t o2;
  int num;
} struct_t[1];

// Pseudo execution counter
volatile int g_flow = 0;
// Throw execution when g_flow == g_throw
volatile int g_throw = 0;

static void struct_init(struct_t x)
{
  M_CHAIN_INIT(o1, testobj_init(x->o1), testobj_clear(x->o1) )
  M_CHAIN_INIT(o2, testobj_init(x->o2), testobj_clear(x->o2) ) {
    x->num = 0;
    if (++g_flow == g_throw) M_THROW(1);
  }
}

static void struct_init_set(struct_t x, const struct_t y)
{
  M_CHAIN_INIT(o1, testobj_init_set(x->o1, y->o1), testobj_clear(x->o1) ) {
    // Throw within the constructor (object partially constructed)
    if (++g_flow == g_throw) M_THROW(1);
    M_CHAIN_INIT(o2, testobj_init_set(x->o2, y->o2), testobj_clear(x->o2) ) {
      x->num = y->num;
      if (++g_flow == g_throw) M_THROW(1);
    }
  }
}

static void struct_initb(struct_t x)
{
  M_CHAIN_OBJ(o1, TESTOBJ_OPLIST, x->o1)
  M_CHAIN_OBJ(o2, TESTOBJ_OPLIST, x->o2)
  M_CHAIN_OBJ(o3, M_BASIC_OPLIST, x->num) {
    if (++g_flow == g_throw) M_THROW(1);
  }
}

static void struct_init_setb(struct_t x, const struct_t y)
{
  M_CHAIN_OBJ(o1, TESTOBJ_OPLIST, x->o1, y->o1) {
    // Throw within the constructor (object partially constructed)
    if (++g_flow == g_throw) M_THROW(1);
    M_CHAIN_OBJ(o2, TESTOBJ_OPLIST, x->o2, y->o2)
    M_CHAIN_OBJ(o3, M_BASIC_OPLIST, x->num, y->num) {
      if (++g_flow == g_throw) M_THROW(1);
    }
  }
}

static void struct_clear(struct_t x)
{
  testobj_clear(x->o1);
  testobj_clear(x->o2);
}

#define M_OPL_struct_t() M_CLASSIC_OPLIST(struct)

static void test4a(void)
{
  g_flow = 0;
  g_throw = 4;
  M_TRY(main) {
    assert(++g_flow == 1);
    M_LET(a, struct_t) {
      assert(++g_flow == 3);
      M_LET( (b, a), struct_t) {
        assert(false);
      }
    }
  } M_CATCH(main, 0) {
    assert(++g_flow == 5);
  }

  g_flow = 0;
  g_throw = 5;
  M_TRY(main) {
    assert(++g_flow == 1);
    M_LET(a, struct_t) {
      assert(++g_flow == 3);
      M_LET( (b, a), struct_t) {
        assert(false);
      }
    }
  } M_CATCH(main, 0) {
    assert(++g_flow == 6);
  }

  g_flow = 0;
  g_throw = 2;
  M_TRY(main) {
    assert(++g_flow == 1);
    M_LET(a, struct_t) {
      assert(false);
    }
  } M_CATCH(main, 0) {
    assert(++g_flow == 3);
  }

  g_flow = 0;
  g_throw = 0;
  M_TRY(main) {
    assert(++g_flow == 1);
    M_LET(a, struct_t) {
      assert(++g_flow == 3);
      M_LET( (b, a), struct_t) {
        assert(++g_flow == 6);
      }
    }
  } M_CATCH(main, 0) {
    assert(false);
  }
}

#define STRUCTB_OPLIST                                        \
  (INIT(struct_initb), INIT_SET(struct_init_setb), CLEAR(struct_clear), TYPE(struct_t))

static void test4b(void)
{
  g_flow = 0;
  g_throw = 4;
  M_TRY(main) {
    assert(++g_flow == 1);
    M_LET(a, STRUCTB_OPLIST) {
      assert(++g_flow == 3);
      M_LET( (b, a), STRUCTB_OPLIST) {
        assert(false);
      }
    }
  } M_CATCH(main, 0) {
    assert(++g_flow == 5);
  }

  g_flow = 0;
  g_throw = 5;
  M_TRY(main) {
    assert(++g_flow == 1);
    M_LET(a, STRUCTB_OPLIST) {
      assert(++g_flow == 3);
      M_LET( (b, a), STRUCTB_OPLIST) {
        assert(false);
      }
    }
  } M_CATCH(main, 0) {
    assert(++g_flow == 6);
  }

  g_flow = 0;
  g_throw = 2;
  M_TRY(main) {
    assert(++g_flow == 1);
    M_LET(a, STRUCTB_OPLIST) {
      assert(false);
    }
  } M_CATCH(main, 0) {
    assert(++g_flow == 3);
  }

  g_flow = 0;
  g_throw = 0;
  M_TRY(main) {
    assert(++g_flow == 1);
    M_LET(a, STRUCTB_OPLIST) {
      assert(++g_flow == 3);
      M_LET( (b, a), STRUCTB_OPLIST) {
        assert(++g_flow == 6);
      }
    }
  } M_CATCH(main, 0) {
    assert(false);
  }
}

static void test_final(void)
{
  // Throw without a try block shall raise the M_RAISE_FATAL macro
# if !defined(__cplusplus)
  test_final_in_progress = true;
  M_THROW(1);
#else
  exit(0);
#endif
}

#ifdef __GNUC__
#if __GNUC__ >= 12
  _Pragma("GCC diagnostic pop")
#endif
#endif

int main(void)
{
  test1();
  test2();
  test3();
  test4a();
  test4b();
  testobj_final_check();
  test_final();
  exit(1); // Shall not be reached.
}
