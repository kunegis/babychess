#include "game_info_editor.hh"

#include "format.hh"
#include "gettext.hh"
#include "piece.hh"
#include "time.hh"

#include <limits.h>
#include <stdio.h>
#include <time.h>
#include <gtk/gtklabel.h>
#include <gtk/gtktable.h>

bool Game_Info_Editor::OE_Open(Pointer <Widget> parent,
			       const Game_Info &game_info)
{
	if (! entry_box.Open(parent, _("Game tags")))  
		return false;
//	if (! label.Open(parent, _("Game tags")))
//		return false;
	if (! box.Open(&entry_box(), 2))  return false; 
	gtk_table_set_homogeneous(GTK_TABLE(box.Get()), FALSE);
	gtk_table_set_col_spacings(GTK_TABLE(box.Get()), dialog_unit);
	
	String date_text= game_info.Format_Date_Locally(); 
	
	Spin_Button::Open_Info oi= {"", game_info.first_move_number, 1, INT_MAX};

	if (! (labels[4].Open(box, Format("%s%s",I_("player:_White"),_(":")), 0.0, "", true) &&
	       entries[4].Open(box, game_info.players[cc_w])                 &&
	       labels[5].Open(box, Format("%s%s", I_("player:_Black"), _(":")),0.0,"", true) &&
	       entries[5].Open(box, game_info.players[cc_b])))
		return false;
	if (! (labels[0].Open(box, Format("%s%s",_("_Event"), _(":")),0.0,"",true)      &&
	       entries[0].Open(box, game_info.event)                 &&
	       labels[1].Open(box, Format("%s%s",_("_Site"), _(":")),0.0,"",true)      &&
	       entries[1].Open(box, game_info.site)                  &&
	       labels[2].Open(box, Format("%s%s",_("_Date"),_(":")), 0.0, "", true)    &&
	       entries[2].Open(box, date_text)                       && 
	       labels[3].Open(box, Format("%s%s",_("_Round"),_(":")), 0.0, "", true)   &&
	       entries[3].Open(box, game_info.round)                 &&
	       labels[6].Open
	       (box, Format("%s%s",_("_First move number"),_(":")), 0.0, "", true) &&
	       first_move_button.Open(box, oi)))
		return false; 

	for (int i= 0; i < 6; ++i)
		gtk_label_set_mnemonic_widget(GTK_LABEL(labels[i].Get()), 
					      entries[i].Get());
	gtk_label_set_mnemonic_widget(GTK_LABEL(labels[6].Get()), first_move_button.Get()); 

	if (! result_chooser.Open(parent, "", _("Game outcome"), 2))  return false;

	result_chooser() << Format("1–0 %s", _("_White victory"))
			 << Format("½–½ %s", _("_Draw"))
			 << Format("0–1 %s", _("_Black victory"))
			 << _("_Unfinished");

	result_chooser->Set_Selection((int) game_info.result); 
		
	return true; 
}

void Game_Info_Editor::Set_Result(int result)
{
	result_chooser->Set_Selection(result); 
}

bool Game_Info_Editor::OE_Retrieve(Game_Info &game_info)
{
	Verbose_Function();
	
	game_info.event= entries[0].Text();
	game_info.site= entries[1].Text();

	String time_F_text= entries[2].Text();
	Verbose_Str(time_F_text());
	struct timeval time;
	time= Parse_Time(time_F_text(), "%x");	
	struct tm *tm;
	if (time.tv_sec >= 0 && (tm= localtime(& time.tv_sec)))
	{
		game_info.date_year=  1900 + tm->tm_year;
		game_info.date_month= tm->tm_mon;
		game_info.date_day=   tm->tm_mday;
	}
	else
	{
		if (1 != sscanf(time_F_text(), "%d", &game_info.date_year))
			game_info.date_year= -1;
		game_info.date_month= game_info.date_day= -1;
	}
	Verbose_Int(game_info.date_year);

	game_info.round= entries[3].Text();
	game_info.players[cc_w]= entries[4].Text();
	game_info.players[cc_b]= entries[5].Text();
	int result= result_chooser->Selection(); 
	assert (result != -1);
	game_info.result= result; 
	game_info.first_move_number= first_move_button.Value();
	assert (game_info.first_move_number >= 1);
	return true; 
}
