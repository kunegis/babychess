#include "text_view.hh"

#include <gtk/gtksignal.h>
#include <gtk/gtktextview.h>

void Text_View::Append_Text(String text )
{
	assert (Is()); 
	gtk_text_buffer_insert_at_cursor(gtk_text_view_get_buffer(GTK_TEXT_VIEW(Get())),
					 text(),
					 *text);
}

bool Text_View::Open(Pointer <Widget> parent,
		     String text,
		     bool editable,
		     const char *new_msg_changed)
{
	if (! Widget::Open(parent, gtk_text_view_new()))
		return false;
	gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW(Get()),
				     GTK_WRAP_WORD);

	gtk_text_view_set_editable(GTK_TEXT_VIEW(Get()),
				   editable); 

	Append_Text(text);

	msg_changed= NULL;

	g_object_connect(gtk_text_view_get_buffer(GTK_TEXT_VIEW(Get())),
			 "signal::changed",
			 &Signal_Changed,
			 this,
			 NULL);

//	gtk_signal_connect(GTK_OBJECT(gtk_text_view_get_buffer(GTK_TEXT_VIEW(Get()))), 
//			   "changed",
//			   GTK_SIGNAL_FUNC(Signal_Changed), 
//			   this);
	msg_changed= new_msg_changed;
	
	return true;
}

String Text_View::Get_Text() const
{
	GtkTextBuffer *buffer= gtk_text_view_get_buffer(GTK_TEXT_VIEW (Get()));
	assert (buffer);
	
	GtkTextIter start, end;

	gtk_text_buffer_get_start_iter(buffer, 
				       & start);
	gtk_text_buffer_get_end_iter(buffer, 
				     & end);
	char *text= gtk_text_buffer_get_text(buffer,
					     & start,
					     & end,
					     FALSE);

	String result(text);
	result.Isolate();
	g_free(text);
	return result; 
}

void Text_View::Wdg_Get_Size_Request(int &width,
				      int &height,
				      bool &resizeable)
{
	width= 200;
	height= 150; 
	resizeable= true; 
}

void Text_View::Scroll_End()
{
	/* place cursor at the end */ 
	GtkTextIter end_iter;
	GtkTextBuffer *buffer= gtk_text_view_get_buffer(GTK_TEXT_VIEW(Get()));
	gtk_text_buffer_get_end_iter(buffer, &end_iter);
	gtk_text_buffer_place_cursor(buffer, &end_iter); 

	/* scroll to make cursor visible */
	GtkTextMark *end_mark= gtk_text_buffer_create_mark(buffer, "end", &end_iter, FALSE);
	gtk_text_view_scroll_to_mark(GTK_TEXT_VIEW(Get()),
				     end_mark,
				     0.0,FALSE,0.0,0.0);
}

void Text_View::Clear()
{
	gtk_text_view_set_buffer(GTK_TEXT_VIEW(Get()),
				 gtk_text_buffer_new(NULL));

}

void Text_View::Signal_Changed(GtkTextBuffer *,
			       gpointer me)
{
	Text_View &I= *(Text_View *)me;
	if (I.msg_changed)
		I.Parent_Receive_Message(I.msg_changed);
}
