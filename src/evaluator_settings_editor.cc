#include "evaluator_settings_editor.hh"

#include "gettext.hh"
#include "cassert.hh"

bool Evaluator_Settings_Editor::OE_Open(Pointer <Widget> parent,
					const Evaluator_Settings &settings)
{
	/* Start */ 
	if (! mode_chooser.Open(parent, "", _("Mode"), 1))
		return false;

	mode_chooser() << _("_Off")
		       << _("_Analyze")
		       << _("Engine plays _one side")
		       << _("Engine plays _both sides");
	mode_chooser->Set_Selection(settings.mode);
//	start_chooser() << _("_Never")
//			<< _("After each _user move")
//			<< _("_Always");
//	start_chooser->Set_Selection(settings.start);

	/* Timing */ 
	if (! timing_chooser.Open(parent, "timing_changed", _("Stop the Engine"), 2))  
		return false;
	cassert (ti_count == 3);
	timing_chooser() << _("_Never")
		  << _("After a _fixed time")
		  << _("After a fixed _depth");
	timing_chooser->Set_Selection(settings.timing);

	{
		Spin_Button::Open_Info oi= {"", settings.time_s, 1, INT_MAX};
		String label= Format("%s (s)", _("_Time"));
		if (! time.Open(parent, label(), oi))  return false; 
	}
	{
		Spin_Button::Open_Info oi= {"", settings.depth, 1,
					    INT_MAX};
		if (! depth.Open(parent, _("_Depth"), oi)) return false;
	}

	/* Flags */ 
	if (! flags_box.Open(parent))  return false;
	for (int i= 0;  i < evf_count;  ++i)
	{
		cassert (evf_count == 1);
		static const char *const text[evf_count]=
			{
				N_("Run engine with high _priority") ,
//			 N_("_Play move when found"),
//			 N_("Start engine when there is _one legal
//			 move")};
			};
		if (! flags[i].Open(flags_box,
				    settings.flags & (1 << i),
				    _(text[i])))
			return false;
	}

	MR_Receive("timing_changed");

	return true; 
}


bool Evaluator_Settings_Editor::OE_Retrieve(Evaluator_Settings &settings)
{
	settings.mode= mode_chooser->Selection();
	if (settings.mode < 0)
		settings.mode= 0;
	settings.timing= (Timing) timing_chooser->Selection();
	if (settings.timing < 0)
		settings.timing= (Timing) 0;
	settings.time_s= time.button.Value();
	settings.depth= depth.button.Value();
//	settings.start= start_chooser->Selection(); 

	settings.flags= 0;
	for (int i= 0;  i < evf_count;  ++i)
		if (flags[i].Checked())
			settings.flags |= (1 << i);

	return true; 
}

void Evaluator_Settings_Editor::MR_Receive(String message)
{
	if (message == "timing_changed")
	{
		gtk_widget_set_sensitive
			(time.button.Get(), 
			 timing_chooser->Selection() == ti_time);
		gtk_widget_set_sensitive
			(depth.button.Get(),
			 timing_chooser->Selection() == ti_depth);
	}
	else
		Object_Editor <Evaluator_Settings> ::MR_Receive(message); 
}
