#ifndef ARG_MAIN_HH
#define ARG_MAIN_HH

/* Implements Main() and takes care of the meta-options --version and --help.
 * Calls the function Arg_Main() that must be implemented.  Also loads the
 * locale-dependent translations.   
 */

int Arg_Main(int argc, char **argv);
/* To be defined:
 *
 * Function called.  ARGC and ARGV as in main(). 
 */

#endif /* ! ARG_MAIN_HH */
