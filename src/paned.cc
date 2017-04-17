#include "paned.hh"

#include "verbose.hh"

#include <gtk/gtkpaned.h>
#include <gtk/gtkhpaned.h>
#include <gtk/gtkvpaned.h>


void Paned::Wdg_Append_Child(Widget &widget/*, bool*/)
{
	Verbose_Function();
	Verbose_Int(next);
	if (next==0)
	{
		gtk_paned_add1(GTK_PANED(Get()), widget.Get());
		++next;
	}
	else if (next==1)
	{
		gtk_paned_add2(GTK_PANED(Get()), widget.Get());
		++next;
		int w= -1, h;
		bool r;
		widget.Wdg_Get_Size_Request(w, h, r);
		Verbose_Int(w);
		gtk_paned_set_position(GTK_PANED(Get()), w);
	}
	else
		assert(0);
	Verbose_Int(next);
}

bool H_Paned::Open(Pointer <Widget> parent)
{
	next= 0;
	return Widget::Open(parent, gtk_hpaned_new());
}

bool V_Paned::Open(Pointer <Widget> parent)
{
	next= 0;
	return Widget::Open(parent, gtk_vpaned_new());
}
