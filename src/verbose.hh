#ifndef VERBOSE_HH
#define VERBOSE_HH

/* Macros that output debugging information.  Only enabled in debug
 * mode (when NDEBUG is not defined), and when <MODULE>_VERBOSE is set in
 * the environment.  E.g.:  Set POSITION_VERBOSE=1 when starting any
 * program to enable debugging info for the POSITION module.   Or:
 * BABYCHESS_BOOK_VERBOSE=1 for src/babychess-book.cc. 
 * 
 * Verbose() is the central macro which takes arguments just as printf(),
 * but whithout the newline.  The variations of the form Verbose_%() 
 * specialize the argument.  
 */

/* Macro:  VERBOSE_ENV_VAR_NAME (optional) :  env variable name to use 
 */

#ifndef NDEBUG

#include <stdio.h>
#include <unistd.h> /* getpid() */ 

#include "main.hh"

extern FILE *verbose_file;

bool Verbose_Enabled(const char *source_filename);

/* The name of the module is determined by reading __FILE__ and
 * checking the corresponding variable in the environment.
 * Example:   __FILE__ = "position_editor.cc"
 *        --> check env("POSITION_EDITOR_VERBOSE")
 */
#define Verbose(format, args...)                           \
          (Verbose_Enabled(__FILE__) &&                    \
           fprintf(verbose_file, format "\n" , ## args))
/* There must be a space before the last comma for ## to work. */

#else /* NDEBUG */
#    define Verbose(args...)  ((void) 0)
#endif /* NDEBUG */ 

/*
 * Variations 
 */

/* Name of the current function.  Used at the beginning of functions.  */
#define Verbose_Function()    Verbose("%s", __PRETTY_FUNCTION__)


/* Prints the name and value of an integral expression.  Also used for
 * bool, etc... */
#define Verbose_Int(arg) Verbose("\t" #arg "= \t%d", arg)

/* const char * */
#define Verbose_Str(arg)  Verbose("\t" #arg "= \t\"%s\"", arg)

/* Pointer */
#define Verbose_Pointer(arg) Verbose("\t" #arg "= \t%p", arg)

/* String (+ internal info) */
#define Verbose_String(arg)  Verbose( #arg "= \"%s\"[%d:%d,%d,%d]", arg(), \
              ((const char *)arg.Raw())[0], *arg, arg.Ref_Count(), arg.Place())

#define Verbose_Process(format, args...)  Verbose("[%u] %s:  " format,\
     getpid(), program_invocation_short_name , ## args)

#define Verbose_Small_Move(move) \
  Verbose( #move "= %c%c-%c%c  ep = %d, prom = %c, capt = %c", \
           'a'+File(move&0xFF),                                \
           '1'+Rank(move&0xFF),                                \
	   'a'+File(move>>8&0xFF),                             \
           '1'+Rank(move>>8&0xFF),                             \
           move>>16&0xFF,                                      \
           "xNBRQx  -"[move>>24&0xF],                              \
           "PNBRQK  -"[move>>28&0xF])

#endif /* ! VERBOSE_HH */
