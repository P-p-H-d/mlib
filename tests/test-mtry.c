/*
 * Copyright (c) 2017-2023, Patrick Pelissier
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
#include "m-try.h"
#include "test-obj.h"

M_TRY_DEF_ONCE()

static void test_throw(void)
{
  M_TRY(protect) {
    M_THROW(67, 56);
  } M_CATCH(protect, 67) {
    assert(protect->n == 1);
    M_THROW(M_ERROR_MEMORY, protect->data[0], 4, 6, 8);
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
      assert(test2->n == 1);
      assert(test2->data[0] == 12);
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
    assert(test1->n == 2);
    assert(test1->data[0] == 1);
    assert(test1->data[1] == 3);
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
    assert(test1->n == 4);
    assert(test1->data[0] == 56);
    assert(test1->data[1] == 4);
    assert(test1->data[2] == 6);
    assert(test1->data[3] == 8);
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
    assert(test1->n == 0);
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
#define OPL (INIT(M_INIT_BASIC), TYPE(int), CLEAR(FAIL), PROPERTIES((NOCLEAR(1))))
  M_TRY(test1) {
    M_LET( obj, OPL) {
      M_THROW(1);
      assert(0);
    }
  } M_CATCH(test1, 1) {
    // Nothing to do
  }
}

int main(void)
{
  test1();
  test2();
  testobj_final_check();
  exit(0);
}
