#include "engine_menu.hh"

#include "gettext.hh"

bool Engine_Menu::Open(Pointer <Widget> parent, 
		       String new_name,
//		       const Engine &new_engine,
		       String new_msg_change)
{
//	engine= new_engine;
	msg_change= new_msg_change; 

	if (! H_Box::Open(parent, dialog_unit/2))   return false;

	H_Box::Set_Next(true);
	if (! menu.Open(this, "menu_change"))  goto close;
	Update_List(new_name);
	
	if (! button.Open(this, "other", _("_Edit...")))
		goto close;

	return true;

 close: Close();
	return false;
}

void Engine_Menu::MR_Receive(String message)
{
	if (message == "other")
	{
		int sel= menu.Selection();
		String filename= conf_dir->Filename(sel);
		if (! Read_From_File(filename(), engine))
		{
			filename= Conf_Saved_Filename(((Engine_Editor *)NULL)->Get_Type_Name());
			if (! Read_From_File(filename(), engine))
				engine.Reset();
		}
		engine_editor.Open("edited", *this, engine, this);
	}
	else if (message == "edited")
	{
		conf_dir.reset(); 
		menu.Clear();
		Update_List();
	}

	else if (message == "menu_change")
		Parent_Receive_Message(msg_change); 

	else
		H_Box::MR_Receive(message);
}

void Engine_Menu::Wdg_Close()
{
	engine_editor.Close(); 
	H_Box::Wdg_Close(); 
}

#if 0
void Engine_Menu::Retrieve(Engine &new_engine)
{  
	int selection= menu.Selection();

	if (selection >= 0)
	{
		String filename= 
			(selection < conf_dir->Stock_Count() 
			 ? conf_dir->Stock_Dir_Name() 
			 : conf_dir->Custom_Dir_Name()) / 
			conf_dir->Name(selection);
		Read_From_File(filename(), engine);
	}
	
	new_engine= engine; 
}
#endif /* 0 */ 

String Engine_Menu::Retrieve() const
{
	int selection= menu.Selection();
	if (selection < 0)
		return "";
	return conf_dir->Name(selection);
}

void Engine_Menu::Update_List(String new_name)
{
	if (new_name == "")
		new_name= Conf_Exact(((Engine_Editor *)NULL)->Get_Type_Name());
//	String current= new_
	int current_index= -1;
	int i= 0;
	conf_dir.reset(new Conf_Dir("engine"));
	String name;
	while (name= conf_dir->Next())
	{
		menu << name; 
		if (name == new_name)
			current_index= i;
		++i; 
	}
	menu.Update();
	if (current_index >= 0) menu.Set_Selection(current_index); 
}

String Engine_Menu::Name() const
{
	String ret= conf_dir->Name(menu.Selection());
	if (menu.Selection() < conf_dir->Stock_Count())
		ret= _(ret()); 
	return ret; 
}

void Engine_Menu::Update(/*const Engine &*/String engine_conf_name)
{
	Update_List(engine_conf_name);
}

