#include "gui_execute.hh"

#include "execute.hh"
#include "inform.hh"
#include "format.hh"
#include "install_dir.hh"
#include "filename.hh"
#include "gettext.hh"

bool _GUI_Execute(//String program_name,
//	const char *mode,
//		 const char *const args[],
//		 Pointer <const Vector <String> > args,
	Pointer <Widget> widget,
	const char *mode,
	const char *arg1,
	const char *arg2)
{
//	Vector <String> a;
//	if (args)  a= args(); 

//	if (program_name)
//	{
//		if (! a.Set(*a+1))  return false;
//		for (int i= *a-1; i > 0; --i)
//			a[i]= a[i-1];
//		a[0]= "--" + program_name; 
//	}
	
	String filename= Install_Dir(id_bin) / "babychess";

	const char *const args[]= {NULL, mode, arg1, arg2, NULL};
	
	if (! Execute(filename(), args))
	{
		
		String text= Format(_("Cannot execute \"%s\""), 
				    "babychess");
		Inform_Error(text, widget); 
		return false;
	}

	return true;   
}
