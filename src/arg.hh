#ifndef ARG_HH
#define ARG_HH

extern void Arg_Print_Help(); 
/* To be defined.  The help text output on "--help"; 
 */

void Arg(int argc, char **argv);
/* Check ARGS for the meta-options --version and --help.  If found, process the
 * option and exit.   
 */

#endif /* ! ARG_HH */
