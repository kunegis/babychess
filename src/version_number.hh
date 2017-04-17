#ifndef VERSION_NUMBER_HH
#define VERSION_NUMBER_HH

#include "../config.hh"
#include "gettext.hh"

const char *Version_Date(); 
/* The compile date in local format  */

#define COPYRIGHT_LINE N_("Copyright © 2000-2005 Jérôme KUNEGIS")
/* We go through gettext not to translate the line but to convert it to
 * the local encoding. */ 

#endif /* ! VERSION_NUMBER_HH */
