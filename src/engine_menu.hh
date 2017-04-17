#ifndef ENGINE_MENU_HH
#define ENGINE_MENU_HH

#include "option_menu.hh"
#include "box.hh"
#include "label.hh"
#include "text_button.hh"
#include "engine.hh"
#include "engine_editor.hh"
#include "owned_editor.hh"

class Engine_Menu
	:  public H_Box
{
public:
	bool Open(Pointer <Widget> parent, 
		  String engine_conf_name,
//		  const Engine &,
		  String msg_change= "");
	void Update(/*const Engine &*/String engine_conf_name);
	String Retrieve() const;
	String Name() const;

private:
	Option_Menu menu;
	Text_Button button; 

	std::auto_ptr <Conf_Dir> conf_dir;
	int stock_count; 
	Owned_Editor <Engine_Editor> engine_editor; 
	Engine engine; /* only used for engine_editor */ 

	String msg_change;

	void Wdg_Close();
	
	void MR_Receive(String); 

	void Update_List(String new_conf_name= ""); 
};

#endif /* ! ENGINE_MENU_HH */ 

