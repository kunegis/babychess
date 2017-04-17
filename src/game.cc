#include "game.hh"

#include "parse_move.hh"
#include "fen.hh"
#include "nag.hh"
#include "format.hh"
#include "gettext.hh"
#include "format_move_name.hh"

Game::Game(const Game &game)
	:  Pointed()
{
	*this= game;
}

void Game::Close()
{
	assert (Is());
	location_stack= 0;

	variation.Free();
	generator.Free();
}

void Game::New()
{
	/* Must free VARIATION_STACK before VARIATION because
	 * VARIATION_STACK contains pointers to objects owned by
	 * VARIATION. 
	 */
	variation_stack.Set(1);
	variation.Free();
	location_stack.Set(1);
	location_stack[0]= 0;
	variation_stack[0]= & variation; 
	position.Reset();
	generator.New(position);
	info.Reset(); 
	starting_position.Free();
}

bool Game::New(const Position &new_position) 
{
	if (starting_position.Is())
		starting_position()= new_position;
	else
	{
		if (! starting_position.Alloc(new_position))
			return false;
	}

	if (! starting_position->Legal())
		if (! starting_position->Set_Legal())
		{
			assert(0);
			starting_position.Free(); 
		}

	location_stack.Set(1);
	location_stack[0]= 0;
	variation_stack.Set(1);
	variation_stack[0]= & variation; 

	variation.Free();
	
	if (generator.Is())  generator.Free(); 
	position= starting_position();

	generator.New(position);

	info.Reset(); 

	return true;
}

bool Game::New(String fen)
{
	if (!fen)  
	{
		New();
		return true;
	}
	Position position;
	if (! Parse_FEN(fen, position))
		return false;
  
	if (! New(position))
		return false;

	return true;
}

bool Game::Append_Move(int move_index)
{
	assert(Is());
	assert(move_index >= 0 &&
	       move_index < *generator);

	assert(location_stack[-1] == *variation_stack[-1]());
	
	if (! variation_stack[-1]().Append_Move(generator, move_index))
		return false;

	position << generator[move_index];
	if (! generator.New(position))
	{
		position >> generator[move_index];
		return false; 
	}
	++ location_stack[-1]; 

	return true;
}

bool Game::Go_To_Start()
{
	assert (Is());

	if (*location_stack == 1 && location_stack[0] == 0)
		return false;

	location_stack= 1;
	variation_stack= 1;
	location_stack[0]= 0; 
	Reset_Position();
	generator.New(position);

	return true;
}

void Game::Go_Backward_In_Current()
{
	assert(Is());

	/* Take back last move from current variation.  */
	position >> variation_stack[-1]()[location_stack[-1] - 1];
	if (! generator.New(position))
	{
		position << variation_stack[-1]()[location_stack[-1] - 1];
		return;
	}
	-- location_stack[-1];
}

bool Game::Go_Forward()
{
	assert (Is());


	int size= location_stack[-1];
	int var_size= *variation_stack[-1]();

	if (size == var_size)
		return false;

	position << variation_stack[-1]()[location_stack[-1]++];
	generator.New(position);

	return true;
}

bool Game::Go_To_End()
{
	assert(Is());

	if (location_stack[-1] == *variation_stack[-1]())
		return false;

	do
		position << variation_stack[-1]()[location_stack[-1]++];
	while (location_stack[-1] < *variation_stack[-1]());

	generator.New(position);

	return true;
}

void Game::Remove_End()
{
	assert (Is());

	variation_stack[-1]().Cut(location_stack[-1]);
}

Game::~Game()
{
}

void Game::Append_Variation(int move_index)
{
	Append_Variation();
	Append_Move(move_index); 
}

bool Game::Append_Variation() 
{
	/* The position does not change ; GENERATOR is not changed. 
	 */

	if (! variation_stack[-1]().Append_Variation(location_stack[-1]))
		return false;
	if (! location_stack.Set(*location_stack + 1))
		return false;
	if (! variation_stack.Set(*location_stack))
	{
		location_stack= -1; 
		return false;
	}

	location_stack[-1]= 0;
	int index= location_stack[-2];
	variation_stack[-1]= 
		variation_stack[-2]()
		.Get_Variations(index)[-1]();

	return true; 
}

void Game::Go_Up()
{
	assert (*location_stack != 1);
	assert (location_stack[-1] == 0);

	location_stack= -1;
	variation_stack= -1; 
}

void Game::operator = (const Game &game)
{
	if (Is())
		Close();
	variation= game.variation;
	location_stack= game.location_stack;
	variation_stack.Set(*game.variation_stack);
	position= game.position;
	starting_position= game.starting_position; 
	if (*location_stack)
	{
		generator.New(position);
		variation_stack[0]= variation;
		for (int i= 1;  i < *location_stack;  ++i)
		{
			int n= -1;
			for (int j= 0;  
			     (j < *game.variation_stack[i - 1]()
			      .Get_Variations(location_stack[i - 1]) &&
			      n < 0);
			     ++j)
				if (game.variation_stack[i] == & game.variation_stack[i - 1]()
				    .Get_Variations(location_stack[i -1])[j]())
					n= j;
			variation_stack[i]= variation_stack[i - 1]()
				.Get_Variations(location_stack[i - 1])[n]();
		}
	}
	info= game.info;
}

void Game::
Reset_Position()
{
	if (starting_position.Is())
		position= starting_position();
	else
		position.Reset();
}

void Game::Enter_Variation(int index)
{
	assert (index >= 0);
	assert (Current_Variation().Have_Variations(location_stack[-1]));
	assert (index < *Current_Variation().Get_Variations(location_stack[-1]));

	if (! variation_stack.Set(*variation_stack + 1))
		return;

	if (! location_stack.Set(*location_stack + 1))
	{
		variation_stack= -1;
		return;
	}

	location_stack[-1]= 0;
	variation_stack[-1]= variation_stack[-2]().Get_Variations(location_stack[-2])[index]();
}

Move_Name Game::Overall_Move_Name(int overall_location) const
{
	int v, i;
	Get_Variation_Location(overall_location, v, i);
	return variation_stack[v]().Get_Move_Name(i);
}

bool Game::Go_To_Overall_Location(int overall_location)
{
	if (overall_location < Overall_Location())
	{
		bool left_variation= false;
		do
		{
			if (location_stack[-1] == 0)
			{
				left_variation= true;
				Go_Up();
			}
			Go_Backward_In_Current(); 
		}
		while (overall_location < Overall_Location());
		if (location_stack[-1] == 0 && overall_location != 0)
		{
			left_variation= true;
			Go_Up();
		}
		return left_variation; 
	}
	else if (overall_location > Overall_Location())
	{
		int previous= Current_Variation().Get_Previous_Move_Count();
		int current_location= overall_location - previous;
		assert (current_location >= 0 && 
			current_location <= Current_Variation().Length());
		while (location_stack[-1] < current_location)
			position << variation_stack[-1]()[location_stack[-1]++]; 
		generator.New(position);
		return false;
	}
	return false;
}

void Game::Remove_Variation()
{
	if (*location_stack == 1)
		return;

	while (location_stack[-1])
		Go_Backward_In_Current();
	int vindex= -1;
	int count= *variation_stack[-2]().Get_Variations(location_stack[-2]); 
	for (int i= 0;  
	     i < count; 
	     ++i)
	{
		if ((& variation_stack[-2]().Get_Variations(location_stack[-2])[i]()) ==
		    (& variation_stack[-1]()))
		{
			vindex= i;
			i= count;
		}
	}
	Go_Up();
	variation_stack[-1]().Remove_Variation(location_stack[-1],
					       vindex); 
	generator.New(position); 
}

void Game::Remove_Variation(int index)
{	
	variation_stack[-1]().Remove_Variation
		(location_stack[-1], index); 
}

const Move &Game::Overall_Move(int location) const
{
	int v, i;
	Get_Variation_Location(location, v, i);
	return variation_stack[v]()[i];
}

void Game::Get_Variation_Location(int overall_location, 
				  int &variation, 
				  int &variation_location) const
{
	int overall_previous= 0;
	for (int i= 0;  i < *location_stack;  ++i)
	{
		if (overall_location < overall_previous + location_stack[i])
		{
			variation= i;
			variation_location= overall_location - overall_previous;
			return; 
		}
		overall_previous += location_stack[i];
	}

	/* Requested location is after the location in current
	 * variation. 
	 */

	variation= *variation_stack - 1;
	variation_location= overall_location - overall_previous + location_stack[-1]; 
}

bool Game::Is_Overall_Variation_Move(int overall_location) const
{
	if (*location_stack == 1)
		return false;
	for (int j= 1;  j < *variation_stack;  ++j)
	{
		if (overall_location == variation_stack[j]()
		    .Get_Previous_Move_Count())
			return true;
	}
	return false;
}

void Game::Commentify(String &comment)
{
	for (unsigned i= 0;  i < *comment;  ++i)
		if (comment[i] == '}')
			comment(i)= ' '; 
}

void Game::Set_NAG(int nag)
{
	assert (location_stack[-1] != 0);
	variation_stack[-1]().Set_NAG(location_stack[-1] - 1, nag); 
}

bool Game::Leave_Variation()
{
	if (*variation_stack == 1)
		return false;

	while (location_stack[-1] != 0)
		Go_Backward_In_Current();

	Go_Up();

	return true; 
}

bool Game::Go_Backward()
{
	if (*location_stack == 1 && location_stack[0] == 0)
		return false;

	Go_Backward_In_Current();

	if (location_stack[-1] == 0)
	{
		if (*location_stack != 1)
			Go_Up();
	}

	assert (*location_stack == 1 || location_stack[-1] != 0);

	return true;
}

bool Game::Make_Main()
{
	while (*location_stack != 1)
		Make_Parent();
	return true; 
}

bool Game::Make_Parent()
{
	if (*location_stack == 1)
		return false;

	/* Index of current subvariation.
	 */
	int loc= location_stack[-2];
	/* location of this variation in parent */ 
	assert (variation_stack[-2]().Have_Variations(loc));
	int vcount= *variation_stack[-2]().Get_Variations(loc);
	assert (vcount > 0); 
	int vindex= -1;
	for (int i= 0;  i < vcount;  ++i)
	{
		if ((& variation_stack[-2]().Get_Variations(loc)[i]()) ==
		    (& variation_stack[-1]()))
		{
			vindex= i;
			i= vcount;
		}
	}

	/* vindex is now index of the variation to reparent */ 
	assert (vindex >= 0);
	variation_stack= -1; /* pop the variation */ 

	/* Detach old main variation.  The current subvariation is
	 * still contained in it.  Existing variation is truncated. 
	 */

	Dynamic <Variation> old_main;
	if (! old_main.Attach(Data_Alloc(Variation, 
					 variation_stack[-1](), 
					 location_stack[-2])))
		return false;

	old_main->Check_Variations(); 

	/* Transfer old subvariation from OLD_MAIN to game. 
	 */
	variation_stack[-1]().Append_Moves(old_main().Get_Variations(0)[vindex]());
	old_main->Check_Variations();
	old_main().Remove_Variation(0, vindex); 
	old_main->Check_Variations(); 

	/* Append subvariations of OLD_MAIN to parent. 
	 */
	if (old_main().Have_Variations(0))
	{
		for (int i= 0;  
		     i < *old_main().Get_Variations(0); 
		     i++)
		{
			variation_stack[-1]().Append_Variation
				(location_stack[-2],
				 old_main().Get_Variations(0)[i]);
		}
		old_main->Get_Variations(0)= 0; 
	}

	old_main->Check_Variations(); 
	
	/* Append OLD_MAIN as subvariation to current variation. */ 
	variation_stack[-1]().Append_Variation
		(location_stack[-2],
		 old_main);
	assert (! old_main.Is());

	location_stack[-2] += location_stack[-1];
	location_stack= -1;
	
	return true; 
}

void Game::Normalize()
{
	variation.Normalize();
}

void Game::Play_Variation(int i)
{
	Enter_Variation(i);
	bool f= Go_Forward();
	assert (f);
}

String Game::
Overall_Move_Text(int i, 
		  const Notation &notation,
		  const Notation_Settings &notation_settings)
	const
{
	String result= /*notation.*/Format_Move_Name
		(Overall_Move_Name(i), notation, notation_settings);
	result += NAG(Overall_NAG(i));
	if (Is_Overall_Variation_Move(i))  result += '*';
	return result; 
}

String Game::Format_Variation_Move(int i, const Notation &notation,
const Notation_Settings &notation_settings) const
{
	if (! variation_stack[-1]().Have_Variations(location_stack[-1]))
	{
		assert(0);
		return "Move";
	}
	String result= /*notation.*/Format_Move_Name
		(Current_Variation().Get_Variations(Current_Location())()[i]()
		 .Get_Move_Name(0),
		 notation,
		 notation_settings);
	result +=
		NAG(Current_Variation().Get_Variations(Current_Location())()[i]()
		    .Get_NAG(0));
	result += '*'; 
	return result; 
}

const Move &Game::Variation_Move(int i) const
{
	if (! variation_stack[-1]().Have_Variations(location_stack[-1]))
	{
		assert(0);
		return *(Move*)NULL; 
	}
	return Current_Variation().Get_Variations(Current_Location())()[i]()[0];
}

String Game::Format_Last_Move(const Notation &notation, const
Notation_Settings &notation_settings) const
{
	String text;
	if (Overall_Location())
	{
		text= Format("%d.%s ",
			     (Overall_Location() - 1) / 2 + info.first_move_number,
			     position.To_Move() ? "" : "..");
		String move= Overall_Move_Text(Overall_Location() - 1,
		notation, notation_settings);
		text += move;
	}
	else
	{
		text= _("Starting position");
	}

	if (*location_stack == 1  
	    && location_stack[0] == *variation
	    && info.result != re_unfinished)
		/* If located at the end of the main variation. */
	{
		text += " ";
		text += info.Get_Result_Readable_Text();
	}

	return text; 
}

void Game::Set_Starting_Previous_Move_Count(int new_starting_previous_move_count)
{
	assert (new_starting_previous_move_count >= 0);
	assert ((new_starting_previous_move_count & 1) == First_Move());
	if (! starting_position.Is())
	{
		if (new_starting_previous_move_count)
		{
			starting_position.Alloc(0);
			starting_position().previous_move_count= new_starting_previous_move_count; 
			position.previous_move_count += new_starting_previous_move_count;
		}
	}
	else
	{
		position.previous_move_count += new_starting_previous_move_count;
		position.previous_move_count -= starting_position().previous_move_count; 
		starting_position().previous_move_count= new_starting_previous_move_count; 
	}
}

const char *Game::To_Move_Description() const
{
	const char *text;
	int to_move= Current_Position().To_Move(); 
	if (*Current_Generator() == 0)
	{
		if (Current_Position().In_Check())
		{
			if (!to_move)  text= N_("White is checkmate");
			else           text= N_("Black is checkmate");
		}
		else
		{
			if (!to_move)  text= N_("White is stalemate");
			else           text= N_("Black is stalemate");
		}
	}
	else
	{
		if (Current_Position().In_Check())
		{
			if (!to_move)  text= N_("White is in check");
			else           text= N_("Black is in check");
		}
		else
		{
			if (!to_move)  text= N_("White to move");
			else           text= N_("Black to move");
		}
	}

	return _(text); 
}

int Game::Overall_Location() const
{
	return variation_stack[-1]().Get_Previous_Move_Count() 
		+ location_stack[-1] 
		- variation.Get_Previous_Move_Count(); 
}
