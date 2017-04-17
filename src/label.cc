#include "label.hh"

#include "format.hh"

#include <gtk/gtklabel.h>

bool Label::Open(Pointer <Widget> parent,
		 const Open_Info &open_info)
{
	assert (open_info.alignment >= 0 && 
		open_info.alignment < 3);

	if (! Widget::Open(parent, gtk_label_new("")))
		return false;
	
	gtk_misc_set_alignment (GTK_MISC (Get()), 
				open_info.alignment,
				0.5);

	span_attributes= open_info.span_attributes;
	with_mnemonic=   open_info.with_mnemonic;

	Set(open_info.text);

	return true;
}

String Label::Text() const
{
	gchar *text;
	gtk_label_get(GTK_LABEL(Get()), & text);
	String result(text);
	result.Isolate();
	return result; 
}

void Label::Set(String text)
{
	assert (Is());

	if (span_attributes == "")
	{
		if (with_mnemonic)
			gtk_label_set_text_with_mnemonic(GTK_LABEL(Get()), text()); 
		else 
			gtk_label_set_text(GTK_LABEL(Get()), text()); 
	}
	else if (span_attributes[0] == '<')
	{
		gtk_label_set_markup(GTK_LABEL(Get()), text());
	}
	else
	{
		char *m= g_markup_escape_text(text(), *text);
		
		String markup= Format("<span %s>%s</span>",
				      span_attributes(),
				      m);
		g_free(m); 
		gtk_label_set_markup(GTK_LABEL(Get()), markup()); 
	}
}
