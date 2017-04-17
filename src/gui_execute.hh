#ifndef GUI_EXECUTE_HH
#define GUI_EXECUTE_HH

/* For GUI programs:  Execute another GUI program.
 */

#include "string.hh"
#include "pointer.hh"
#include "vector.hh"
#include "widget.hh"

bool _GUI_Execute(//String mode, /* e.g. edit-game or play */
//	const char *mode, /* may be NULL or e.g. "--play" */ 
	Pointer <Widget> = NULL,
//	const char *const args[]= NULL,
	
//		 Pointer <const Vector <String> > args= NULL,
	const char *mode= NULL,
	const char *arg1= NULL,
	const char *arg2= NULL);

/* Execute BabyChess in another process.  The program is executed
 * whithout waiting for termination.  Return whether the program was
 * started.  WIDGET is used as the window that the error message will be
 * transient for.       
 */

#if 0
Inline bool GUI_Execute(const char *mode,
			Pointer <Widget> widget)
{
	const char *const args[]= {NULL, mode, NULL};
	return GUI_Execute(mode, NULL, widget);
}

Inline bool _GUI_Execute(const char *mode, 
			const char *arg,
			Pointer <Widget> widget)
{
	const char *const args[]= {NULL, mode, arg, NULL};
//	Vector <String> args;
//	if (! args.Set(1))
//		return false;
//	args[0]= arg;
	return GUI_Execute(args, widget);
}

Inline bool _GUI_Execute(const char *mode, 
			const char *arg1, 
			const char *arg2,
			Pointer <Widget> widget= NULL)
{
	const char *const args[]= {NULL, mode, arg1, arg2, NULL};
//	Vector <String> args;
//	if (! args.Set(2))
//		return false;
//	args[0]= arg1;
//	args[1]= arg2;
	return GUI_Execute(args, widget);
}
#endif /* 0 */ 

#endif /* ! GUI_EXECUTE_INCLUDED */
