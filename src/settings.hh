#ifndef SETTINGS_HH
#define SETTINGS_HH

/* Setttings files in the home directory in ~/.babychess */

#include "filename.hh"
#include "inline.hh"
#include "dir.hh"
#include "stream_file.hh"

#include <glib/gutils.h>

/* Name of the settings directory in the user's home directory */
#ifndef SETTINGS_DIR_NAME
#   define SETTINGS_DIR_NAME   ".babychess"
#endif

const char *Settings_Dir();
/* The settings directory.  Create it if necessary. */ 

const char *Settings_Tmp();
/* Return ~/.babychess/tmp, creating it if necessary.  Return NULL on
 * error. 
 */

//Inline String Settings_Filename(const char *name)
//{
//	return String(g_get_home_dir()) / SETTINGS_DIR_NAME / name;
//}

/* Load and save for settings that don't have configurations */
template <class Type>
Inline bool Settings_Simple_Load(Type &object, const char *name)
{
	assert (name); 
	String filename= 
		String(Settings_Dir())/name;
	return Read_From_File(filename(), object);
}

template <class Type>
Inline void Settings_Simple_Load_Or_Default(Type &object, const char *name)
{
	if (! Settings_Simple_Load(object, name))
		object.Reset();
}

template <class Type>
Inline bool Settings_Simple_Save(const Type &object, const char *name)
{
	assert (name);
	String filename= 
		String(Settings_Dir())/name;
	return Write_To_File(filename(), object);
}

#endif /* ! SETTINGS_HH */
