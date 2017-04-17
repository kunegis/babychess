#include "game_list.hh"

#include "format.hh"

bool Game_List::Open(Pointer <Widget> parent,
		     const Game &new_game,
		     const Notation &new_notation,
		     const Notation_Settings &new_notation_settings,
		     const char *clicked)
{
	if (! List::Open(parent, 
			 List::Open_Info
			 (3,
			  scr_vertical,
			  "clicked")))
		return false; 
	(*this)->Set_Column_Right_Aligned(0);
	move_list_length= -1;
	move_list_first= -1;
	game= new_game; 
	notation= new_notation; 
	notation_settings= new_notation_settings; 
	msg= clicked; 
	location= -1;
	return true;
}

void Game_List::MR_Receive(String message)
{
	if (message == "clicked")
	{
		int row, col;
		(*this)->Clicked(row, col);
		Verbose("Game_List:: clicked(row = %d, col = %d)",
		    row, col);

		if (col == 0 && row)  
			col= 1; 
		/* overall index to go to */
		location= row * 2 + col;
		if (game->First_Move() == cc_b)
			-- location;
		
		if (location < 0)  location= 0;

		if (location == game->Overall_Location())  return;
		if (location > game->Overall_Length())     
			location= game->Overall_Length();
		return List::MR_Receive(msg); 
	}
	else
		List::MR_Receive(message); 
}

void Game_List::Update_Length()
{
	int new_length= game->Overall_Length();
	int new_first= game->Starting_Position()
		? game->Starting_Position()().previous_move_count / 2 + 1
		: 1;
	if (new_length == move_list_length && new_first == move_list_first)  return;

	/* number of lines */
	int color= game->First_Move();
	int cells= new_length + color;
	int row_count= (cells + 1) / 2;
	if (row_count == 0)  row_count= 1;
	(*this)->Set_Row_Count(row_count);


	/* move numbers */

	for (int i= new_first == move_list_first ? (move_list_length+color+1)/2 : 0 ; 
	     i < row_count;  ++i)
	{

		(*this)->Set(0, i, Format("%d.", i + new_first));
	}
	
	/*  "..." before first move if black begins */
	if (color)
		(*this)->Set(1, 0, "...");

	/* "" twice on first line if the game is empty */
	if (new_length == 0)
	{
		(*this)->Set(1, 0, "");
		(*this)->Set(2, 0, "");
	}

	/* "" at the end if the last move is white's */
	if (cells % 2 && move_list_length > new_length)
		(*this)->Set(2, row_count-1, "");

	/* end */
	move_list_length= new_length;
	move_list_first=  new_first; 
}

void Game_List::Update_Content(int begin, int end)
{
	if (end < 0)    end= game->Overall_Length();
	assert (begin <= end); 
	int color= game->First_Move(); 
	for (int i= begin;  i < end;  ++i)
	{
		String text= game->Overall_Move_Text(i, notation(), notation_settings());
		(*this)->Set ((i + color) % 2 + 1,
				     (i + color) / 2,
				     text);
	}
}

void Game_List::Update_Highlight()
{
	if (game->Overall_Location() > 0)
	{
		int index= game->Overall_Location()
			+ (game->First_Move() == cc_b ? 1 : 0) - 1; 
		(*this).Set_Selection(index / 2, true);
	}
	else
		(*this).Set_Selection(0, true);
}

void Game_List::Update_Content_One(int location)
{
	(*this)->Set
		((location+game->First_Move()) % 2 + 1,
		 (location+game->First_Move()) / 2,
		 game->Overall_Move_Text
		 (location, notation(), notation_settings())); 
}

void Game_List::Wdg_Close()
{
	game= NULL;
	notation= NULL; 
	notation_settings= NULL;
	List::Wdg_Close();
}
