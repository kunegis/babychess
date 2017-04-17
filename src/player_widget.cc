#include "player_widget.hh"

#include <gtk/gtktable.h>

bool Player_Widget::Open(Pointer <Widget> parent,
			 const Player &player,
			 int color)

{
	if (! V_Box::Open(parent, dialog_unit))  return false;
	if (! type_box.Open(this, dialog_unit))    goto close;

		if (! color_box.Open(type_box, dialog_unit/2))  goto close;
		if (! piece.Open(color_box, Piece(color, pi_K)))  goto close; 
		if (! color_label.Open
		    (color_box, 
		     color ? I_("player:Black") : I_("player:White"), 
		     0.5,
		     "size=\"xx-large\" weight = \"bold\""))  goto close;

	type_box.Set_Next(true); 
	if (!type.Open(type_box, "type-change", 2))
		goto close; 
	type.width= 2;
//	type.fill= true;
	blank.Open(type);
	type.space++; 
	gtk_table_set_homogeneous(GTK_TABLE(type.Get()), FALSE);
	type << _("_Human");
	type.space++;
	type << _("_Engine:"); 

	type.Set_Selection(player.type); 
	if (! engine_menu.Open(type, player.engine_conf_name, "engine_change"))  
		goto close; 

	if (! name.Open(this, _("Player _name")))
		goto close; 
	MR_Receive("type-change"); 
	if (player.type == p_engine)
		MR_Receive("engine_change");
	else
		name.entry.Set(player.name); 
	return true;

 close:
	Close();
	return false;
}

void Player_Widget::Set(const Player &player)
{
	assert (player.type >= 0 && player.type < p_count);
	type.Set_Selection(player.type);
	engine_menu.Update(player.engine_conf_name);
	MR_Receive("type-change"); 
	if (player.type == p_engine)
		MR_Receive("engine_change"); 
}

void Player_Widget::Retrieve(Player &player)
{
	player.type= type.Selection();
	if (player.type < 0)  player.type= 0;
	player.engine_conf_name= engine_menu.Retrieve(); 
	player.name= name.entry.Text();
}

void Player_Widget::MR_Receive(String message)
{
	if (message == "type-change")
	{
		gtk_widget_set_sensitive(GTK_WIDGET(engine_menu.Get()),
					 type.Selection() == 1);
	}

	else if (message == "engine_change")
	{
		name.entry.Set(engine_menu.Name());
	}

	else  V_Box::MR_Receive(message); 
}

