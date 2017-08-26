M\*LIB: a Generic type-safe Container Library in C language
==========================================================

Overview
--------

M\*LIB (M star lib) is a library allowing the programmer to use **generic but
type safe container** in pure C language, aka handling generic
[containers](https://en.wikipedia.org/wiki/Container_%28abstract_data_type%29).
The objects within the containers can have proper constructor, destructor
(and other methods):
this will be handled by the library. This allows to construct fully
recursive objects (container-of[...]-container-of-type-T).

This is more or less an equivalent to the C++ STL.

M\*LIB should be portable to any systems that support C99 (or C11
for some very rare features).

M\*LIB is **only** composed of a set of headers.
There is no C file. Just put the header in the search path of your compiler,
and it will work.
So there is no dependency (except some other headers of M\*LIB).

One of M\*LIB's design key is to ensure safety. This is done by multiple means:

* in debug mode, the contracts of the function are checked, ensuring
that the data are not corrupted.
[Buffer overflow](https://en.wikipedia.org/wiki/Buffer_overflow) are checked in this mode
through [bound checking](https://en.wikipedia.org/wiki/Bounds_checking).
* very few casts are used within the library. Still the library can be used
with the greatest level of warnings by a C compiler without
any aliasing warning.
* the genericity is not done directly by macro, but indirectly by making them
define inline functions with the proper prototypes: this allows
the calls to have proper warning checks and proper debugging.

M\*LIB should still be quite-efficient: even if the implementation may not always be state
of the art, there is no overhead in using this library rather than using
direct C low-level access: the compiler is able to **fully** optimize
the library calls
since all the functions are declared as inline.
In [fact](https://github.com/P-p-H-d/mlib/wiki/performance), M\*LIB
is one of the fastest generic C library you can find.

M\*LIB uses internally the 'malloc', 'realloc' and 'free' functions to handle
the memory pool. This behavior can be overridden at different level.
M\*LIB default policy is to abort the 
program if there is a memory error. However, this behavior
can also be customized.

M\*LIB may use a lot of assertions in its implementation to ensure safety: 
it is highly recommended to properly define NDEBUG for released programs. 
Otherwise you may have a noticeable slow-down (up to a factor x10 in some cases).

M\*LIB is distributed under BSD-2 simplified license.

It is strongly advised not to read the source to know how to use the library
but rather read the examples or the tests.

Components
----------

The available containers which doesn't require the user structure to be modified are:

* [m-array.h](#m-array): header for creating array of generic type and of variable size,
* m-list.h: header for creating single-linked list of generic type,
* m-deque.h: header for creating double-ended queue of generic type and of variable size,
* m-dict.h: header for creating generic dictionary or set of generic types,
* m-tuple.h: header for creating arbitrary tuple of generic type,
* m-rbtree.h: header for creating binary sorted tree,
* m-variant.h: header for creating arbitrary variant of generic type,

The available containers of M\*LIB for thread synchronization are:

* m-buffer.h: header for creating fixed-size queue (or stack) of generic type (multiple produce / multiple consummer),
* m-snapshot: header for creating 'snapshot' buffer for passing data between a producer and a consumer running at different rates (thread safe). It ensures that the consumer only sees complete data with minimal lag, but the consumer doesn't expect to see every piece of data.
* m-shared.h: header for creating shared pointer of generic type.

The following containers are intrusive (You need to modify your structure):

* m-i-list.h: header for creating dual-linked intrusive list of generic type,
* m-i-shared.h: header for creating intrusive shared pointer of generic type (Thread Safe),


Other headers offering other functionality are:

* m-bitset.h: header for creating bit set (or "packed array of bool"),
* m-string.h: header for creating dnamic variable-length string,
* m-core.h: header for meta-programming with the C preprocessor.
* m-mutex.h: header for providing a very thin layer across multiple implementation of mutex/threads.
* m-algo.h: header for providing various generic algorithms to the previous containers.
* m-atomic.h: header for ensuring compatibility between C's stdatomic.h and C++'s atomic header. Provide also an implementation over mutex if neither are available.
* m-mempool.h: header for creating specialized & fast memory allocator.

Each containers define their iterators.

Each header can be used separately from others: dependency between headers have been kept to the minimum.

![Dependence between headers](https://raw.githubusercontent.com/P-p-H-d/mlib/master/depend.png)

Build & Installation
--------------------

M\*LIB is **only** composed of a set of headers, as such there is no build for the library.
The library doesn't depend on any other library than the libc.

To run the test suite, run:

       make check

To generate the documentation, run:

       make doc

To install the headers, run:

       make install PREFIX=/my/directory/to/install


External Reference
------------------

Many other implementation of generic container libraries exist in C.
For example:

* [KLIB](https://github.com/attractivechaos/klib)
* [SGLIB](http://sglib.sourceforge.net/)
* [GLIB](https://wiki.gnome.org/Projects/GLib)
* [GDB internal library](https://sourceware.org/git/gitweb.cgi?p=binutils-gdb.git;a=blob;f=gdb/common/vec.h;h=41e41b5b22c9f5ec14711aac35ce4ae6bceab1e7;hb=HEAD)
* [UTHASH](http://troydhanson.github.io/uthash/index.html)
* [LIBNIH](https://github.com/keybuk/libnih)
* [CCL](http://www.cs.virginia.edu/~lcc-win32/ccl/ccl.html)
* [Generic-Container-Lib](https://github.com/Siapran/Generic-Container-Lib)
* [qlibc](http://wolkykim.github.io/qlibc/)
* [cello](http://libcello.org/)
* [Red-black tree implementation](http://www.canonware.com/rb/)
* [BSD tree.h](http://fxr.watson.org/fxr/source/sys/tree.h)
* [GDSL - The Generic Data Structures Library](http://home.gna.org/gdsl/)
* [LIBCONTAINER](http://www.agottem.com/libcontainer)
* [LIBCOLLECTION](https://bitbucket.org/manvscode/libcollections)
* [LIBDYNAMIC](https://github.com/fredrikwidlund/libdynamic)
* [C GENERIC CONTAINER LIBRARY](https://github.com/ta5578/C-Generic-Container-Library)
* [LIBFDS](http://www.liblfds.org/)
* [GENCCONT: Generic C Containers](https://github.com/pmj/genccont)
* [Collections-C](https://github.com/srdja/Collections-C)
* [RayLanguage](https://github.com/kojiba/RayLanguage)
* [TommyDS](https://github.com/amadvance/tommyds)
* [LIBSRT:  Safe Real-Time library for the C programming language](https://github.com/faragon/libsrt)
* [OKLIB](https://github.com/brackeen/ok-lib)

Each of theses can be classified into:

* Everything is a pointer to void (with potential register callbacks to handle the contained objects),
* Header files are included multiple times with different macro values in order to generate different code,
* Macros are used to access structures in a generic way,
* Macros are used to generate context-dependent C code.

M\*LIB's category is the last one. Some macros are also defined to access structure in a generic way, but they are optional.
M\*LIB main added value compared to other libraries is its oplist feature
allowing it to chain the containers and/or use complex types in containers:
list of array of dictionary are perfectly supported by M\*LIB.

For the macro-preprocessing part, other libraries also exist. For example:

* [P99](http://p99.gforge.inria.fr/p99-html/)
* [C99 Lambda](https://github.com/Leushenko/C99-Lambda)
* [MAP MACRO](https://github.com/swansontec/map-macro)
* [C Preprocessor Tricks, Tips and Idioms](https://github.com/pfultz2/Cloak/wiki/C-Preprocessor-tricks,-tips,-and-idioms)
* [CPP MAGIC](http://jhnet.co.uk/articles/cpp_magic)
* [Zlang](https://github.com/pfultz2/ZLang)
* [Boost preprocessor](http://www.boost.org/doc/libs/1_63_0/libs/preprocessor/doc/index.html)

For the string library, there is for example:

* [The Better String Library](http://bstring.sourceforge.net/) (with a page which lists a lot of other string libraries).


How to use
----------

To use these data structures, you include the desired header,
instantiate the definition of the structure and its associated methods by using a macro _DEF.
Then you use the defined functions. Let's see an example which is rather simple:

    #include <stdio.h>
    #include "m-list.h"
    
    LIST_DEF(list_uint, unsigned int)      /* Define struct list_uint_t and its methods */
    
    int main(void) {
       list_uint_t list ;             /* list_uint_t has been define above */
       list_uint_init(list);          /* All type needs to be initialized */
       list_uint_push_back(list, 42); /* Push 42 in the list */
       list_uint_push_back(list, 17); /* Push 17 in the list */
       list_uint_it_t it;             /* Define an iterator to scan each one */
       for(list_uint_it(it, list)     /* Start iterator on first element */
           ; !list_uint_end_p(it)     /* Until the end is not reached */
           ; list_uint_next(it)) {    /* Set the iterator to the next element*/
              printf("%d\n",          /* Get a reference to the underlying */
                *list_uint_cref(it)); /* data and print it */
       }
       list_uint_clear(list);         /* Clear all the list */
    }

[ Do not forget to add -std=c99 (or c11) to your compile command to request a C99 compatible build ]

This looks like a typical C program except the line with 'LIST\_DEF'
which doesn't have any semi-colon at the end. And in fact, except
this line, everything is typical C program and even macro free!
The only macro is in fact LIST\_DEF: this macro expands to the
good type for the list of the defined type and to all the necessary
functions needed to handle such type. It is heavily context dependent
and can generate different code depending on it.
You can use it as many times as needed to defined as many lists as you want.

You could replace LIST\_DEF by ARRAY\_DEF to change
the kind of container (an array instead of a linked list)
without changing the code below: the generated interface
of a list or of an array is very similar.
Yet the performance remains the same as hand-written code
for both the list variant and the array variant.

This is equivalent to this C++ program using the STL:

    #include <iostream>
    #include <list>
    
    typedef std::list<unsigned int> list_uint_t;
    typedef std::list<unsigned int>::iterator list_uint_it_t;
    
    int main(void) {
       list_uint_t list ;             /* list_uint_t has been define above */
       list.push_back(42);            /* Push 42 in the list */
       list.push_back(17);            /* Push 17 in the list */
       for(list_uint_it_t it = list.begin()  /* Iterator is first element*/
           ; it != list.end()         /* Until the end is not reached */
           ; ++it) {                  /* Set the iterator to the next element*/
           std::cout << *it << '\n';  /* Print the underlying data */
       }
    }

As you can see, this is rather equivalent with the following remarks:

* M\*LIB requires an explicit definition of the instance of the list,
* M\*LIB code is more verbose in the method name,
* M\*LIB needs explicit construction and destruction (as plain old C requests),
* M\*LIB doesn't return a value to the underlying data but a pointer to this value:
  - this was done for performance (it avoids copying all the data within the stack)
  - and for generality reasons (some structure may not allow copying data).

Note: list\_uint\_t is internally defined as an array of structure of size 1.
This has the following advantages:

* you effectively reserve the data whenever you declare a variable,
* you pass automatically the variable per reference for function calls,
* you can not copy the variable by an affectation (you have to use the API instead).

You can also condense the M\*LIB code by using the EACH & LET macros if you are not afraid of using syntactic macros:

    #include <stdio.h>
    #include "m-list.h"
    
    LIST_DEF(list_uint, unsigned int)   /* Define struct list_uint_t and its methods */
    
    int main(void) {
       M_LET(list, LIST_OPLIST(uint)) { /* Define & init list as list_uint_t */
         list_uint_push_back(list, 42); /* Push 42 in the list */
         list_uint_push_back(list, 17); /* Push 17 in the list */
         for M_EACH(item, list, LIST_OPLIST(uint)) {
           printf("%d\n", *item);       /* Print the item */
         }
       }                                /* Clear of list will be done now */
    }

Another example with a complete type (with proper initialization & clear function) by using the [GMP](https://gmplib.org/) library:

    #include <stdio.h>
    #include <gmp.h>
    #include "m-array.h"

    ARRAY_DEF(array_mpz, mpz_t, (INIT(mpz_init), INIT_SET(mpz_init_set), SET(mpz_set), CLEAR(mpz_clear)) )

    int main(void) {
       array_mpz_t array ;             /* array_mpz_t has been define above */
       array_mpz_init(array);          /* All type needs to be initialized */
       mpz_t z;                        /* Define a mpz_t type */
       mpz_init(z);                    /* Initialize the z variable */
       mpz_set_ui (z, 42);
       array_mpz_push_back(array, z);  /* Push 42 in the array */
       mpz_set_ui (z, 17);
       array_mpz_push_back(array, z); /* Push 17 in the array */
       array_it_mpz_t it;              /* Define an iterator to scan each one */
       for(array_mpz_it(it, array)     /* Start iterator on first element */
           ; !array_mpz_end_p(it)      /* Until the end is not reached */
           ; array_mpz_next(it)) {     /* Set the iterator to the next element*/
              gmp_printf("%Zd\n",      /* Get a reference to the underlying */
                *array_mpz_cref(it));  /* data and print it */
       }
       mpz_clear(z);                   /* Clear the z variable */
       array_mpz_clear(array);         /* Clear all the array */
    }

or the equivalent:

    #include <stdio.h>
    #include <gmp.h>
    #include "m-array.h"

    ARRAY_DEF(array_mpz, mpz_t, M_CLASSIC_OPLIST(mpz) )
    #define ARRAY_OP ARRAY_OPLIST(mpz, M_CLASSIC_OPLIST(mpz))
    
    int main(void) {
      M_LET(array, ARRAY_OP)
       M_LET (z, M_CLASSIC_OPLIST(mpz)) {
         mpz_set_ui (z, 42);
         array_mpz_push_back(array, z);  /* Push 42 in the array */
         mpz_set_ui (z, 17);
         array_mpz_push_back(array, z); /* Push 17 in the array */
         for M_EACH(item, array, ARRAY_OP) {
              gmp_printf("%Zd\n", *item);
         }
       }
    }

Here we can see that we register the mpz\_t type into the container with
the minimum information of its interface needed.

Other examples are available in the example folder.

Internal fields of the structure are subject to change even for small revision
of the library.

The final goal of the library is to be able to write code like this in pure C while keeping type safety and compile time name resolution:

	let(list, list_uint_t) {
	  push(list, 42);
	  push(list, 17);
	  for each (item, list) {
	    print(item, "\n");
	  }
	}


ERRORS & COMPILERS
------------------

When defining the oplist of a type using M\*LIB, sometimes (often) the list of errors/warnings generated
by the compiler can be (very) huge (specially with latest compilers),
even if the error itself is minor. This is more or less the same as the use of template in C++.
You should focus mainly on the first reported error/warning
even if the link between what the compiler report and what the error is
is not immediate. The error is always in the **oplist definition**.
Examples of typical errors:

* lack of inclusion of an header,
* overriding locally operator names by macros (like NEW, DEL, INIT, OPLIST, ...),
* lack of ( ) or double level of ( ) around the oplist,
* an unknown variable (example using DEFAULT\_OPLIST instead of M\_DEFAULT\_OPLIST),
* a missing argument,
* a missing mandatory operator in the oplist.

Another way to debug is to generate the preprocessed file
(by usually calling the compiler with the '-E' option instead of '-c')
and check what's wrong in the preprocessed file:

          cc -std=c99 -E test-file.c > test-file.i
          perl -pi -e 's/;/;\n/g' test-file.i
          cc -std=c99 -c -Wall test-file.i

If there is a warning reported by the compiler in the generated code,
then there is definitely an **error** you should fix (except if it reports
shadowed variables).


OPLIST
------

An OPLIST is a fundamental notion of M\*LIB which hasn't be used in any other library.
In order to know how to operate on a type, M\*LIB needs additional information
as the compiler doesn't know how to handle properly any type (contrary to C++).
This is done by giving an operator list (or oplist in short) to any macro which
needs to handle the type. As such, an oplist as only meaning within a macro
expansion. Fundamentally, this is the exposed interface of a type with documentated
operators.

An oplist is an *unordered* list of operator in the following format:

(OPERATOR1(function1), OPERATOR2(function2), ...)

The function 'function1' is used to handle 'OPERATOR1'.
The function 'function2' is used to handle 'OPERATOR2'.

It is used to perform the association between the operation on a type
and the function which performs this operation.
Without an oplist, M\*LIB has no way to known how to deal with your type
and will deal with it like a classic C type.

A function name can be followed by M\_IPTR in the oplist (for example: INIT(init\_func M\_IPTR) )
to specify that the function accept as its *first* argument a pointer to the type rather than the type itself
(aka the prototype is init\_func(type *) instead of init\_func(type)).
If you use the '[1]' trick (see below), you won't need this.

An oplist has no real form from a C language point of view. It is just an abstraction
which disappears after the macro expansion step of the preprocessing.

For each object / container, an oplist is needed and the following operators are
usually expected for an object:

* INIT constructor(obj): initialize the object 'obj' into a valid state.
* INIT\_SET constructor(obj, org): initialize the object 'obj' into the same state as the object 'org'.
* SET operator(obj, org): set the initialized object 'obj' into the same state as the initialized object org.
* CLEAR destructor(obj): destroy the initialized object 'obj', releasing any attached memory. This method shall never fail.

INIT, INIT\_SET & SET methods shall only fail due to ***memory errors***.

Not all operators are needed within an oplist to handle a container.
If some operator is missing, the associated default operator of the function is used.
To use C integer or float types, the default constructors are perfectly fine:
you may use M\_DEFAULT\_OPLIST to define the operator list of such types or you
can just omit it.

NOTE: An iterator doesn't have a constructor nor destructor methods.

Other documented operators are:

* NEW (type) -> type pointer: alloc a new object suitable aligned. The returned object is **not initialized**. INIT operator should be called. Default is M\_MEMORY\_ALLOC.
* DEL (&obj): free the allocated uninitialized object 'obj' (default is M\_MEMORY\_DEL). The object is not cleared before being free. The object shall have been allocated by the NEW operator.
* REALLOC(type, type pointer, number) --> type pointer: realloc the given type pointer (either NULL or a pointer returned by the REALLOC operator itself) to an array of number objects of this type. Previously objects pointed by the pointer are kept up to the minimum of the new or old array size. New objects are not initialized. Freed objects are not cleared. Default is M\_MEMORY\_REALLOC.
* FREE (&obj) : free the allocated uninitialized array object 'obj' (default is M\_MEMORY\_FREE). The objects are not cleared before being free.  The object shall have been allocated by the REALLOC operator.
* INIT\_MOVE(objd, objc): Initialize 'objd' to the same state than 'objc' by stealing as resources as possible from 'objc', and then clear 'objc'. It is equivalent to calling INIT\_SET(objd,objc) then CLEAR(objc) (but usually way faster).
* MOVE(objd, objc): Set 'objd' to the same state than 'objc' by stealing as resources as possible from 'objc' and then clear 'objc'. It is equivalent to calling SET(objd,objc) then CLEAR(objc) or CLEAR(objd) and then INIT\_MOVE(objd, objc).
* SWAP(objd, objc): Swap the object 'objc' and the object 'objd' states.
* CLEAN(obj): Empty the container from all its objects. Nearly like CLEAR except that the container 'obj' remains initialized (but empty).
* HASH (obj) --> size_t: return a hash of the object (used for hash table). Default is performing a hash of the memory representation of the object.
* EQUAL(obj1, obj2) --> bool: return true if both objects are equal, false otherwise. Default is using the C comparison operator.
* CMP(obj1, obj2) --> int: return a negative integer if obj1 < obj2, 0 if obj1 = obj2, a positive integer otherwise. Default is C comparison operators.
* ADD(obj1, obj2, obj3) : set obj1 to the sum of obj2 and obj3. Default is '+' C operator.
* SUB(obj1, obj2, obj3) : set obj1 to the difference of obj2 and obj3. Default is '-' C operator.
* MUL(obj1, obj2, obj3) : set obj1 to the product of obj2 and obj3. Default is '*' C operator.
* DIV(obj1, obj2, obj3) : set obj1 to the division of obj2 and obj3. Default is '/' C operator.
* TYPE() --> type: return the type associated to this oplist.
* SUBTYPE() --> type: return the type of the element stored in the container.
* OPLIST() --> oplist: return the oplist of the type of the elements stored in the container.
* IT\_TYPE() --> type: return the type of an iterator object of this container.
* IT\_FIRST(it\_obj, container): set the object iterator it\_obj to the first sub-element of container.
* IT\_LAST(it\_obj, container): set the object iterator it\_obj to the last sub-element of container.
* IT\_END(it\_obj, container): set the object iterator it\_obj to the end of the container (Can't use PREVIOUS or NEXT afterwise).
* IT\_SET(it\_obj, it\_obj2): set the object iterator it\_obj to it\_obj2.
* IT\_END\_P(it\_obj)--> bool: return true if it\_obj is the end has been reached.
* IT\_LAST\_P(it\_obj)--> bool: return true if it_obj is the iterator end has been reached or if the iterator points to the last element.
* IT\_EQUAL\_P(it\_obj, it\_obj2) --> bool: return true if both iterators points to the same element.
* IT\_NEXT(it\_obj): move the iterator to the next sub-component.
* IT\_PREVIOUS(it\_obj): move the iterator to the previous sub-component.
* IT\_CREF(it\_obj) --> &obj: return a const pointer to the object referenced by the iterator.
* IT\_REF(it\_obj) --> &obj: return a pointer to the object referenced by the iterator.
* IT\_REMOVE(container, it\_obj): remove it\_obj from container (clearing it) and update it\_obj to point to the next object. All other iterators of the given containers become invalidated.
* OUT\_STR(FILE* f, obj): Output 'obj' as a string into the FILE stream 'f'.
* IN\_STR(obj, FILE* f) --> bool: Set 'obj' to the string object in the FILE stream 'f'. Return true in case of success (in which case the stream 'f' has been advanced to the end of the parsing of the object), false otherwise (in which case, the stream 'f' is in an indetermined position).
* GET\_STR(string_t str, obj, bool append): Set 'str' to a string representation of the object 'obj'. Append to the string if 'append' is true, set it otherwise.
* UPDATE(dest, src): Update 'dest' with 'src'. What it does exactly is node dependent: it can either SET or ADD to the node the new 'src' (default is SET).
* OOR\_SET(obj, int\_value): some containers may want to store some information within some uninitialized objects (for example Open Addressing Hash Table). This method will store the integer value 'int\_value' into the uninitialized object 'obj'. The way to store this information is object dependent. In general, you use out-of-range value for detecting such values. The object remains unitialized but set to of out-of-range value. int\_value values can be 0 or 1.
* OOR\_EQUAL(obj, int\_value): This method will compare the object 'obj' to the out-of-range value used to represent int\_value and return true if both objects are equal.


More operators are expected.

Example:

        (INIT(mpz_init),SET(mpz_set),INIT_SET(mpz_init_set),CLEAR(mpz_clear))

If there is two operations with the same name in an oplist, the left one has the priority. This allows partial overriding.


Memory Allocation
-----------------

Memory Allocation functions can be set by overriding the following macros before using the _DEF macros:

* M\_MEMORY\_ALLOC (type): return a pointer to a new object of type 'type'.
* M\_MEMORY\_DEL (ptr): free the single object pointed by 'ptr'.
* M\_MEMORY\_REALLOC (type, ptr, number): return a pointer to an array of 'number' objects of type 'type', reusing the old array pointed by 'ptr'. 'ptr' can be NULL, in which case the array will be created.
* M\_MEMORY\_FREE (ptr): free the array of objects pointed by 'ptr'.

ALLOC & DEL operators are supposed to allocate fixed size single element object (no array).
Theses objects are not expected to grow. REALLOC & FREE operators deal with allocated memory for growing objects.
Do not mix pointers between both: a pointer allocated by ALLOC (resp. REALLOC) is supposed to be only freed by DEL (resp. FREE). There are separated 'free' operators to allow specialization in the allocator (a good allocator can take this hint into account).

M\_MEMORY\_ALLOC and  M\_MEMORY\_REALLOC are supposed to return NULL in case of memory allocation failure.
The defaults are 'malloc', 'free', 'realloc' and 'free'.

You can also overide the methods NEW, DEL, REALLOC & DEL in the oplist given to a container.


Out-of-memory error
-------------------

When a memory exhaustion is reached, the global macro "M\_MEMORY\_FULL" is called and the function returns immediately after.
The object remains in a valid (if previously valid) and unchanged state in this case.
By default, the macro prints an error message and abort the program: handling non-trivial memory errors is hard,
testing such cases is mandatory to avoid security holes, testing them is even harder, 
and this behavior is rather conservative.
Moreover a good program design should handle a process entire failure (using for examples multiple
processes for doing the job) so even a process stops can be recovered.
See [here](http://joeduffyblog.com/2016/02/07/the-error-model/) for more
information about why abandonment is good.

It can however be overloaded to handle other policy for error handling like:

* throwing an error (in which case, the user is responsible to free memory of the allocated objects - destructor can still be called),
* set a global error and handle it when the function returns,
* other policy.

This function takes the size in bytes of the memory which has been tried to be allocated.

If needed, this macro shall be defined ***prior*** to instantiate the structure.



API Documentation
-----------------

### M-LIST

This header is for creating single-linked list.

#### LIST\_DEF(name, type, [, oplist])

Define the list 'name##\_t' which contains the objects of type 'type' and its associated methods as "static inline" functions.
'name' shall be a C identifier which will be used to identify the list. It will be used to create all the types and functions to handle the container.
It shall be done once per type and per compilation unit.
It also define the iterator name##\_it\_t and its associated methods as "static inline" functions.

A fundamental property of a list is that the objects created within the list
will remain at their initialized address, and won't moved due to
a new element being pushed/popped in the list.

The object 'oplist' is expected to have at least the following operators (INIT, INIT_SET, SET and CLEAR), otherwise default operators are used. If there is no given oplist, the default operators are also used. The created methods will use the operators to init, set and clear the contained object.

For this structure, the back is always the first element, and the front is the last element: the list grows from the back.

Example:

	LIST_DEF(list_mpz, mpz_t,                                               \
		(INIT(mpz_init), INIT_SET(mpz_init_set), SET(mpz_set), CLEAR(mpz_clear)))

	list_mpz_t my_list;

	void f(mpz_t z) {
		list_mpz_push_back (my_list, z);
	}

If the given oplist contain the method MEMPOOL, then LIST\_DEF will create a dedicated mempool
named with the given value of the method, optimized for this kind of list:

* it creates a mempool using MEMPOOL\_def named "list_name",
* it creates a variable named with the value of MEMPOOL\_LINKAGE used for linkage,
* it overwrites memory allocation of the created list to use this mempool with this variable.

Using mempool allows to create heavily efficient list but it will be only worth the effort in some
heavy performance context. The created mempool has to be initialized before using any
methods of the created list by calling  mempool\_list\_name\_init(variable)
and cleared by calling mempool\_list\_name\_clear(variable).

Example:

        LIST_DEF(list_uint, unsigned int, (MEMPOOL(list_mpool), MEMPOOL_LINKAGE(static)))

        static void test_list (size_t n)
        {
          mempool_list_uint_init(list_mpool);
          M_LET(a1, LIST_OPLIST(uint)) {
              for(size_t i = 0; i < n; i++)
                  list_uint_push_back(a1, rand_get() );
          }
          mempool_list_uint_clear(list_mpool);
        }


#### LIST\_OPLIST(name [, oplist])

Return the oplist of the list defined by calling LIST\_DEF with name & oplist. 

#### Created methods

In the following methods, name stands for the name given to the macro which is used to identify the type.
The following types are automatically defined by the previous macro:

#### name\_t

Type of the list of 'type'.

#### name\_it\_t

Type of an iterator over this list.

The following methods are automatically and properly created by the previous macro.

##### void name\_init(name\_t list)

Initialize the list 'list' (aka constructor) to an empty list.

##### void name\_init\_set(name\_t list, const name\_t ref)

Initialize the list 'list' (aka constructor) and set it to the value of 'ref'.

##### void name\_set(name\_t list, const name\_t ref)

Set the list 'list' to the value of 'ref'.

##### void name\_init\_move(name\_t list, name\_t ref)

Initialize the list 'list' (aka constructor) by stealing as many resources from 'ref' as possible.
After-wise 'ref' is cleared and can no longer be used.

##### void name\_move(name\_t list, name\_t ref)

Set the list 'list' (aka constructor) by stealing as many resources from 'ref' as possible.
After-wise 'ref' is cleared and can no longer be used.

##### void name\_clear(name\_t list)

Clear the list 'list (aka destructor). The list can't be used anymore, except with a constructor.

##### void name\_clean(name\_t list)

Clean the list (the list becomes empty). The list remains initialized but is empty.

##### const type *name\_back(const name\_t list)

Return a constant pointer to the data stored in the back of the list.

##### void name\_push\_back(name\_t list, type value)

Push a new element within the list 'list' with the value 'value' contained within.

##### type *name\_push\_raw(name\_t list)

Push a new element within the list 'list' without initializing it and returns a pointer to the **non-initialized** data.
The first thing to do after calling this function is to initialize the data using the proper constructor. This allows to use a more specialized
constructor than the generic one.
Return a pointer to the **non-initialized** data.

##### type *name\_push\_new(name\_t list)

Push a new element within the list 'list' and initialize it with the default constructor of the type.
Return a pointer to the initialized object.

##### void name\_pop\_back(type *data, name\_t list)

Pop a element from the list 'list' and set *data to this value.

##### bool name\_empty\_p(const name\_t list)

Return true if the list is empty, false otherwise.

##### void name\_swap(name\_t list1, name\_t list2)

Swap the list 'list1' and 'list2'.

##### void name\_it(name\_it\_t it, name\_t list)

Set the iterator 'it' to the back(=first) element of 'list'.
There is no destructor associated to this initialization.

##### void name\_it\_set(name\_it\_t it, const name\_it\_t ref)

Set the iterator 'it' to the iterator 'ref'.
There is no destructor associated to this initialization.

##### bool name\_end\_p(const name\_it\_t it)

Return true if the iterator doesn't reference a valid element anymore.

##### bool name\_last\_p(const name\_it\_t it)

Return true if the iterator references the top(=last) element or if the iterator doesn't reference a valid element anymore.

##### bool name\_it\_equal\_p(const name\_it\_t it1, const name\_it\_t it2)

Return true if the iterator it1 references the same element than it2.

##### void name\_next(name\_it\_t it)

Move the iterator 'it' to the next element of the list, ie. from the back (=first) element to the front(=last) element.

##### type *name\_ref(name\_it\_t it)

Return a pointer to the element pointed by the iterator.
This pointer remains valid until the list is modified by another method.

##### const type *name\_cref(const name\_it\_t it)

Return a constant pointer to the element pointed by the iterator.
This pointer remains valid until the list is modified by another method.

##### type *name\_get(const name\_t list, size\_t i)

Return a pointer to the element i-th of the list (from 0). 
It is assumed than i is within the size of the list.

##### const type *name\_cget(const name\_t list, size\_t i)

Return a constant pointer to the element i-th of the list (from 0). 
It is assumed than i is within the size of the list.

##### size\_t name\_size(const name\_t list)

Return the number elements of the list (aka size). Return 0 if there no element.

##### void name\_insert(name\_t list, const name\_it\_t it, const type x)

Insert 'x' after the position pointed by 'it' (which is an iterator of the list 'list') or if 'it' doesn't point anymore to a valid element of the list, it is added as the back (=first) element of the 'list'

##### void name\_remove(name\_t list, name\_it\_t it)

Remove the element 'it' from the list 'list'.
After wise, 'it' points to the next element of the list.

##### void name\_splice\_back(name\_t list1, name\_t list2, name\_it\_t it)

Move the element pointed by 'it' (which is an iterator of 'list2') from the list 'list2' to the back position of 'list1'.
After wise, 'it' points to the next element of 'list2'.

##### void name\_splice(name\_t list1, name\_t list2)

Move all the element of 'list2' into 'list1", moving the last element
of 'list2' after the first element of 'list1'.
After-wise, 'list2' is emptied.

##### void name\_reverse(name\_t list)

Reverse the order of the list.

##### void name\_get\_str(string\_t str, const name\_t list, bool append)

Generate a string representation of the list 'list' and set 'str' to this representation
(if 'append' is false) or append 'str' with this representation (if 'append' is true).
This method is only defined if the type of the element defines a GET\_STR method itself.

##### void name\_out\_str(FILE *file, const name\_t list)

Generate a string representation of the list 'list' and outputs it into the FILE 'file'.
This method is only defined if the type of the element defines a OUT\_STR method itself.

##### void name\_in\_str(FILE *file, const name\_t list)

Read from the file 'file' a string representation of a list and set 'list' to this representation.
This method is only defined if the type of the element defines a IN\_STR method itself.

##### bool name\_equal\_p(const name\_t list1, const name\_t list2)

Return true if both lists 'list1' and 'list2' are equal.
This method is only defined if the type of the element defines a EQUAL method itself.

##### size\_t name\_hash(const name\_t list)

Return the has value of 'list'.
This method is only defined if the type of the element defines a HASH method itself.




### M-ARRAY

#### ARRAY\_DEF(name, type [, oplist])

Define the array 'name##\_t' which contains the objects of type 'type' and its associated methods as "static inline" functions.
An array is a collection of element which are individually indexable.
Compared to C arrays, the created methods handle automatically the size (aka growable array).
'name' shall be a C identifier which will be used to identify the container.

It also define the iterator name##\_it\_t and its associated methods as "static inline" functions.

The object oplist is expected to have at least the following operators (INIT, INIT_SET, SET and CLEAR), otherwise default operators are used. If there is no given oplist, the default operators are also used. The created methods will use the operators to init, set and clear the contained object.

Example:

	ARRAY_DEF(array_mpfr_t, mpfr,                                                                  \
	   (INIT(mpfr_init), INIT_SET(mpfr_init_set), SET(mpfr_set), CLEAR(mpfr_clear)))

	array_mpfr_t my_array;

	void f(mpfr_t z) {
		array_mpfr_push_back (my_array, z);
	}


#### ARRAY\_OPLIST(name [, oplist])

Return the oplist of the array defined by calling ARRAY\_DEF with name & oplist. 


#### Created methods

In the following methods, name stands for the name given to the macro which is used to identify the type.
The following types are automatically defined by the previous macro:

#### name\_t

Type of the array of 'type'.

#### name\_it\_t

Type of an iterator over this array.

The following methods are automatically and properly created by the previous macros:

##### void name\_init(name\_t array)

Initialize the array 'array' (aka constructor) to an empty array.

##### void name\_init\_set(name\_t array, const name\_t ref)

Initialize the array 'array' (aka constructor) and set it to the value of 'ref'.

##### void name\_set(name\_t array, const name\_t ref)

Set the array 'array' to the value of 'ref'.

##### void name\_init\_move(name\_t array, name\_t ref)

Initialize the array 'array' (aka constructor) by stealing as many resources from 'ref' as possible.
After-wise 'ref' is cleared.

##### void name\_move(name\_t array, name\_t ref)

Set the array 'array' by stealing as many resources from 'ref' as possible.
After-wise 'ref' is cleared.

##### void name\_clear(name\_t array)

Clear the array 'array (aka destructor).

##### void name\_clean(name\_t array)

Clean the array (the array becomes empty but remains initialized).

##### void name\_push\_back(name\_t array, const type value)

Push a new element into the back of the array 'array' with the value 'value' contained within.

##### type *name\_push\_new(name\_t array)

Push a new element into the back of the array 'array' and initialize it with the default constructor.
Return a pointer to this element.

##### type *name\_push\_raw(name\_t array)

Push a new element within the array 'array' without initializing it and return
a pointer to the non-initialized data.
The first thing to do after calling this function is to initialize the data
using the proper constructor. This allows to use a more specialized
constructor than the generic one.

##### void name\_push\_at(name\_t array, size\_t key, const type x)

Push a new element into the position 'key' of the array 'array' with the value 'value' contained within.
'key' shall be a valid position of the array: from 0 to the size of array (included).

##### void name\_pop\_back(type *data, name\_t array)

Pop a element from the back of the array 'array' and set *data to this value.

##### const type *name\_back(const name\_t array)

Return a constant pointer to the last element of the array.

##### bool name\_empty\_p(const name\_t array)

Return true if the array is empty, false otherwise.

##### size\_t name\_size(const name\_t array)

Return the size of the array.

##### size\_t name\_capacity(const name\_t array)

Return the capacity of the array.

##### void name\_swap(name\_t array1, name\_t array2)

Swap the array 'array1' and 'array2'.

##### void name\_set\_at(name\_t array, size\_t i, type value)

Set the element 'i' of array 'array' to 'value'.
'i' shall be within the size of the array.

##### void name\_set\_at2(name\_t array, size\_t i, type value)

Set the element 'i' of array 'array' to 'value', increasing the size
of the array if needed.

##### void name\_resize(name\_t array, size\_t size)

Resize the array 'array' to the size 'size' (initializing or clearing elements).

##### void name\_reserve(name\_t array, size\_t capacity)

Extend or reduce the capacity of the 'array' to a rounded value based on 'capacity'.
If the given capacity is below the current size of the array, the capacity is set to the size of the array.

##### void name\_pop\_at(type *dest, name\_t array, size\_t key)

Set *dest to the value the element 'i' if dest is not NULL,
Remove this element from the array.
'key' shall be within the size of the array.

##### void name\_remove(name\_t array, name\_it\_t it)

Remove the element pointed by the iterator 'it' from the array 'array'.
'it' shall be within the array. Afterwise 'it' points to the next element, or points to the end.

##### void name\_remove\_v(name\_t array, size\_t i, size\_t j)

Remove the element 'i' (included) to the element 'j' (excluded)
from the array.
'i' and 'j' shall be within the size of the array, and i < j.

##### void name\_insert(name\_t array, size\_t i, const type x)

Insert the object 'x' at the position 'key' of the array.
'key' shall be within the size of the array.

##### void name\_insert\_v(name\_t array, size\_t i, size\_t j)

Insert from the element 'i' (included) to the element 'j' (excluded)
new empty elements to the array.
'i' and 'j' shall be within the size of the array, and i < j.

##### type *name\_get(name\_t array, size\_t i)

Return a pointer to the element 'i' of the array.
'i' shall be within the size of the array.

##### const type *name\_cget(const name\_t it, size\_t i)

Return a constant pointer to the element 'i' of the array.
'i' shall be within the size of the array.

##### void name\_it(name\_it\_t it, name\_t array)

Set the iterator 'it' to the first element of 'array'.

##### void name\_it\_set(name\_it\_t it1, name\_it\_t it2)

Set the iterator 'it1' to 'it2'.

##### bool name\_end\_p(name\_it\_t it)

Return true if the iterator doesn't reference a valid element anymore.

##### bool name\_last\_p(name\_it\_t it)

Return true if the iterator references the last element of the array, or doesn't reference a valid element.

##### bool name\_it\_equal\_p(const name\_it\_t it1, const name\_it\_t it2)

Return true if both iterators point to the same element.

##### void name\_next(name\_it\_t it)

Move the iterator 'it' to the next element of the array.

##### void name\_previous(name\_it\_t it)

Move the iterator 'it' to the previous element of the array.

##### type *name\_ref(name\_it\_t it)

Return a pointer to the element pointed by the iterator.
This pointer remains valid until the array is modified by another method.

##### const type *name\_cref(const name\_it\_t it)

Return a constant pointer to the element pointed by the iterator.
This pointer remains valid until the array is modified by another method.

##### void name\_get\_str(string\_t str, const name\_t array, bool append)

Generate a string representation of the array 'array' and set 'str' to this representation
(if 'append' is false) or append 'str' with this representation (if 'append' is true).
This method is only defined if the type of the element defines a GET\_STR method itself.

##### void name\_out\_str(FILE *file, const name\_t array)

Generate a string representation of the array 'array' and outputs it into the FILE 'file'.
This method is only defined if the type of the element defines a OUT\_STR method itself.

##### void name\_in\_str(FILE *file, const name\_t array)

Read from the file 'file' a string representation of a array and set 'array' to this representation.
This method is only defined if the type of the element defines a IN\_STR method itself.

##### bool name\_equal\_p(const name\_t array1, const name\_t array2)

Return true if both arrays 'array1' and 'array2' are equal.
This method is only defined if the type of the element defines a EQUAL method itself.

##### size\_t name\_hash(const name\_t array)

Return the has value of 'array'.
This method is only defined if the type of the element defines a HASH method itself.




### M-DEQUE

This header is for creating double-ended queue (or deque). 
A deque is an abstract data type that generalizes a queue, 
for which elements can be added to or removed from either the front (head) or back (tail)

#### DEQUE\_DEF(name, type, [, opdeque])

Define the deque 'name##\_t' which contains the objects of type 'type' and its associated methods as "static inline" functions.
'name' shall be a C identifier which will be used to identify the deque. It will be used to create all the types and functions to handle the container.
It shall be done once per type and per compilation unit.
It also define the iterator name##\_it\_t and its associated methods as "static inline" functions.

The 'oplist' is expected to have at least the following operators (INIT, INIT_SET, SET and CLEAR), otherwise default operators are used. If there is no given oplist, the default operators are also used. The created methods will use the operators to init, set and clear the contained object.

Example:

	DEQUE_DEF(deque_mpz, mpz_t,                                               \
		(INIT(mpz_init), INIT_SET(mpz_init_set), SET(mpz_set), CLEAR(mpz_clear)))

	deque_mpz_t my_deque;

	void f(mpz_t z) {
		deque_mpz_push_back (my_deque, z);
	}


#### DEQUE\_OPLIST(name [, oplist])

Return the oplist of the deque defined by calling DEQUE\_DEF with name & oplist. 

#### Created methods

In the following methods, name stands for the name given to the macro which is used to identify the type.
The following types are automatically defined by the previous macro:

#### name\_t

Type of the deque of 'type'.

#### name\_it\_t

Type of an iterator over this deque.

The following methods are automatically and properly created by the previous macro.

##### void name\_init(name\_t deque)

Initialize the deque 'deque' (aka constructor) to an empty deque.

##### void name\_init\_set(name\_t deque, const name\_t ref)

Initialize the deque 'deque' (aka constructor) and set it to the value of 'ref'.

##### void name\_set(name\_t deque, const name\_t ref)

Set the deque 'deque' to the value of 'ref'.

##### void name\_init\_move(name\_t deque, name\_t ref)

Initialize the deque 'deque' (aka constructor) by stealing as many resources from 'ref' as possible.
After-wise 'ref' is cleared and can no longer be used.

##### void name\_move(name\_t deque, name\_t ref)

Set the deque 'deque' (aka constructor) by stealing as many resources from 'ref' as possible.
After-wise 'ref' is cleared and can no longer be used.

##### void name\_clear(name\_t deque)

Clear the deque 'deque (aka destructor). The deque can't be used anymore, except with a constructor.

##### void name\_clean(name\_t deque)

Clean the deque (the deque becomes empty). The deque remains initialized but is empty.

##### const type *name\_back(const name\_t deque)

Return a constant pointer to the data stored in the back of the deque.

##### void name\_push\_back(name\_t deque, type value)

Push a new element within the deque 'deque' with the value 'value' at the back of the deque.

##### type *name\_push\_back÷\_raw(name\_t deque)

Push at the back a new element within the deque 'deque' without initializing it and returns a pointer to the **non-initialized** data.
The first thing to do after calling this function is to initialize the data using the proper constructor. This allows to use a more specialized
constructor than the generic one.
Return a pointer to the **non-initialized** data.

##### type *name\_push\_back\_new(name\_t deque)

Push at the back a new element within the deque 'deque' and initialize it with the default constructor of the type.
Return a pointer to the initialized object.

##### void name\_pop\_back(type *data, name\_t deque)

Pop a element from the deque 'deque' and set *data to this value.
If data pointer is NULL, then the poped value is discarded.

##### const type *name\_front(const name\_t deque)

Return a constant pointer to the data stored in the front of the deque.

##### void name\_push\_front(name\_t deque, type value)

Push at the front a new element within the deque 'deque' with the value 'value'.

##### type *name\_push\_front\_raw(name\_t deque)

Push at the front a new element within the deque 'deque' without initializing it and returns a pointer to the **non-initialized** data.
The first thing to do after calling this function is to initialize the data using the proper constructor. This allows to use a more specialized
constructor than the generic one.
Return a pointer to the **non-initialized** data.

##### type *name\_push\_front\_new(name\_t deque)

Push at the front a new element within the deque 'deque' and initialize it with the default constructor of the type.
Return a pointer to the initialized object.

##### void name\_pop\_front(type *data, name\_t deque)

Pop a element from the deque 'deque' and set *data to this value.
If data pointer is NULL, then the poped value is discarded.

##### bool name\_empty\_p(const name\_t deque)

Return true if the deque is empty, false otherwise.

##### void name\_swap(name\_t deque1, name\_t deque2)

Swap the deque 'deque1' and 'deque2'.

##### void name\_it(name\_it\_t it, name\_t deque)

Set the iterator 'it' to the first element of 'deque' (aka the front).
There is no destructor associated to this initialization.

##### void name\_it\_set(name\_it\_t it, const name\_it\_t ref)

Set the iterator 'it' to the iterator 'ref'.
There is no destructor associated to this initialization.

##### bool name\_end\_p(const name\_it\_t it)

Return true if the iterator doesn't reference a valid element anymore.

##### bool name\_last\_p(const name\_it\_t it)

Return true if the iterator references the last element or if the iterator doesn't reference a valid element anymore.

##### bool name\_it\_equal\_p(const name\_it\_t it1, const name\_it\_t it2)

Return true if the iterator it1 references the same element than it2.

##### void name\_next(name\_it\_t it)

Move the iterator 'it' to the next element of the deque, ie. from the front element to the back element.

##### void name\_previous(name\_it\_t it)

Move the iterator 'it' to the previous element of the deque, ie. from the back element to the front element.

##### type *name\_ref(name\_it\_t it)

Return a pointer to the element pointed by the iterator.
This pointer remains valid until the deque is modified by another method.

##### const type *name\_cref(const name\_it\_t it)

Return a constant pointer to the element pointed by the iterator.
This pointer remains valid until the deque is modified by another method.

##### type *name\_get(const name\_t deque, size\_t i)

Return a pointer to the element i-th of the deque (from 0). 
It is assumed than i is within the size of the deque.
The algorithm complexity is in O(ln(n))

##### const type *name\_cget(const name\_t deque, size\_t i)

Return a constant pointer to the element i-th of the deque (from 0). 
It is assumed than i is within the size of the deque.
The algorithm complexity is in O(ln(n))

##### size\_t name\_size(const name\_t deque)

Return the number elements of the deque (aka size). Return 0 if there no element.

##### void name\_get\_str(string\_t str, const name\_t deque, bool append)

Generate a string representation of the deque 'deque' and set 'str' to this representation
(if 'append' is false) or append 'str' with this representation (if 'append' is true).
This method is only defined if the type of the element defines a GET\_STR method itself.

##### void name\_out\_str(FILE *file, const name\_t deque)

Generate a string representation of the deque 'deque' and outputs it into the FILE 'file'.
This method is only defined if the type of the element defines a OUT\_STR method itself.

##### void name\_in\_str(FILE *file, const name\_t deque)

Read from the file 'file' a string representation of a deque and set 'deque' to this representation.
This method is only defined if the type of the element defines a IN\_STR method itself.

##### bool name\_equal\_p(const name\_t deque1, const name\_t deque2)

Return true if both deques 'deque1' and 'deque2' are equal.
This method is only defined if the type of the element defines a EQUAL method itself.

##### size\_t name\_hash(const name\_t deque)

Return the has value of 'deque'.
This method is only defined if the type of the element defines a HASH method itself.

##### void name\_swap\_at(name\_t deque, size\_t i, size\_t j)

Swap the values within the deque pointed by 'i' and by 'j'.
'i' & 'j' shall be valid index within the deque.
This method is only defined if the type of the element defines a SWAP method itself.





### M-DICT

#### DICT\_DEF2(name, key\_type, key\_oplist, value\_type, value\_oplist)

Define the dictionary 'name##\_t' and its associated methods as "static inline" functions.
A dictionary (or associative array, map, symbol table) is an abstract data type
composed of a collection of (key, value) pairs,
such that each possible key appears at most once in the collection.
'name' shall be a C identifier which will be used to identify the container.
Current implementation uses chained Hash-Table and as such, elements in the dictionary are **unordered**.

It shall be done once per type and per compilation unit.
It also define the iterator name##\_it\_t and its associated methods as "static inline" functions.

The object oplists are expected to have the following operators (INIT, INIT\_SET, SET and CLEAR), otherwise default operators are used.
If there is no given oplist, the default operators are also used. The created methods will use the operators to init, set and clear the contained object.
The key_oplist shall also define the additional operators (HASH and EQUAL).

Interface is subjected to minor change.

Example:

	DICT_DEF2(dict_str, string_t, STRING_OPLIST, string_t, STRING_OPLIST)
	dict_str_t my_dict;
	void f(string_t key, string_t value) {
		dict_str_set_at (my_dict, key, value);
	}


#### DICT\_OA\_DEF2(name, key\_type, key\_oplist, value\_type, value\_oplist)

Define the dictionary 'name##\_t' and its associated methods
as "static inline" functions much like DICT\_DEF2.
The difference is that it uses an Open Addressiong Hash-Table as 
container.

It shall be done once per type and per compilation unit.
It also define the iterator name##\_it\_t and its associated methods as "static inline" functions.

The object oplists are expected to have the following operators 
(INIT, INIT\_SET, SET, CLEAR), otherwise default operators are used.
If there is no given oplist, the default operators are also used. 
The created methods will use the operators to init, set and clear the contained object.
The key_oplist shall also define the additional operators :
HASH and EQUAL and **OOR\_EQUAL** and **OOR\_SET**

Interface is subjected to minor change.

This implementation is in general faster for small types of keys
(like integer).

Example:

	static inline bool oor_equal_p(int k, unsigned char n) {
	  return k == (int)-n-1;
	}
	static inline void oor_set(int *k, unsigned char n) {
	  *k = (int)-n-1;
	}

	DICT_OA_DEF2(dict_int, int, M_OPEXTEND(M_DEFAULT_OPLIST, OOR_EQUAL(oor_equal_p), OOR_SET(oor_set M_IPTR)), int64_t, M_DEFAULT_OPLIST)

	dict_int_t my_dict;
	void f(int key, int64_t value) {
		dict_int_set_at (my_dict, key, value);
	}


#### DICT\_OPLIST(name, key\_oplist, value\_oplist)

Return the oplist of the dictionary defined by calling DICT\_DEF2 with name & key\_oplist & value\_oplist. 

#### DICT\_SET\_DEF2(name, key\_type, key\_oplist)

Define the set 'name##\_t' and its associated methods as "static inline" functions.
A set is a specialized version of a dictionary with no value.

It shall be done once per type and per compilation unit.
It also define the iterator name##\_it\_t and its associated methods as "static inline" functions.

The oplist is expected to have the following operators (INIT, INIT\_SET, SET, CLEAR, HASH and EQUAL), otherwise default operators are used.
If there is no given oplist, the default operators are also used. The created methods will use the operators to init, set and clear the contained object.

Interface is subjected to minor change.

Example:

	DICT_SET_DEF2(dict_strSet, string_t, STRING_OPLIST)
	dict_strSet_t set;
	void f(string_t key) {
		dict_strSet_set_at (set, key);
	}

#### DICT\_SET\_OPLIST(name, key\_oplist)

Return the oplist of the set defined by calling DICT\_SET\_DEF2 with name & key\_oplist.

#### Created methods

In the following methods, name stands for the name given to the macro which is used to identify the type.
The following types are automatically defined by the previous macro:

##### name\_t

Type of the dictionary of 'key_type' --> 'value_type'.

##### name\_it\_t

Type of an iterator over this dictionary.

The following methods are automatically and properly created by the previous macro:

##### void name\_init(name\_t dict)

Initialize the dictionary 'dict' to be empty.

##### void name\_clear(name\_t dict)

Clear the dictionary 'dict'.

##### void name\_init\_set(name\_t dict, const name\_t ref)

Initialize the dictionary 'dict' to be the same as 'ref'.

##### void name\_set(name\_t dict, const name\_t ref)

Set the dictionary 'dict' to be the same as 'ref'.

##### void name\_init\_move(name\_t dict, name\_t ref)

Initialize the dictionary 'dict' by stealing as resource as possible
from 'ref' and clear 'ref'.

##### void name\_move(name\_t dict, name\_t ref)

Set the dictionary 'dict'  by stealing as resource as possible
from 'ref' and clear 'ref'.

##### void name\_clean(name\_t dict)

Clean the dictionary 'dict'. 'dict' remains initialized.

##### size\_t name\_size(const name\_t dict)

Return the number of elements of the dictionary.

##### value\_type \*name\_get(const name\_t dict, const key\_type key)

Return a pointer to the value associated to the key 'key' in dictionary
'dict' or NULL if the key is not found.

##### void name\_set\_at(name\_t dict, const key\_type key, const value\_type value)

Set the value referenced by key 'key' in the dictionary 'dict' to 'value'.

##### void name\_remove(name\_t dict, const key\_type key)

Remove the element referenced by key 'key' in the dictionary 'dict'.
Do nothing if 'key' is no present in the dictionary.

##### void name\_it(name\_it\_t it, name\_t dict)

Set the iterator 'it' to the first element of 'dict'.

##### void name\_it\_set(name\_it\_t it, const name\_it\_t ref)

Set the iterator 'it' to the same element than 'ref'.

##### bool name\_end\_p(const name\_it\_t it)

Return true if 'it' references no longer a valid element.

##### bool name\_last\_p(const name\_it\_t it)

Return true if 'it' references the last element or is no longer valid.

##### void name\_next(name\_it\_t it)

Update the iterator 'it' to the next element.

##### name\_pair\_t *name\_ref(name\_it\_t it)

Return a pointer to the pair composed by the key and its value.
'key' element can not be modified.
This pointer remains valid until the dictionary is modified by another method.

##### const name\_pair\_t *name\_ref(name\_it\_t it)

Return a constant pointer to the pair composed by the key and its value.
This pointer remains valid until the dictionary is modified by another method.

##### void name\_get\_str(string\_t str, const name\_t dict, bool append)

Generate a string representation of the dict 'dict' and set 'str' to this representation
(if 'append' is false) or append 'str' with this representation (if 'append' is true).
This method is only defined if the type of the element defines a GET\_STR method itself.

##### void name\_out\_str(FILE *file, const name\_t dict)

Generate a string representation of the dict 'dict' and outputs it into the FILE 'file'.
This method is only defined if the type of the element defines a OUT\_STR method itself.

##### void name\_in\_str(FILE *file, const name\_t dict)

Read from the file 'file' a string representation of a dict and set 'dict' to this representation.
This method is only defined if the type of the element defines a IN\_STR method itself.

##### bool name\_equal\_p(const name\_t dict1, const name\_t dict2)

Return true if both dicts 'dict1' and 'dict2' are equal.
This method is only defined if the type of the element defines a EQUAL method itself.



### M-TUPLE

#### TUPLE\_DEF2(name, (element1, type1, oplist1) [, ...])

Define the tuple 'name##\_t' and its associated methods as "static inline" functions.
A tuple is a finite ordered list of elements of different types. 
Each parameter of the macro is expected to be an element of the tuple.
Each element is defined by three parameters within parenthesis: 
the element name, the element type and the element oplist.
'name' and 'element' shall be a C identifier which will be used to identify the container.

This is more or less like a C structure. The main added value compared to using a struct
is that it generates also all the basic methods to handle it.
In fact, it generates a C struct with the given type and element.

It shall be done once per type and per compilation unit.

The object oplists are expected to have the following operators (INIT, INIT_SET, SET and CLEAR), otherwise default operators are used. If there is no given oplist, the default operators are also used. The created methods will use the operators to init, set and clear the contained object.

The interface is subjected to change.

Example:

	TUPLE_DEF2(pair, (key, string_t, STRING_OPLIST),
			 (value, mpz_t, (INIT(mpz_init), INIT_SET(mpz_init_set), SET(mpz_set), CLEAR(mpz_clear) )))
	void f(void) {
		pair_t p1;
		pair_init (p1);
		string_set_str(p1->key, "HELLO");
		mpz_set_str(p1->value, "1742", 10);
		pair_clear(p1);
	}

#### TUPLE\_OPLIST(name, oplist1[, ...] )

Return the oplist of the tuple defined by calling TUPLE\_DEF2 with the given name & the oplists.

#### Created methods

In the following methods, name stands for the name given to the macro which is used to identify the type.
The following types are automatically defined by the previous macro:

#### name\_t

Type of the defined tuple.

The following methods are automatically and properly created by the previous macros:

##### void name\_init(name\_t tuple)

Initialize the tuple 'tuple' (aka constructor) to an empty tuple.

##### void name\_init\_set(name\_t tuple, const name\_t ref)

Initialize the tuple 'tuple' (aka constructor) and set it to the value of 'ref'.

##### void name\_init\_set2(name\_t tuple, const type1 element1[, ...])

Initialize the tuple 'tuple' (aka constructor) and set it to the value of the tuple ('element1'[, ...]).

##### void name\_set(name\_t tuple, const name\_t ref)

Set the tuple 'tuple' to the value of 'ref'.

##### void name\_init\_move(name\_t tuple, name\_t ref)

Initialize the tuple 'tuple' (aka constructor) by stealing as many resources from 'ref' as possible.
After-wise 'ref' is cleared.
This method is created only if all oplists of the tuple define INIT\_MOVE method.

##### void name\_move(name\_t tuple, name\_t ref)

Set the tuple 'tuple' by stealing as many resources from 'ref' as possible.
After-wise 'ref' is cleared.
This method is created only if all oplists of the tuple define MOVE method.

##### void name\_clear(name\_t tuple)

Clear the tuple 'tuple (aka destructor).

##### const type1 *name\_get\_element1(const name\_t tuple)

Return a constant pointer to the element 'element1' of the tuple.
There is as many methods as there are elements.

##### void name\_set\_element1(name\_t tuple, type1 element1)

Set the element of the tuple to 'element1'
There is as many methods as there are elements.

##### int name\_set\_cmp(const name\_t tuple1, const name\_t tuple2)

Compare 'tuple1' to 'tuple2' using lexicographic order.
This method is created only if all oplists of the tuple define CMP method.

##### int name\_set\_equal\_p(const name\_t tuple1, const name\_t tuple2)

Return true if 'tuple1' and 'tuple2' are identical.
This method is created only if all oplists of the tuple define EQUAL method.

##### void name\_get\_str(string\_t str, const name\_t tuple, bool append)

Generate a string representation of the tuple 'tuple' and set 'str' to this representation
(if 'append' is false) or append 'str' with this representation (if 'append' is true).
This method is only defined if all oplists define a GET\_STR method.

##### void name\_out\_str(FILE *file, const name\_t tuple)

Generate a string representation of the tuple 'tuple' and outputs it into the FILE 'file'.
This method is only defined if all oplists define a OUT\_STR method.

##### void name\_in\_str(FILE *file, const name\_t tuple)

Read from the file 'file' a string representation of a tuple and set 'tuple' to this representation.
This method is only defined if all oplists define a IN\_STR method.


### M-VARIANT

#### VARIANT\_DEF2(name, (element1, type1, oplist1) [, ...])

Define the variant 'name##\_t' and its associated methods as "static inline" functions.
A variant is a finite exclusive list of elements of different types :
the variant can be only equal to one element at a time. 
Each parameter of the macro is expected to be an element of the variant.
Each element is defined by three parameters within parenthesis: 
the element name, the element type and the element oplist.
'name' and 'element' shall be a C identifier which will be used to identify the container.

This is more or less like a C union. The main added value compared to using a union
is that it generates all the basic methods to handle it and it dynamically identifies
which element is stored within.

It shall be done once per type and per compilation unit.

The object oplists are expected to have the following operators (INIT, INIT_SET, SET and CLEAR), otherwise default operators are used. If there is no given oplist, the default operators are also used. The created methods will use the operators to init, set and clear the contained object.

The interface is subjected to change.

Example:

	VARIANT_DEF2(either, (key, string_t, STRING_OPLIST),
			 (value, mpz_t, (INIT(mpz_init), INIT_SET(mpz_init_set), SET(mpz_set), CLEAR(mpz_clear) )))
	void f(sting_t s) {
		either_t p1;
		either_init (p1);
		either_set_key(p1, s);
		either_clear(p1);
	}

#### VARIANT\_OPLIST(name, oplist1[, ...] )

Return the oplist of the variant defined by calling VARIANT\_DEF2 with the given name & the oplists.

#### Created methods

In the following methods, name stands for the name given to the macro which is used to identify the type.
The following types are automatically defined by the previous macro:

#### name\_t

Type of the defined variant.

The following methods are automatically and properly created by the previous macros:

##### void name\_init(name\_t variant)

Initialize the variant 'variant' (aka constructor) to be empty.

##### void name\_init\_set(name\_t variant, const name\_t ref)

Initialize the variant 'variant' (aka constructor) and set it to the value of 'ref'.

##### void name\_set(name\_t variant, const name\_t ref)

Set the variant 'variant' to the value of 'ref'.

##### void name\_init\_move(name\_t variant, name\_t ref)

Initialize the variant 'variant' (aka constructor) by stealing as many resources from 'ref' as possible.
After-wise 'ref' is cleared.
This method is created only if all oplists of the variant define INIT\_MOVE method.

##### void name\_move(name\_t variant, name\_t ref)
##### void name\_move_elementN(name\_t variant, typeN ref)

Set the variant 'variant' by stealing as many resources from 'ref' as possible.
After-wise 'ref' is cleared.
This method is created only if all oplists of the variant define MOVE method.

##### void name\_clear(name\_t variant)

Clear the variant 'variant (aka destructor).

##### void name\_clean(name\_t variant)

Clean the variant 'variant and make it empty.

##### void name\_set_elementN(name\_t variant, const typeN elementN)

Set the variant 'variant' to the type and value of 'element1'.

##### typeN * name\_get_elementN(name\_t variant)

Return a pointer to the 'varian' viewed as of type 'typeN'.
If the variant isn't an object of such type, it returns NULL.

##### bool name\_empty\_p(const name\_t variant)

Return true if the variant is empty, false otherwise.

##### bool name\_elementN\_p(const name\_t variant)

Return true if the variant is of the type of 'elementN'.

##### size_t name\_hash(const name\_t variant)

Return a hash associated to the variant.
All types associated to the variant shall have a hash function
for this function to be defined.

##### bool name\_equal\_p(const name\_t variant1, const name\_t varian2)

Return true if both objects are equal, false otherwise.
All types associated to the variant shall have a equal\_p function
for this function to be defined.

##### void name\_swap(name\_t variant1, name\_t varian2)

Swap both objects.

##### void name\_get\_str(string\_t str, name\_t variant, bool append)

Convert the variant into a string, appending it into 'str' or not.
All types associated to the variant shall have a get\_str function
for this function to be defined.

##### void name\_out\_str(FILE *f, name\_t variant)

Convert the variant into a string and send it to the stream 'f'.
All types associated to the variant shall have a out\_str function
for this function to be defined.

##### void name\_in\_str(name\_t variant, FILE *f)

Read a string representation of the variant from the stream 'f'
and update the object variant with it.
All types associated to the variant shall have a in\_str function
for this function to be defined.


### M-RBTREE

#### RBTREE\_DEF(name, type[, oplist])

Define the binary ordered tree 'name##\_t' and its associated methods as "static inline" functions.
A binary tree is a tree data structure in which each node has at most two children, which are referred to as the left child and the right child.
In this kind of tree, all elements of the tree are totally ordered.
The current implementation is [RED-BLACK TREE](https://en.wikipedia.org/wiki/Red%E2%80%93black_tree).
It shall not be confused with a [B-TREE](https://en.wikipedia.org/wiki/B-tree).
'name' shall be a C identifier which will be used to identify the container.

The CMP operator is used to perform the total ordering of the elements.

The UPDATE operator is used to update an element if the 
pushed item already exist in the container. The default behavior
will overwrite the recorded value with the new one.
 
It shall be done once per type and per compilation unit.
It also define the iterator name##\_it\_t and its associated methods as "static inline" functions.

The object oplist is expected to have the following operators (INIT, INIT_SET, SET, CMP and CLEAR), otherwise default operators are used. If there is no given oplist, the default operators are also used. The created methods will use the operators to init, set and clear the contained object.

Example:

	RBTREE_DEF(rbtree_uint, unsigned int)
	void f(unsigned int num) {
		rbtree_uint_t tree;
		rbtree_uint_init(tree);
		for(unsigned int i = 0; i < num; i++)
			rbtree_uint_push(tree, i);
		rbtree_uint_clear(tree);                              
	}

#### RBTREE\_OPLIST(name [, oplist])

Return the oplist of the Red-Black defined by calling RBTREE\_DEF with name & oplist.


#### Created methods

The following methods are automatically and properly created by the previous macros. 
In the following methods, name stands for the name given to the macro which is used to identify the type.

##### name\_t

Type of the Red Black Tree of 'type'.

##### name\it_\_t

Type of an iterator over this Red Black Tree.

##### void name\_init(name\_t rbtree)

Initialize the Red Black Tree 'rbtree' to be empty.

##### void name\_clear(name\_t rbtree)

Clear the Red Black Tree 'rbtree'.

##### void name\_init\_set(name\_t rbtree, const name\_t ref)

Initialize the Red Black Tree 'rbtree' to be the same as 'ref'.

##### void name\_set(name\_t rbtree, const name\_t ref)

Set the Red Black Tree 'rbtree' to be the same as 'ref'.

##### void name\_init\_move(name\_t rbtree, name\_t ref)

Initialize the Red Black Tree 'rbtree' by stealing as resource as possible
from 'ref' and clear 'ref'.

##### void name\_move(name\_t rbtree, name\_t ref)

Set the Red Black Tree 'rbtree' by stealing as resource as possible
from 'ref' and clear 'ref'.

##### void name\_clean(name\_t rbtree)

Clean the Red Black Tree 'rbtree'. 'rbtree' remains initialized but empty.

##### size\_t name\_size(const name\_t rbtree)

Return the number of elements of the Red Black Tree.

##### void name\_push(name\_t rbtree, const type data)

Push 'data' into the Red Black Tree 'rbtree' at its ordered place
while keeping the tree balanced.

##### void name\_pop(type *dest, name\_t rbtree, const type data)

Pop 'data' from the Red Black Tree 'rbtree'
and save the poped value into 'dest' if the pointer is not null
while keeping the tree balanced.
Do nothing if 'data' is no present in the Red Black Tree.

##### type * name\_min(const name\_t rbtree)
##### const type * name\_cmin(const name\_t rbtree)

Return a pointer to the minimum element of the tree
or NULL if there is no element.

##### type * name\_max(const name\_t rbtree)
##### const type * name\_cmax(const name\_t rbtree)

Return a pointer to the maximum element of the tree
or NULL if there is no element.

##### type * name\_get(const name\_t rbtree, const type *data)
##### const type * name\_cget(const name\_t rbtree, const type *data)

Return a pointer to the element of the tree 'rbtree' which is equal to 'data',
or NULL if there is no match.

##### void name\_swap(name\_t rbtree1, name\_t rbtree2)

Swap both trees.

##### bool name\_empty\_p(const name\_t rbtree)

Return true if the tree is empty, false otherwise.

##### void name\_it(name\it_\_t it, name\_t rbtree)

Set the iterator 'it' to the first element of 'rbtree'.

##### void name\_it\_set(name\it_\_t it, const name\it_\_t ref)

Set the iterator 'it' to the same element than 'ref'.

##### void name\_it\_last(name\it_\_t it, name\_t rbtree)

Set the iterator 'it' to the last element of 'rbtree'.

##### void name\_it\_end(name\it_\_t it, name\_t rbtree)

Set the iterator 'it' to no element of 'rbtree'.

##### void name\_it\_from(name\it_\_t it, const name\_t rbtree, const type data)

Set the iterator 'it' to the greatest element of 'rbtree'
lower of equal than 'data' or the first element is there is none.

##### bool name\_end\_p(const name\it_\_t it)

Return true if 'it' references no longer a valid element.

##### bool name\_last\_p(const name\it_\_t it)

Return true if 'it' references the last element or is no longer valid.

##### bool name\_to\_p(const name\it_\_t it, const type data)

Return true if 'it' references an element which is greater or equal than 'data'.

##### void name\_next(name\it_\_t it)

Update the iterator 'it' to the next element.

##### void name\_previous(name\it_\_t it)

Update the iterator 'it' to the previous element.

##### type *name\_ref(name\it_\_t it)
##### const type *name\_ref(name\it_\_t it)

Return a pointer to the element pointer by the iterator 'it'.
This pointer remains valid until the Red Black Tree is modified by another method.

##### void name\_get\_str(string\_t str, const name\_t rbtree, bool append)

Generate a string representation of the rbtree 'rbtree' and set 'str' to this representation
(if 'append' is false) or append 'str' with this representation (if 'append' is true).
This method is only defined if the type of the element defines a GET\_STR method itself.

##### void name\_out\_str(FILE *file, const name\_t rbtree)

Generate a string representation of the rbtree 'rbtree' and outputs it into the FILE 'file'.
This method is only defined if the type of the element defines a OUT\_STR method itself.

##### void name\_in\_str(FILE *file, const name\_t rbtree)

Read from the file 'file' a string representation of a rbtree and set 'rbtree' to this representation.
This method is only defined if the type of the element defines a IN\_STR method itself.

##### bool name\_equal\_p(const name\_t rbtree1, const name\_t rbtree2)

Return true if both rbtrees 'rbtree1' and 'rbtree2' are equal.
This method is only defined if the type of the element defines a EQUAL method itself.

##### size\_t name\_hash\_p(const name\_t rbtree)

Return the hash of the tree.
This method is only defined if the type of the element defines a HASH method itself.



### M-BUFFER

#### BUFFER\_DEF(name, type, size, policy[, oplist])

Define the buffer 'name##\_t' and its associated methods as "static inline" functions.
A buffer is a fixed size queue or stack.
If it is built with the BUFFER\_THREAD\_SAFE option it can be used to transfert message
from mutiple producer threads to multiple consummer threads. This is done internaly using
a mutex.

'name' shall be a C identifier which will be used to identify the container.

The size parameter defined the fixed size of the queue. It can be 0, in which case, the fixed size will be defined at initialization time.

Multiple additional policy can be applied to the buffer by performing a logical or of the following properties:

* BUFFER\_QUEUE : define a FIFO queue (default),
* BUFFER\_STACK : define a stack (exclusive with BUFFER\_QUEUE),
* BUFFER\_BLOCKING : define blocking calls for push/pop (default),
* BUFFER\_UNBLOCKING : define unblocking calls for push/pop,
* BUFFER\_THREAD\_SAFE : define thread safe functions (default),
* BUFFER\_THREAD\_UNSAFE : define thread unsafe functions,
* BUFFER\_PUSH\_INIT\_POP\_MOVE : change the behavior of PUSH to push a new initialized object, and POP as moving this new object into the new emplacement (this is mostly used for performance reasons or to handle properly a shared_ptr semantic). In practice, it works as if POP performs the initialization of the object. 
* BUFFER\_PUSH\_OVERWRITE : PUSH will always overwrite the first entry (this is mostly used to reduce latency).

This container is designed to be used for easy synchronization inter-threads (and the variable shall be a global shared one).

It shall be done once per type and per compilation unit.

The object oplist is expected to have the following operators (INIT, INIT\_SET, SET, INIT\_MOVE and CLEAR), otherwise default operators are used. If there is no given oplist, the default operators are also used. The created methods will use the operators to init, set and clear the contained object.

Example:

	BUFFER_DEF(buffer_uint, unsigned int, 10, BUFFER_QUEUE|BUFFER_BLOCKING)

	buffer_uint_t g_buff;

	void f(unsigned int i) {
		buffer_uint_init(g_buff, 10);
		buffer_uint_push(g_buff, i);
		buffer_uint_pop(&i, g_buff);
		buffer_uint_clear(g_buff);
	}


#### Created methods

The following methods are automatically and properly created by the previous macros. In the following methods, name stands for the name given to the macro which is used to identify the type.

##### void name\_init(buffer\_t buffer, size\_t size)

Initialize the buffer 'buffer' to fill in at least 'size-1' elements.
The 'size' argument shall be the same as the one used to create the buffer
or the one used to create the buffer was '0'.
This function is not thread safe.

##### void name\_clear(buffer\_t buffer)

Clear the buffer and destroy all its allocations.
This function is not thread safe.

##### void name\_clean(buffer\_t buffer)

Clean the buffer making it empty but remain initialized.
This function is thread safe if the buffer was built thread safe.

##### bool name\_empty\_p(const buffer\_t buffer)

Return true if the buffer is empty, false otherwise.
This function is thread safe if the buffer was built thread safe. 

##### bool name\_full\_p(const buffer\_t buffer)

Return true if the buffer is full, false otherwise.
This function is thread safe if the buffer was built thread safe. 

##### size\_t name\_size(const buffer\_t buffer)

Return the number of elements in the buffer which can be enqueued.
This function is thread safe if the buffer was built thread safe. 

##### size\_t name\_overwrite(const buffer\_t buffer)

If the buffer is built with the BUFFER\_PUSH\_OVERWRITE option,
this function returns the number of elements which have been overwritten
and thus discarded.
If the buffer was not built with the BUFFER\_PUSH\_OVERWRITE option,
it returns 0.

##### bool name\_push(buffer\_t buffer, const type data)

Push the object 'data' in the buffer 'buffer'.
It waits for any empty room to come if the buffer was built as blocking.
Returns true if the data was pushed, false otherwise.
Always return true if the buffer is blocking.
This function is thread safe if the buffer was built thread safe. 

##### bool name\_pop(type *data, buffer\_t buffer)

Pop from the buffer 'buffer' into the object pointed by 'data'.
It waits for any data to come if the buffer was built as blocking.
If the buffer is built with the BUFFER\_PUSH\_INIT\_POP\_MOVE option,
the object pointed by 'data' shall be uninitialized
(the pop function will perform a quick initialization of the object
using an INIT_MOVE operator)
, otherwise it shall be an initialized object (the pop function will 
perform a SET operator).
Returns true if a data was popped, false otherwise.
Always return true if the buffer is blocking.
This function is thread safe if the buffer was built thread safe. 



### M-SNAPSHOT

This header is for created snapshots.

#### SNAPSHOT\_DEF(name, type[, oplist])

Define the snapshot 'name##\_t' and its associated methods as "static inline" functions.
A snapshot is a mechanism to allows a consumer thread and a produce thread, **working at different speeds**, to exchange messages in a reliable and thread safe way (the message is passed atomatically).
In practice, it is implemented using a triple buffer.

This container is designed to be used for easy synchronization inter-threads (and the variable shall be a global one).

It shall be done once per type and per compilation unit. No all functions are thread safe.

The object oplist is expected to have the following operators (INIT, INIT\_SET, SET and CLEAR), otherwise default operators are used. If there is no given oplist, the default operators are also used. The created methods will use the operators to init, set and clear the contained object. It can have the optional methods INIT\_MOVE and MOVE.

Example:

	SNAPSHOT_DEF(snapshot_uint, unsigned int)
	void f(unsigned int * i) {
		snapshot_uint_t t;
		snapshot_uint_init(t);
		i = snapshot_uint_take(t);
		*i = 1;
		i = snapshot_uint_look(t);
		snapshot_uint_clear(t);
	}


#### Created methods

The following methods are automatically and properly created by the previous macros. In the following methods, name stands for the name given to the macro which is used to identify the type.

TODO: document the API.


### M-SHARED

This header is for creating shared pointer.

#### SHARED\_PTR\_DEF(name, type[, oplist])

Define the shared pointer 'name##\_t' and its associated methods as "static inline" functions.
A shared pointer is a mechanism to keep tracks of all users of an object
and performs an automatic destruction of the object whenever all users release
their need on this object.

The destruction of the object is thread safe.

The object oplist is expected to have the following operators (CLEAR and DEL), otherwise default operators are used. If there is no given oplist, the default operators are also used. The created methods will use the operators to init, set and clear the contained object.

There are designed to work with buffers with policy BUFFER\_PUSH\_INIT\_POP\_MOVE
to send a shared pointer across multiple threads.

Example:

	SHARED_PTR_DEF(shared_mpz, mpz_t, (CLEAR(mpz_clear)))
	void f(void) {
		shared_mpz_t p;
		mpz_t z;
		mpz_init(z);
		shared_mpz_init2 (p, z);
		buffer_uint_push(g_buff1, p);
		buffer_uint_push(g_buff2, p);
		shared_mpz_clear(p);
	}


#### Created methods

The following methods are automatically and properly created by the previous macros. In the following methods, name stands for the name given to the macro which is used to identify the type.

TODO: document the API.


### M-I-SHARED

This header is for creating intrusive shared pointer.

#### ISHARED\_PTR\_DEF(name, type[, oplist])

Extend an object by adding the necessary interface to handle it as a shared pointer and define the associated methods to handle it as "static inline" functions.
A shared pointer is a mechanism to keep tracks of all users of an object
and performs an automatic destruction of the object whenever all users release
their need on this object.

The destruction of the object is thread safe.

The object oplist is expected to have the following operators (CLEAR and DEL), otherwise default operators are used. If there is no given oplist, the default operators are also used. The created methods will use the operators to init, set and clear the contained object.

There are designed to work with buffers with policy BUFFER\_PUSH\_INIT\_POP\_MOVE
to send a shared pointer across multiple threads.

It is recommended to use the intrusive shared pointer over the standard one if possible.
(They are faster & cleaner).

Example:

        typedef struct mystruct_s {
                ISHARED_PTR_INTERFACE(ishared_mystruct, mystruct_s);
                char *message;
        } mystruct_t;

        static void mystruct_clear(mystruct_t *p) { free(p->message); }

        ISHARED_PTR_DEF(ishared_mystruct, mystruct_t, (CLEAR(mystruct_clear M_IPTR)))

        void f(void) {
                mystruct_t *p = ishared_mystruct_new();
                p->message = strdup ("Hello");
                buffer_mystruct_push(g_buff1, p);
                buffer_mystruct_push(g_buff2, p);
                ishared_mystruct_clear(p);
        }


#### Created methods

The following methods are automatically and properly created by the previous macros. In the following methods, name stands for the name given to the macro which is used to identify the type.

TODO: document the API.


### M-I-LIST

This header is for creating intrusive dual-chained list.

#### ILIST\_INTERFACE(name, type)

Extend an object by adding the necessary interface to handle it within 
a dual linked intrusive list.
This is the intrusive part.
It shall be put within the structure of the object to link, at the top
level of the structure.
See example of ILIST\_DEF.

#### ILIST\_DEF(name, type[, oplist])

Define the dual linked list 
and define the associated methods to handle it as "static inline" functions.
'name' shall be a C identifier which will be used to identify the list. It will be used to create all the types and functions to handle the container.
It shall be done once per type and per compilation unit.

An object is expected to be part of only one list of a kind in the entire program at a time.
An intrusive list allows to move from an object to the next object without needing to go through the entire list,
or to remove an object from a list in O(1).
It may, or may not, be better than standard list. It depends on the context.

The object oplist is expected to have the default operators. If there is no given oplist, the default values for the operators are used. The created methods will use the operators to init, set and clear the contained object.

The given interface won't allocate anything to handle the objects as
all allocations and initialization are let to the user.

However the objects within the list can be automaticly be destroyed
by calling the CLEAR method to destruct the object and the DEL
method to free the used memory (only if the FREE operator is defined in the
oplist).
If there is no FREE operator, it is up to the user to free the used memory.
The default CLEAR operator will do nothing on the object.

Example:

	typedef struct test_s {
	  int n;
	  ILIST_INTERFACE (ilist_tname, struct test_s);
	} test_t;

	ILIST_DEF(ilist_tname, test_t)

	void f(void) {
		test_t x1, x2, x3;
		ilist_tname_t list;

		x1.n = 1;
		x2.n = 2;
		x3.n = 3;

		ilist_tname_init(list);
		ilist_tname_push_back (list, &x3);
		ilist_tname_push_front (list, &x1);
		ilist_tname_push_after (&x1, &x2);

		int n = 1;
		for M_EACH(item, list, ILIST_OPLIST(ilist_tname)) {
			assert (n == item->n);
			n++;
		}
		ilist_tname_clear(list);
	}


#### Created methods

The following methods are automatically and properly created by the previous macros. In the following methods, name stands for the name given to the macro which is used to identify the type.

TODO: document the API.

### M-BITSET

This header is for using bitset.

A bitset can be seen as a specialized version of an array of bool, where each item takes only 1 bit.
It allows for compact representation of such array.

Example:

	void f(void) {
		bitset_t set;
		bitset_init(set);
		for(int i = 0; i < 100; i ++)
			bitset_push_back(set, i%2);
		bitset_clear(set);
	}


#### methods

The methods are mostly the same than for an array. The following methods are available:

TODO: document the API.

### M-STRING

This header is for using dynamic string. The size of the string is automatically updated in function of the needs.


Example:

	void f(void) {
		string_t s1;
		string_init (s1);
		string_set_str (s1, "Hello, world!");
		string_clear(s1);
	}

#### methods

The following methods are available:

TODO: document the API.

### M-CORE

This header is the internal core of M\*LIB, providing a lot of functionality 
by extending a lot the preprocessing capability.
Working with these macros is not easy and the developer needs to know
how the macro preprocessing works.
It also adds the needed macro for handling the oplist.
As a consequence, it is needed by all other header files.

Some macros are using recursivity to work.
This is not an easy feat to do as it needs some tricks to work (see
reference).
This still work well with only one major limitation: it can not be chained.
For example, if MACRO is a macro implementing recursivity, then
MACRO(MACRO()) won't work.


Example:

	M_MAP(f, 1, 2, 3, 4)
	M_REDUCE(f, g, 1, 2, 3, 4)
	M_SEQ(1, 20, f)
	
#### Compiler Macros

The following compiler macros are available:

##### M\_ASSUME(cond)

M\_ASSUME is equivalent to assert, but gives hints to compiler
about how to optimize the code if NDEBUG is defined.

##### M\_LIKELY(cond) / M\_UNLIKELY(cond)

M\_LIKELY / M\_UNLIKELY gives hints on the compiler of the likehood
of the given condition.

#### Preprocessing macro extension

##### M\_MAX\_NB\_ARGUMENT

Maximum number of argument which can be handled by this header.

##### M\_C(a,b), M\_C3(a,b,c), M\_C4(a,b,c,d)

Return a symbol corresponding to the concatenation of the input arguments.

##### M\_INC(number)

Increment the number given as argument (from [0..29[) and return 
a pre-processing token corresponding to this value (meaning it is evaluated
at macro processing stage, not at compiler stage).
If number is not within the range, the behavior is undefined.

##### M\_DEC(number)

Decrement the number given as argument (from [0..29[) and return 
a pre-processing token corresponding to this value (meaning it is evaluated
at macro processing stage, not at compiler stage).
If number is not within the range, the behavior is undefined.

##### M\_RET\_ARG1(arglist,...) / M\_RET\_ARG2(...) / M\_RET\_ARG3(...) /  M\_RET\_ARG27(...)

Return the argument 1 of the given arglist (respectively 2, 3, and 27).
The argument shall exist in the arglist.

##### M\_BOOL(cond)

Convert an integer or a symbol into 0 (if 0) or 1 (if not 0 or symbol unknown).
Return a pre-processing token corresponding to this value (meaning it is evaluated
at macro processing stage, not at compiler stage).

##### M\_INV(cond)

Inverse 0 into 1 and 1 into 0. It is undefined if cond is not 0 or 1
(use M\_BOOL to convert). 
Return a pre-processing token corresponding to this value (meaning it is evaluated
at macro processing stage, not at compiler stage).

##### M\_AND(cond1, cond2)

Perform a logical 'and' between cond1 and cond2. 
cond1 and cond2 shall be 0 or 1 otherwise it is undefined.
Return a pre-processing token corresponding to this value (meaning it is evaluated
at macro processing stage, not at compiler stage).

##### M\_OR(cond1, cond2)

Perform a logical 'or' between cond1 and cond2. 
cond1 and cond2 shall be 0 or 1 otherwise it is undefined.
Return a pre-processing token corresponding to this value (meaning it is evaluated
at macro processing stage, not at compiler stage).

##### M\_IF(cond)(action\_if\_true, action\_if\_false)

Return the pre-processing token 'action_if_true' if 'cond' is true, action\_if\_false otherwise (meaning it is evaluated
at macro processing stage, not at compiler stage).
cond shall be 0 or 1 otherwise it is undefined.

##### M\_COMMA\_P(arglist)

Return 1 if there is a comma inside the argument list, 0 otherwise.
Return a pre-processing token corresponding to this value (meaning it is evaluated
at macro processing stage, not at compiler stage).

##### M\_EMPTY\_P(expression)

Return 1 if the argument 'expression' is 'empty', 0 otherwise.
Return a pre-processing token corresponding to this value (meaning it is evaluated
at macro processing stage, not at compiler stage).

##### M\_DEFERRED\_COMMA

Return a comma ',' at a later phase of the macro processing steps.

##### M\_IF\_EMPTY(cond)(action\_if\_true, action\_if\_false)

Return the pre-processing token 'action_if_true' if 'cond' is empty, action\_if\_false otherwise (meaning it is evaluated
at macro processing stage, not at compiler stage).
cond shall be 0 or 1 otherwise it is undefined.

##### M\_PARENTHESIS\_P(expression)

Return 1 if the argument 'expression' starts a parenthesis and ends it
(like '(...)'), 0 otherwise.
Return a pre-processing token corresponding to this value (meaning it is evaluated
at macro processing stage, not at compiler stage).

##### M\_DELAY1(expr) / M\_DELAY2(expr) / M\_DELAY3(expr) / M\_DELAY4(expr) / M\_ID

Delay the evaluation by 1, 2, 3 or 4 steps.
This is necessary to write macros which are recursive.
The argument is a macro-function which has to be deferred.
M\_ID is an equivalent of M\_DELAY1.

##### M\_EVAL(expr)

Perform a complete stage evaluation of the given expression,
removing recursive expression within it.
Only ONE M\_EVAL expression is expected in the evaluation chain.
Can not be chained.

##### M\_APPLY(func, args...)

Apply 'func' to '(args...) ensuring
that a() isn't evaluated until all 'args' have been also evaluated.
It is used to delay evaluation.

##### M\_APPLY\_FUNC(func, arg1)

Apply 'func' to 'arg1' if 'arg1' is not empty.

##### M\_APPLY\_FUNC2(func, arg1, arg2)

Apply 'func' to 'arg1, arg2' if 'arg2' is not empty.

##### M\_MAP(func, args...)

Apply 'func' to each argument of the 'args...' list of argument.

##### M\_MAP2(func, data, args...)

Apply 'func' to each couple '(data, argument)' 
with argument an element of the 'args...' list.

##### M\_MAP\_PAIR(func, args...)

Map a macro to all given pair of arguments (Using recursivity).
Can not be chained.

##### M\_REDUCE(funcMap, funcReduce, args...)

Map the macro funcMap to all given arguments 'args'
and reduce all theses computation with the macro 'funcReduce' (using recursivity)
Can not be chained.

##### M\_REDUCE2(funcMap, funcReduce, data, args...)

Map the macro funcMap to all pair (data, arg) of the given argument list 'args' 
and reduce all theses computation with the macro 'funcReduce'.
Do not use recursivity.

##### M\_SEQ(init, end, macro, data)

Generate a sequence of number from 'init' to 'end'
and apply to the macro the pair '(data, num)' for each number 'num'
of the sequence (using recursivity).

##### M\_NARGS(args...)

Return the number of argument of the given list.
Doesn't work for empty argument.

##### M\_IF\_NARGS\_EQ1(argslist)(action\_if\_one\_arg, action\_otherwise)

Return the pre-processing token 'action\_if\_one\_arg' if 'argslist' has only one argument, action\_otherwise otherwise
(meaning it is evaluated at macro processing stage, not at compiler stage).

##### M\_IF\_NARGS\_EQ2(argslist)(action\_if\_two\_arg, action\_otherwise)

Return the pre-processing token 'action\_if\_two\_arg' if 'argslist' has two arguments, action\_otherwise otherwise
(meaning it is evaluated at macro processing stage, not at compiler stage).

##### M\_IF\_DEBUG(action)

Return the pre-processing token 'action' if the build is compiled in debug mode (i.e. NDEBUG is undefined).

##### M\_NOTEQUAL(x, y)

Return 1 if x != y, 0 otherwise (resolution is performed at preprocessing time).
x and y shall be within the maximum argument value.

##### M\_EQUAL(x, y)

Return 1 if x == y, 0 otherwise (resolution is performed at preprocessing time).
x and y shall be within the maximum argument value.

##### M\_ADD(x, y)

Return x+y (resolution is performed at preprocessing time).
x and y shall be within the maximum argument value (using recursivity).

##### M\_SUB(x, y)

Return x-y (resolution is performed at preprocessing time).
x and y shall be within the maximum argument value and x >= y (using recursivity).

##### M\_INVERT(args...)

Reverse the argument list. For example, if args was a,b,c, it return c,b,a.

### C11 Macro

Theses macros are only valid if the program is built in C11 mode:

##### M\_PRINTF\_FORMAT(x)

Return the printf format associated to the type of 'x'. 'x' shall be printable with printf.

##### M\_PRINT\_ARG(x)

Print using printf the variable 'x'. The format of 'x' is deduced.

##### M\_FPRINT\_ARG(file, x)

Print into a file 'file' using fprintf the variable 'x'. The format of 'x' is deduced.

##### M\_PRINT(args...)

Print using printf all the variable in 'args'. The format of the arguments are deduced.

##### M\_FPRINT(file, args...)

Print into a file 'file' using fprintf all the variables in 'args'. The format of 'x' is deduced.

##### M\_AS\_TYPE(type, x)

Within a C11 \_Generic statement, all expressions shall be valid C
expression even if the case if always false, and is not executed.
This can seriously limit the _Generic statement.
This macro overcomes this limitation by returning :

* either the input 'x' if it is of type 'type',
* or the value 0 view as a type 'type'.

So the returned value is **always** of type 'type' and is safe in a \_Generic statement.


### C Macro

Theses macros expand their code at compilation level:

##### M\_MIN(x, y)
 
Return the minimum of 'x' and 'y'. x and y shall not have any side effect.

##### M\_MAX(x, y)
 
Return the maximum of 'x' and 'y'. x and y shall not have any side effect.

##### M\_POWEROF2\_P(n)
 
Return true if 'n' is a power of 2. n shall not have any side effect.

##### M\_SWAP(type, a, b)
 
Swap the values of 'a' and 'b'. 'a' and 'b' are of type 'type'. a and b shall not have any side effect.

##### M\_ASSIGN\_CAST(type, a)
 
Check if 'a' can be assigned to a temporary of type 'type' and returns this temporary.
If it cannot, the compilation failed.

##### M\_CONST\_CAST(type, a)
 
Check if 'a' can be properly casted to (const type *) and returns this casted pointer if possible.
If it cannot, the compilation failed.

##### M\_TYPE\_FROM\_FIELD(type, ptr, fieldType, field)
 
Assuming 'ptr' is a pointer to a fieldType object which is stored within a structure of type 'type'
at the position 'field', it returns a pointer to the structure.

#### HASH Functions

##### M\_HASH\_SEED --> size\_t

User shall overwrite this macro by a random seed (of type size_t) before including
the header m-core.h o that all hash functions will use this variable
as a seed for the hash functions. If no user macro is defined,
the default is to expand it to 0,
making all hash computations predictible.

##### M\_HASH\_DECL(hash)

Declare and initialize a new hash computation, named 'hash' which
is an integer.

##### M\_HASH\_UP(hash, value)

Update the 'hash' variable with the given 'value'
by incorporating the 'value' within the 'hash'. 'value' can be up to a 'size_t' 
variable.

##### uint32_t m\_core\_hash (const void *str, size\_t length)

Compute the hash of the binary representation of the data pointer by 'str'
of length 'length'. 'str' shall have the same alignement restriction
than an 'uint32_t'.

#### OPERATORS Functions

##### M\_GET\_INIT oplist
##### M\_GET\_INIT\_SET oplist
##### M\_GET\_INIT\_MOVE oplist
##### M\_GET\_SET oplist
##### M\_GET\_MOVE oplist
##### M\_GET\_SWAP oplist
##### M\_GET\_CLEAR oplist
##### M\_GET\_NEW oplist
##### M\_GET\_DEL oplist
##### M\_GET\_REALLOC oplist
##### M\_GET\_FREE oplist
##### M\_GET\_MEMPOOL oplist
##### M\_GET\_MEMPOOL\_LINKAGE oplist
##### M\_GET\_HASH oplist
##### M\_GET\_EQUAL oplist
##### M\_GET\_CMP oplist
##### M\_GET\_UPDATE oplist
##### M\_GET\_TYPE oplist
##### M\_GET\_SUBTYPE oplist
##### M\_GET\_OPLIST oplist
##### M\_GET\_SORT oplist
##### M\_GET\_IT\_TYPE oplist
##### M\_GET\_IT\_FIRST oplist
##### M\_GET\_IT\_LAST oplist
##### M\_GET\_IT\_END oplist
##### M\_GET\_IT\_SET oplist
##### M\_GET\_IT\_END\_P oplist
##### M\_GET\_IT\_LAST\_P oplist
##### M\_GET\_IT\_EQUAL\_P oplist
##### M\_GET\_IT\_NEXT oplist
##### M\_GET\_IT\_PREVIOUS oplist
##### M\_GET\_IT\_REF oplist
##### M\_GET\_IT\_CREF oplist
##### M\_GET\_ADD oplist
##### M\_GET\_SUB oplist
##### M\_GET\_MUL oplist
##### M\_GET\_DIV oplist
##### M\_GET\_CLEAN oplist
##### M\_GET\_PUSH oplist
##### M\_GET\_POP oplist
##### M\_GET\_REVERSE oplist
##### M\_GET\_GET\_STR oplist
##### M\_GET\_OUT\_STR oplist
##### M\_GET\_IN\_STR oplist
##### M\_GET\_SEPARATOR oplist
##### M\_GET\_EXT\_ALGO oplist
##### M\_GET\_EXPECTED\_SIZE oplist
##### M\_GET\_OOR\_SET oplist
##### M\_GET\_OOR\_EQUAL oplist

Return the associated method to the given operator within the given oplist.

##### M\_DEFAULT\_OPLIST

Default oplist for C standard types (int & float)

##### M\_CSTR\_OPLIST

Default oplist for the C type const char *

##### M\_CLASSIC\_OPLIST(name)

Create the oplist with the classic operators using the pattern 'name', i.e.
name##_init, name##_clear, etc.

##### M\_OPFLAT oplist

Remove the parenthesis around an oplist.

##### M\_OPCAT(oplist1,oplist2)

Concat two oplists in one. 'oplist1''s operators will have higher priority to 'oplist2'

##### M\_OPEXTEND(oplist, ...)

Extend an oplist with the given list of operators.
Theses new operators will have higher priority than the ones
in the given oplist.

##### M\_TEST\_METHOD\_P(method, oplist)

Test if a method is present in an oplist. Return 0 or 1.

##### M\_IF\_METHOD(method, oplist) 

Perfom a preprocessing M\_IF, if the method is present in the oplist.
Example: M\_IF\_METHOD(HASH, oplist)(define function which uses HASH method, ) 

##### M\_IF\_METHOD\_BOTH(method, oplist1, oplist2)     

Perform a preprocessing M\_IF if the method exists in both oplist.
Example: M\_IF\_METHOD\_BOTH(HASH, oplist1, oplist2) (define function , )

##### M\_IF\_METHOD\_ALL(method, ...)

Perform a preprocessing M\_IF if the method exists for all oplist.
Example: M\_IF\_METHOD\_ALL(HASH, oplist1, oplist2, oplist3) (define function, ) 

##### M_IPTR

By putting this after a method for an operator in the oplist,
it specifies that the first argument of the moethod shall be a pointer
to the destination type, rather than the type.

##### M\_DO\_INIT\_MOVE(oplist, dest, src)
##### M\_DO\_MOVE(oplist, dest, src)

Perform an INIT\_MOVE/MOVE if present, or emulate it otherwise.
Note: default methods for INIT\_MOVE/MOVE are not robust enough yet.

#### Syntax enhancing

##### M\_EACH(item, container, oplist)

This macro allows to iterate over the given 'container' of oplist 'oplist'.
It shall be used after the for C keyword.
'item' will be a created pointer variable to the underlying type,
only available within the 'for'.
There can only have one M\_EACH per line.
Example: 
         for M_EACH(item, list, LIST_OPLIST) { action; }

##### M\_LET(var1[,var2[,...]], oplist)

This macro allows to define the variable 'var1'(resp. var2, ...) 
of oplit 'oplist', 
initialize 'var1' (resp. var2, ...) by calling the initialization method,
and clear 'var1' (resp. var2, ...) by calling the initialization method
when the bracket associated to the M\_LET go out of scope.
There can only have one M\_LET per line.
Example:

     M_LET(a, STRING_OPLIST) { do something with a }  or
     M_LET(a, b, c, STRING_OPLIST) { do something with a, b & c }

NOTE: The user code can not perform a return or a goto outside the {}
otherwise the clear code of the object won't be called .

#### Memory functions

##### type *M\_MEMORY\_ALLOC (type)

Return a pointer to a new allocated object of type 'type'.
The object is not initialized.
In case of allocation error, it returns NULL.
The default function is the malloc function.
It can be overriden before including the header m-core.h

##### void M\_MEMORY\_DEL (type *ptr)

Delete the cleared object pointed by the pointer 'ptr'.
The pointer was previously allocated by the macro M\_MEMORY\_ALLOC.
'ptr' can not be NULL.
The default function is the free function.
It can be overriden before including the header m-core.h

##### type *M\_MEMORY\_REALLOC (type, ptr, number)

Return a pointer to an array of 'number' objects of type 'type'.
The objects are not initialized, nor the state of previous objects changed.
'ptr' is either NULL, or pointer returned from a previous call 
of M\_MEMORY\_REALLOC.
In case of allocation error, it returns NULL.
The default function is the realloc function.
It can be overriden before including the header m-core.h

##### void M\_MEMORY\_FREE (type *ptr)

Delete the cleared object pointed by the pointer 'ptr'.
The pointer was previously allocated by the macro M\_MEMORY\_REALLOC.
'ptr' can not be NULL.
The default function is the free function.
It can be overriden before including the header m-core.h
A pointer allocated by M\_MEMORY\_ALLOC can not be freed by this function.

##### void M\_MEMORY\_FULL (size_t size)

This macro is called when a memory exception error shall be raised.
It can be overriden before including the header m-core.h
The default is to abort the execution.
The macro can :

* abort the execution,
* throw an exception (In this case, the state of the object is unchanged),
* set a global error variable and return.

NOTE: The last case is not 100% supported. 

##### void M\_INIT\_FAILURE (void)

This macro is called when an initialization error shall be raised.
It can be overriden before including the header m-core.h
The default is to abort the execution.
The macro can :

* abort the execution,
* throw an exception (In this case, the state of the object is unchanged),
* set a global error variable and return.

NOTE: The last case is not currently supported. 

##### void M\_ASSERT\_INIT\_FAILURE(expression)

This macro is called when an assertion in an initialization context
is called.
If the expression is false, the execution is aborted.
The assertion is kept in release programs.
It can be overriden before including the header m-core.h
The default is to abort the execution.


### M-MUTEX

This header is for providing very thin layer around OS implementation of threads, conditional variables and mutexs.
It has backends for WIN32, POSIX thread or C11 thread.

It was needed due to the low adoption rate of the C11 equivalent layer.

Example:

	m_thread_t idx_p;
	m_thread_t idx_c;

	m_thread_create (idx_p, conso_function, NULL);
	m_thread_create (idx_c, prod_function, NULL);
	m_thread_join (idx_p;
	m_thread_join (idx_c);

#### methods

The following methods are available:

#### m\_mutex\_t

A type representating a mutex.

##### void m\_mutex\_init(mutex)

Initialize the variable mutex of type m\_mutex\_t. 
If the initialization fails, the program aborts.

##### void m\_mutex\_clear(mutex)

Clear the variable mutex of type m\_mutex\_t. 
If the variable is not initialized, the behavior is undefined.

##### void m\_mutex\_lock(mutex)

Lock the variable mutex of type m\_mutex\_t for exclusive use.
If the variable is not free, it will wait indefinitely until it is.
If the variable is not initialized, the behavior is undefined.

##### void m\_mutex\_unlock(mutex)

Unlock the variable mutex of type m\_mutex\_t for exclusive use.
If the variable is not locked, the behavior is undefined.
If the variable is not initialized, the behavior is undefined.

##### M\_LOCK\_DECL(name)

Define the lock 'name'. This shall be called in the global space (reserved for global variables).

##### M\_LOCK(name)

Use the lock 'name': the encapsulation instructions are protected by the lock.
Example:

        M_LOCK_DECL(n_lock);
        unsigned long n = 0;
        void f(void) {
             M_LOCK(n_lock) {
               n ++;
             }
        }

#### m\_cond\_t

A type representating a conditionnal variable, used within a mutex section.

##### void m\_cond\_init(m\_cond\_t cond)

Initialize the conditional variable cond of type m\_cond\_t. 
If the initialization failed, the program aborts.

##### void m\_cond\_clear(m\_cond\_t cond)

Clear the variable cond of type m\_cond\_t. 
If the variable is not initialized, the behavior is undefined.

##### void m\_cond\_signal(m\_cond\_t cond)

Within a mutex exclusive section,
signal that the event associated to the variable cond of type m\_cond\_t 
has occured to other threads.
If the variable is not initialized, the behavior is undefined.

##### void m\_cond\_wait(m\_cond\_t cond, m\_mutex\_t mutex)

Within a mutex exclusive section defined by mutex,
wait indefinitely for the event associated to the variable cond of type m\_cond\_t
to occur.
IF multiple threads wait for the same event, which thread to awoken
is not specified.
If any variable is not initialized, the behavior is undefined.

#### m\_thread\_t

A type representating an id of a thread.

##### void m\_thread\_create(m\_thread\_t thread, void (\*function)(void\*), void \*argument)

Create a new thread and set the it of the thread to 'thread'.
The new thread run the code function(argument) with
argument a 'void \*' and function taking a 'void \*' and returning
nothing.
If the initialization fails, the program aborts.

##### void m\_thread\_join(m\_thread\_t thread)

Wait indefinetly for the thread 'thread' to exit.



### M-ALGO

This header is for generating generic algorithm to containers.

#### ALGO\_DEF(name, container_oplist)

Define the available algorithms for the container which oplist is container_oplist.
The defined algorithms depend on the availability of the methods of the containers
(For example, if there no CMP operation, there is no MIN operation defined).

Example:

	ARRAY_DEF(array_int, int)
	ALGO_DEF(array_int, ARRAY_OPLIST(int))
	void f(void) {
		array_int_t l;
		array_int_init(l);
		for(int i = 0; i < 100; i++)
			array_int_push_back (l, i);
		array_int_push_back (l, 17);
		assert( array_int_contains(l, 62) == true);
		assert( array_int_contains(l, -1) == false);
		assert( array_int_count(l, 17) == 2);
		array_int_clear(l);
	}

#### Created methods

The following methods are automatically and properly created by the previous macros. In the following methods, name stands for the name given to the macro which is used to identify the type.

TODO: document the API.

### M-MEMPOOL

This header is for generating specialized optimized memory pools:
it will generate specialized functions to alloc & free only one kind of an object.
The mempool functions are not thread safe for a given mempool.

#### MEMPOOL\_DEF(name, type)

Generate specialized functions & types prefixed by 'name' 
to alloc & free a 'type' object.

Example:

	MEMPOOL_DEF(mempool_uint, unsigned int)

	void f(void) {
          mempool_uint_t m;
          mempool_uint_init(m);
          unsigned int *p = mempool_uint_alloc(m);
          mempool_uint_free(m, p);
          mempool_uint_clear(m);
        }

#### Created methods

The following methods are automatically and properly created by the previous macros. In the following methods, name stands for the name given to the macro which is used to identify the type.

##### name\_t

The type of a mempool.

##### void name\_init(name\_t m)

Initialize the mempool 'm'.

##### void name\_clear(name\_t m)

Clear the mempool 'm'.
All allocated objects associated to the this mempool which weren't explicitly freed
will be deleted too (without calling their clear method).

##### type *name\_alloc(name\_t m)

Create a new object of type 'type' and return a new pointer to the uninitialized object.

##### void name\_free(name\_t m, type *p)

Free the object 'p' created by the call to name\_alloc.
The clear method of the type is not called.


