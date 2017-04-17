#ifndef CASSERT_HH
#define CASSERT_HH

/* Like assert, but if the expression is a constant, fail at compile
 * time.  This functionality should and is maybe already in GNU. 
 */

#ifdef NDEBUG

#   define cassert(test)  ((void)0)

#else

#   define cassert(test)  (__builtin_constant_p(test)?({ int a[test ? \
    1 : -1]; a[0]= a[0]= 0; }): (assert (test),0))

#endif

#endif /* ! CASSERT_HH */
