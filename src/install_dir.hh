#ifndef INSTALL_DIR_HH
#define INSTALL_DIR_HH

/* Locate files used at runtime.  Return the name of the directory. 
 *
 *     id_bin:    directory of the executables
 *     id_files:  directory of all other files (COPYING, AUTHORS, share/, ...) 
 *     id_base:   base installation directory (usually /usr or ~)
 */

#include "string.hh"

enum  Install_Dir_Type {id_base, id_bin, id_files};

const String &Install_Dir(Install_Dir_Type= id_files);

#endif /* ! INSTALL_DIR_HH */
