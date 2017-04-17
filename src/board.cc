#include "board.hh"

/* This is the only widget processing mouse events. */
/* And focus events */ 

#include "main.hh"
#include "board_image.hh"
#include "format.hh"
#include "verbose.hh"
#include "image.hh"
#include "image_stream.hh"
//#include "board_colors_reset.hh"
#include "squares_touched.hh"
#include "format_move_name.hh"

#include <gdk/gdkkeysyms.h>
#include <gtk/gtkstock.h>

int Board::frame_width= -1;

void Board::B_Colors::Make(const Board_Colors &settings)
{
	have_colors= true;

	for (int i= 0;  i < c_count;  ++i)
	{
		Canvas::Alloc_Color(colors[i][0], settings.colors[i]);
	}

	/* colors that are used for circles */
	enum {count= 3};
	const int indices[count]= {c_last_move, c_selection, c_selection_no};
	for (int i= 0;  i < count;  ++i)
	{
		int index= indices[i];
		RGB color= settings.colors[index];
		color.r -= 50;  color.r= std::max(color.r, 0);
		color.g -= 50;  color.g= std::max(color.g, 0);
		color.b -= 50;  color.b= std::max(color.b, 0);
		Canvas::Alloc_Color(colors[index][1], color);
	}
}

void Board::Draw_Circle(Canvas &canvas, 
			const B_Colors &colors,
			GdkRectangle rect, 
			int circle_color)
{
	/* big circle */ 
	canvas.Set_Function(GDK_COPY);
	
	gdk_gc_set_foreground(canvas.Get_GC(), (GdkColor*)&colors(circle_color, 1)); 
	canvas.Draw_Ellipse(rect);

	/* small circle */ 
	enum {part=23};
	GdkRectangle inner= 
		{rect.x + rect.width  / part,
		 rect.y + rect.height / part,
		 rect.width   - rect.width / part * 2,
		 rect.height  - rect.height / part * 2};
	gdk_gc_set_foreground(canvas.Get_GC(), (GdkColor*)&colors(circle_color, 0));
	canvas.Draw_Ellipse(inner);
}

void Board::Draw_Square_Background(Canvas &canvas,
				   const B_Colors &colors, 
				   GdkRectangle rect,
				   int circle_color,
				   int square_color,
				   int settings_flags,
				   int colors_flags,
				   int square,
				   int focus)
{
	GdkColor bg_color= colors(square_color, 0); 
	canvas.Set_Function(GDK_COPY);

	/* The "black" background color is grey by default.  When it
	 * would have been black, draw diagonal stripes, as black
	 * piece on a black background are not visible. 
	 */
	bool draw_riffles= false;

	GdkColor black;
	gdk_color_black(gdk_colormap_get_system(), &black);
	GdkColor white;
	gdk_color_white(gdk_colormap_get_system(), &white);

	if (square_color == cc_b && 
	    (colors_flags & bt_riffled || bg_color.pixel == black.pixel))
	{
		draw_riffles= true;
		gdk_gc_set_foreground(canvas.Get_GC(), &white);
		canvas.Draw_Rect(rect);

	}
	else  /* filled background */
	{
		gdk_gc_set_foreground(canvas.Get_GC(), &bg_color); 
		canvas.Draw_Rect(rect);
	}

	/* circle */

	if (circle_color)
	{
		Draw_Circle(canvas, colors, rect, circle_color); 
	}

	/* focus */
	if (focus)
	{
		gdk_gc_set_foreground(canvas.Get_GC(), &white);
		gdk_gc_set_background(canvas.Get_GC(), &black);

		int square_width= rect.width;
		
		int line_width= std::max(square_width/30, 1);

		canvas.Set_Line_Mode(line_width,
				     GDK_JOIN_MITER,
				     GDK_CAP_NOT_LAST,
				     GDK_LINE_DOUBLE_DASH);
		canvas.Set_Function(GDK_XOR);

		int space= square_width/15;
		GdkPoint points[4]= 
			{{rect.x+space,             rect.y+space       },
			 {rect.x+rect.width-space,  rect.y+space       },
			 {rect.x+rect.width-space,  rect.y+rect.height-space},
			 {rect.x+space,             rect.y+rect.height-space}};

		gdk_draw_polygon(canvas.Get_Drawable(), canvas.Get_GC(),
				 FALSE, points, 4);
	}

	/* riffles */
	if (draw_riffles)
	{
		enum {line_count= 8};

		gdk_gc_set_foreground(canvas.Get_GC(), &bg_color);
		int line_width= std::max(((rect.width + rect.height) / 9 / line_count), 1);
		canvas.Set_Line_Mode(line_width);

		for (int i= 0; i < line_count; ++i)
		{
			gdk_draw_line(canvas.Get_Drawable(), canvas.Get_GC(),
				      rect.x,
				      rect.y+i*rect.height/line_count,
				      rect.x+i*rect.width/line_count,
				      rect.y);

			gdk_draw_line(canvas.Get_Drawable(), canvas.Get_GC(),
				      rect.x+i*rect.width/line_count,
				      rect.y+rect.height,
				      rect.x+rect.width,
				      rect.y+i*rect.height/line_count);
		}

		bg_color= white;
		/* Use white background when drawing the square name,
		 * even on dark squares. 
		 */
	}

	/* square name */
	if (settings_flags & b_tutor)
	{
		gdk_gc_set_foreground(canvas.Get_GC(), (GdkColor*)&colors(c_text, 0));
		gdk_gc_set_background(canvas.Get_GC(), &white); 
		canvas.Set_Function(GDK_COPY);

		char text[2]= {'a'+File(square), '1'+Rank(square)};

		canvas.Draw_Label((GdkPoint){rect.x,
					rect.y + rect.height - Canvas::Get_Descent()},
				  text, 2);
	}
}

void Board::Draw_Exact_Square(Canvas &canvas,
			      Scaled_Board_Image &scaled,
			      const B_Colors &colors,
			      GdkPoint point,
			      int piece,
			      int square_color,
			      int circle_color,
			      int settings_flags,
			      int colors_flags,
			      int square,
			      int focus)
{
	Draw_Square_Background
		(canvas, colors, 
		 (GdkRectangle) { point.x, point.y, scaled.Width(), scaled.Width()},
		 circle_color,
		 square_color,
		 settings_flags,
		 colors_flags,
		 square,
		 focus);

	if (piece == pi_empty)  return;

	if (! scaled.Get_Image().Has_Canvas())  return;

	canvas.Set_Function(GDK_COPY);

	scaled.Draw(canvas,
		    point, 
		    piece & pi_type,
		    Piece_Color(piece));
}

bool Board::Open(Pointer <Widget> parent, 
		 const Open_Info &open_info)
{
	dragging= false;

	msg_move= open_info.msg_move;
	msg_middle= open_info.msg_middle;
	msg_turn= open_info.msg_turn;
	msg_wheel_up= open_info.msg_wheel_up;
	msg_wheel_down= open_info.msg_wheel_down;

	colors.Open(*this, "board_colors_edited");
	Settings_Simple_Load_Or_Default(settings, "board_settings");
	turned= open_info.turned;
	statusbar= NULL;

	position= open_info.position;
	generator= open_info.generator; 
	opening_book= open_info.opening_book;
	square_width= -1;
	last_a= last_b= 0; 
	last_highlight[0]= last_highlight[1]= 0;
	selection= has_focus= 0; 
	focus= sq_e4;
	want_clear_selection= false;

	if (! Drawing_Area::Open(parent))
		return false;

	menubar= open_info.menubar;

	Connect_Key_Signals(Get());

	GTK_WIDGET_SET_FLAGS (Get(), GTK_CAN_FOCUS);

	gtk_widget_add_events(Get(),
			      GDK_POINTER_MOTION_MASK      |
			      GDK_POINTER_MOTION_HINT_MASK |
			      GDK_BUTTON_MOTION_MASK       |
			      GDK_BUTTON_PRESS_MASK        |
			      GDK_SCROLL                   |
			      GDK_BUTTON_RELEASE_MASK      |
			      GDK_FOCUS_CHANGE_MASK);

	gtk_signal_connect(GTK_OBJECT(Get()), "motion-notify-event",
			   GTK_SIGNAL_FUNC(Signal_Motion_Notify), this); 
	gtk_signal_connect(GTK_OBJECT(Get()), "button-press-event",
			   GTK_SIGNAL_FUNC(Signal_Button_Press), this); 
	gtk_signal_connect(GTK_OBJECT(Get()), "button-release-event",
			   GTK_SIGNAL_FUNC(Signal_Button_Release), this); 
	gtk_signal_connect(GTK_OBJECT(Get()), "focus-in-event",
			   GTK_SIGNAL_FUNC(Signal_Focus), this);
	gtk_signal_connect(GTK_OBJECT(Get()), "focus-out-event",
			   GTK_SIGNAL_FUNC(Signal_Focus), this);
	gtk_signal_connect(GTK_OBJECT(Get()), "scroll_event",
			   GTK_SIGNAL_FUNC(Signal_Scroll), this);
	

	if (frame_width < 0)
		frame_width= Canvas::Get_Ascent() + std::max(dialog_unit / 6, 2); 

	Update_Board_Image_Size();
	Set_Menu();

	return true;
}

void Board::Wdg_Draw(Canvas &canvas)
{
	if (! (board_image.Is() && board_image.Has_Canvas()))   return;
	board_colors.Make(colors()); 

	/* draw invalid squares */ 
	for (int rank= 7;  rank >= 0;  --rank)
	{
		int bits= invalid.Get_Rank(rank);
		assert (bits >= 0 && bits < 0x100);
		int file= 0;
		while (bits)
		{
			if (bits & 1)  Draw_Actual_Square(Make_Square(file, rank));
			bits >>= 1;
			++file;
		}
	}
	invalid.Empty(); 

	/* copy canvas */ 
	board_image().Set_Function(GDK_COPY);
	canvas.Draw_Canvas(board_image(),  (GdkRectangle){0, 0, Size().x,
								  Size().y},
			   (GdkPoint){0, 0});
}

void Board::Update_Board_Image_Size()
{
	Interrupt_Dragging();
	
	int w= (!(settings.flags & b_hide_frame))
		? 2 * frame_width : 0;
	int new_square_width= std::max((std::min(Size().x, Size().y) - w) / 8, 1); 
	
	if (new_square_width != square_width)
	{
		Verbose("new_square_width= %d", new_square_width);
		square_width= new_square_width;
		scaled_image.Load(square_width);

		board_image.Free();
		board_image.Create(Size());

		if (board_image.Is())
		{
			Draw_Frame();
			Invalidate_Position(); 
		}
		
		drag_img.Free();
		drag_img.Create(Size()); 
	}

	if (! board_image.Has_Canvas())  return; 


	GdkColor color;
	Canvas::Alloc_Color(color, RGB(0xEA, 0xEA, 0xEA));
	gdk_gc_set_foreground(board_image->Get_GC(), 
			      &color);
	board_image().Set_Function(GDK_COPY); 
		
	w += 8 * square_width;
	board_image().Draw_Rect((GdkRectangle){w, 0, 
				     Size().x - w, w});
	board_image().Draw_Rect((GdkRectangle){0, w, 
						       Size().x, Size().y - w});
}

void Board::Draw_Square(Canvas &canvas,
			int square,
			int circle_color,
			bool empty)
{
	if (! scaled_image.Is())  return; 

	assert (Is_Inside_Square(square));

	GdkRectangle rect= Square_Rect(square);
	int piece= position()[square];
	if (empty)  piece= pi_empty; 
	Draw_Exact_Square(canvas, scaled_image, board_colors,
			  (GdkPoint){rect.x,rect.y}, piece, Square_Color(square), circle_color,
			  settings.flags,
			  colors().flags, 
			  square,
			  has_focus && (focus == square));
}

void Board::Wdg_Size(GdkPoint)
{
	Update_Board_Image_Size();
}

int Board::Point_Square(GdkPoint p) const
{
	if (! (/*preferences().flags*/settings.flags & b_hide_frame))
	{
		p.x -= frame_width;
		p.y -= frame_width; 
	}

	int square= Make_Square(sq_a + p.x / square_width,
				sq_8 - p.y / square_width);

	if (turned)
	//	if (/*preferences().flags*/settings.flags & turned)
		Square_Turn(square);

	return square;
}

bool Board::Get_Squares(int &from, int &to, guint *new_state)
{
	from= drag_from;
	to=   drag_to;
	if (new_state) *new_state= state; 
	return true;
}

void Board::Invalidate_Position()
{
	selection= has_focus= focus= 0;
	invalid.Set();
	Invalidate();
}

void Board::MR_Receive(String message)
{
	if (message == "board_colors")
	{
		colors.Edit(); 
	}

	else if (message == "board_turn_board")
	{
		Set_Turned(! turned);
		Set_Menu_Turned();
	}
	else if (message == "board_hide_frame")
	{
		Set_Hide_Frame(! (settings.flags & b_hide_frame));
		Set_Menu_Hide_Frame();
		Save_Settings();
	}
	else if (message == "board_tutor")
	{
		Interrupt_Dragging();
		settings.flags ^= b_tutor;
		Invalidate_Position();
		Set_Menu_Tutor();
		Save_Settings();
	}
	else if (message == "board_no_highlight_last")
	{
		settings.flags ^= b_no_highlight_last;
		Set_Menu_No_Highlight_Last();
		Update_Last_Highlight(); 
		Save_Settings();
	}

	else if (message == "board_colors_edited")
	{
		Interrupt_Dragging();
		square_width= -1;
		board_colors.Invalidate(); 
		Update_Board_Image_Size();
		Invalidate(); 
	}

	else 
		Parent_Receive_Message(message);  
}

void Board::Interrupt_Dragging()
{
	if (! dragging)  return;

	dragging= false;
	if (statusbar)
		gtk_statusbar_pop(statusbar, 0);


	/* redraw current position of dragged piece */ 
	GdkRectangle rect= {last_org.x, last_org.y, square_width, square_width};
	if (rect.x < 0)
	{
		rect.width += rect.x;
		rect.x= 0;
	}
	if (rect.y < 0)
	{
		rect.height += rect.y;
		rect.y= 0;
	}

	GdkPoint osize= board_image.Get_Size();
	if (rect.x + rect.width > osize.x)
		rect.width= osize.x - rect.x;
	if (rect.y + rect.height > osize.y)
		rect.height= osize.y - rect.y;
	board_image().Draw_Canvas(drag_img(), rect, (GdkPoint){rect.x,rect.y});

	/* Invalidate square around DRAG_TO that were obscured by piece
	 * */ 
	Invalidate(rect);

	/* Redraw from square */
	Invalidate_Square(drag_from, false, false); 

	/* Unselect tutor squares */
	if (tutor_high.Is())
	//	if (*tutor_highlight)
	{
		int file, rank;
		for (Bit_Board_Iterator i(tutor_high);
		     i.Next(file, rank);)
//		for (int i= 0;  i < *tutor_highlight;  ++i)
		{
			int square= Make_Square(file, rank);//tutor_highlight[i];

			invalid.Set(square);
			GdkRectangle rect= Square_Rect(square);
			Invalidate(rect);
		}
//		tutor_highlight= 0;
		tutor_high.Empty();
	}
}

GdkRectangle Board::Square_Rect(int square) const
{
	int file= File(square);
	int rank= Rank(square); 
	if (turned)
	{
		file= sq_h - (file - sq_a);
		rank= sq_8 - (rank - sq_1);
	}

	int w= settings.flags & b_hide_frame ? 0 : frame_width;
	return (GdkRectangle)
		{square_width * (file - sq_a) + w,
			 square_width * (sq_8 - rank) + w,
			 square_width, square_width};
}

void Board::Wdg_Close()
{
	colors.Close(); 
	position= NULL;
	generator= NULL;
	opening_book= NULL;
	notation= NULL;
	notation_settings= NULL;

	Drawing_Area::Wdg_Close();
}

void Board::Set_Hide_Frame(bool hide)
{
	Interrupt_Dragging();
	if (hide == ((settings.flags & b_hide_frame) != 0))
		return;
	settings.flags &=~ b_hide_frame;
	settings.flags |= hide*b_hide_frame;
	square_width= -1;
	Update_Board_Image_Size();
	Invalidate(); 
}

void Board::Set_Last_Highlight(int a, int b)
{
	Interrupt_Dragging();

	Clear_Last_Highlight(); 

	{
		last_highlight[0]= a;
		invalid.Set(a);
		GdkRectangle rect= Square_Rect(a);
		Invalidate(rect);
	}

	{
		last_highlight[1]= b;
		invalid.Set(b);
		GdkRectangle rect= Square_Rect(b);
		Invalidate(rect);
	}
}

void Board::Clear_Last_Highlight()
{
	if (last_highlight[0] != 0)
	{
		int square= last_highlight[0];
		last_highlight[0]= 0;
		invalid.Set(square);
		GdkRectangle rect= Square_Rect(square);
		Invalidate(rect);
	}
	if (last_highlight[1] != 0)
	{
		int square= last_highlight[1];
		last_highlight[1]= 0;	
		invalid.Set(square);
		GdkRectangle rect= Square_Rect(square); 
		Invalidate(rect);
	}
}

void Board::Wdg_Get_Size_Request(int &,  int &,  bool &resizeable)
{
	resizeable= true; 
}

void Board::Invalidate_Square(int square, 
			      bool interrupt_dragging,
			      bool clear_selection)
{
	assert (Is_Inside_Square(square));

	if (interrupt_dragging)  Interrupt_Dragging(); 

	invalid.Set(square);
  
	GdkRectangle rect= Square_Rect(square);
	Invalidate(rect);

	if (clear_selection)
	{
		Clear_Selection();
		Clear_Focus();
	}
}

void Board::Set_Turned(bool new_turned) 
{
	Interrupt_Dragging();
	turned= new_turned;
	square_width= -1;
	Update_Board_Image_Size();

	Parent_Receive_Message(msg_turn);
//	Parent_Send_Message(msg_turn);
}

void Board::Draw_Frame()
{
	if (settings.flags & b_hide_frame)       return;
	if (! board_image.Is())                  return;
	if (! board_image.Has_Canvas())          return;

	Canvas &canvas= board_image.Get_Canvas();

	board_colors.Make(colors()); 
	
	canvas.Set_Function(GDK_COPY);

	GdkColor color;
	Canvas::Alloc_Color(color, colors().colors[c_frame]);
	gdk_gc_set_foreground(canvas.Get_GC(), &color);
	canvas.Draw_Rect((GdkRectangle){0, 
			      0,
			      2 * frame_width + 8 * square_width,
			      frame_width});
	canvas.Draw_Rect((GdkRectangle){0,
			      frame_width + 8 * square_width,
			      2 * frame_width + 8 * square_width,
			      frame_width});
	canvas.Draw_Rect((GdkRectangle){0,
			      frame_width,
			      frame_width,
			      8 * square_width});
	canvas.Draw_Rect((GdkRectangle){frame_width + 8 * square_width,
			      frame_width,
			      frame_width,
			      8 * square_width});

	gdk_gc_set_foreground
		(canvas.Get_GC(), 
		 (GdkColor*)&board_colors(c_frame_text, 0));

	int ascent= Canvas::Get_Ascent();

	for (int file= 0;  file < 8;  ++file)
	{
		char t= turned ? 'H' - file : 'A' + file;
		int width= Canvas::Get_Label_Width(&t, 1);
		int x= frame_width + ((2 * file + 1) * square_width - width) / 2;
		int y= (frame_width - ascent) / 2 + ascent;
		canvas.Draw_Label((GdkPoint){x, y}, &t, 1);
		y += frame_width + 8 * square_width; 
		canvas.Draw_Label((GdkPoint){x, y}, &t, 1);
	}

	for (int rank= 0;  rank < 8;  ++rank)
	{
		char t= turned ? '1' + rank : '8' - rank;
		int width= Canvas::Get_Label_Width(&t, 1);
		int y= frame_width + rank * square_width + (square_width - ascent) / 2 + ascent; 
		int x= (frame_width - width) / 2;
		canvas.Draw_Label((GdkPoint){x, y}, &t, 1);
		x += frame_width + 8 * square_width;
		canvas.Draw_Label((GdkPoint){x, y}, &t, 1);
	}
}

void Board::Invalidate_Move_Squares(const Move &move)
{
	int changed[4];
	Squares_Touched(move, changed);
	Invalidate_Square(changed[0], false);
	Invalidate_Square(changed[1], false);
	if (changed[2])  Invalidate_Square(changed[2], false);
	if (changed[3])  Invalidate_Square(changed[3], false);
}

bool Board::Is_Inside_Point(GdkPoint p) const
{
	int w= settings.flags & b_hide_frame ? 0 : frame_width;
	return (p.x >= w && p.x < 8 * square_width + w  &&
		p.y >= w && p.y < 8 * square_width + w);
}

bool Board::Wdg_Key_Press(guint keyval)
{
	Verbose_Function();
	Verbose_Int(keyval);

	Interrupt_Dragging(); 

	if (keyval == GDK_Escape)
	{
		if (dragging)        Interrupt_Dragging(); 
		else if (selection)  Clear_Selection();
		else if (has_focus)  Clear_Focus(); 
	}
	else if (keyval == GDK_Up)
	{
		if (has_focus)  Set_Focus(turned ?
					  focus - 10 : focus + 10);
		else            Set_Focus(-1);
	}
	else if (keyval == GDK_Down)
	{
		if (has_focus)  Set_Focus(/*preferences->flags & bt_turned*/turned ?
					  focus + 10 : focus - 10);
		else            Set_Focus(-1);
	}
	else if (keyval == GDK_Right)
	{
		if (has_focus)  Set_Focus(/*preferences->flags & bt_turned*/turned ?
					  focus - 1 : focus + 1);
		else            Set_Focus(-1);
	}
	else if (keyval == GDK_Left)
	{
		if (has_focus)  Set_Focus(/*preferences->flags & bt_turned*/turned ?
					  focus + 1 : focus - 1);
		else            Set_Focus(-1);
	}
	else if (keyval == GDK_Return)
	{
		if (has_focus)   
		{
			if (selection)
			{
				drag_from= selection;
				Verbose("RETURN drag_from= selection = %d", drag_from);
				drag_to=   focus;
				Set_Selection(focus);
				Parent_Receive_Message/*Send*/(msg_move); 
			}
			else
				Set_Selection(focus);
		}
	}
	else
		return false;
	return true;
}

void Board::Set_Last_Move(int square_a, int square_b)
{
	last_a= square_a;
	last_b= square_b;
	if (! (settings.flags & b_no_highlight_last))
//	if (! (preferences().flags & bt_no_highlight_last))
		Set_Last_Highlight(square_a, square_b);
	else    
		Clear_Last_Highlight(); 
}

void Board::Update_Last_Highlight()
{
	if (! (/*preferences()*/settings.flags & b_no_highlight_last))
	{
		if (last_a && last_b)  Set_Last_Highlight(last_a, last_b);
	}
	else
		Clear_Last_Highlight(); 
}

void Board::Draw_Actual_Square(int square)
{
	if (! board_image.Has_Canvas())  return;

	Draw_Square(board_image(),
		    square,
		    (square == selection     
		     ? c_selection
		     : (last_highlight[0] == square || last_highlight[1] ==
			square)          ? c_last_move 
		     : 0));
}

bool Board::Open_Menu()
{
	if (menu.get())  return true; 

	/*  The logic below is to remove the first item of the list
	 *  and to remove "/Board" from the beginning of each string.   */
	static GtkItemFactoryEntry menu_items[] = 
		{BOARD_MENU};
	static bool have= false;
	enum {count= sizeof(menu_items) / sizeof(GtkItemFactoryEntry)};

	if (! have)
	{
		for (int i= 1;  i < count;  ++i)
		{
			menu_items[i].path += strlen("/Board");
		}
		have= true;
	}

	menu.reset(new Popup_Menu(count-1, menu_items+1, *this/*, &Translate*/));

	Set_Menu();

	return true;
}

void Board::Set_Menu()
{
	Set_Menu_Turned();
	Set_Menu_Hide_Frame();
	Set_Menu_Tutor();
	Set_Menu_No_Highlight_Last();
}

void Board::Set_Menu_Turned()
{
	menubar->Set_Active("/Board/Turn Board", turned);
	if (menu.get())
		menu->Set_Active("/Turn Board", turned);
}

void Board::Set_Menu_Hide_Frame()
{
	menubar->Set_Active("/Board/Hide the Frame", settings.flags & b_hide_frame);
	if (menu.get()) 
		menu->Set_Active("/Hide the Frame", settings.flags & b_hide_frame);
}

void Board::Set_Menu_Tutor()
{
	menubar->Set_Active("/Board/Tutor Mode", settings.flags & b_tutor);
	if (menu.get())
		menu->Set_Active("/Tutor Mode", settings.flags & b_tutor);
}

void Board::Set_Menu_No_Highlight_Last()
{
	menubar->Set_Active("/Board/Don’t Highlight the Last Move", 
			    settings.flags & b_no_highlight_last);
	if (menu.get())
		menu->Set_Active("/Don’t Highlight the Last Move", 
				 settings.flags & b_no_highlight_last);
}

const char *Board::Translate(const char *text)
{
	String full= "/Board";
	full += text;
	const char *translation= _(full());
	if (translation == full())  return text;
	const char *result= strchr(translation+1, '/');
	return result; 
}

gint Board::Signal_Motion_Notify(GtkWidget *,
				 GdkEventMotion *event,
				 gpointer data)
{
	GdkModifierType state;
	gint x, y;
	if (event->is_hint)
	{
		gdk_window_get_pointer (event->window, &x, &y, &state);
		event->x= x;
		event->y= y;
	}
	((Board *)data)->Motion(event);
	return TRUE;
}

gint Board::Signal_Button_Press(GtkWidget *,
				GdkEventButton *event,
				gpointer data)
{
	((Board *)data)->Button_Press(event);
	return TRUE;
}

gint Board::Signal_Button_Release(GtkWidget *,
				  GdkEventButton *event,
				  gpointer data)
{
	((Board *)data)->Button_Release(event);
	return TRUE;
}

void Board::Motion(GdkEventMotion *event)
{
	if (! dragging)  return;

	/* Adjust pointer_pos (drift) */
	enum {part= 80};
	if (pointer_pos.x < square_width/2)
	{
		pointer_pos.x += std::max((square_width /part), 1);
		pointer_pos.x= std::min(pointer_pos.x, square_width/2);
	}
	if (pointer_pos.y < square_width/2)
	{
		pointer_pos.y += std::max((square_width /part), 1);
		pointer_pos.y= std::min(pointer_pos.y, square_width/2);
	}
	if (pointer_pos.x > square_width/2)
	{
		pointer_pos.x -= std::max((square_width /part), 1);
		pointer_pos.x= std::max(pointer_pos.x, square_width/2);
	}
	if (pointer_pos.y > square_width/2)
	{
		pointer_pos.y -= std::max((square_width /part), 1);
		pointer_pos.y= std::max(pointer_pos.y, square_width/2);
	}
	
	GdkPoint p= {(int)event->x, (int)event->y};
	GdkPoint new_org= {p.x-pointer_pos.x, p.y - pointer_pos.y};

	GdkPoint center= {new_org.x + square_width / 2,
			  new_org.y + square_width / 2};
	if (Is_Inside_Point(center) && drag_from != Point_Square(center))
		want_clear_selection= false; 

	/* Temporary image */ 
	static Pixmap i;
	static int i_size= -1;

	if (i_size != square_width)
	{
		if (i.Is())
			i.Free();
		i.Create((GdkPoint){square_width, square_width});
		i_size= square_width;
	}

	if (! (i.Is() 
	       && i.Has_Canvas()
	       && drag_img.Has_Canvas()
	       && board_image.Has_Canvas()
	       && scaled_image.Get_Image().Has_Canvas()))
		return; 

	/* Draw background on board on old location */ 
	board_image->Set_Function(GDK_COPY);
	board_image->Draw_Canvas
		(drag_img(),
		 (GdkRectangle){last_org.x, last_org.y, square_width, square_width},
		 last_org);
	
	/* Draw image into temporary */
	i->Set_Function(GDK_COPY); 
	i->Draw_Canvas(board_image(),
			(GdkRectangle){0, 0, square_width,square_width},
			new_org);

	/* Draw new piece on temporary */ 
	scaled_image.Draw(i(),
			  (GdkPoint){0, 0},
			  dragged_piece & pi_type,
			  Piece_Color(dragged_piece));

	/* Draw temporary on board */ 
	board_image->Draw_Canvas
		(i(),
		 (GdkRectangle){new_org.x, new_org.y, square_width, square_width},
		 (GdkPoint){0,0});
	Invalidate((GdkRectangle){last_org.x,last_org.y, square_width,
					  square_width});
	Invalidate((GdkRectangle){new_org.x,new_org.y,square_width,
					  square_width});

	/* Set new origin */
	last_org= new_org;

	/* Set statusbar text */
	if (statusbar)
	{
		gtk_statusbar_pop(statusbar, 0);
		String text= Format
			("%c%c–",
			 'a'+File(drag_from),
			 '1'+Rank(drag_from));
		if (Is_Inside_Point(center)&&Point_Square(center)!=drag_from)
			text += Format("%c%c",
				       'a'+File(Point_Square(center)),
				       '1'+Rank(Point_Square(center)));
		if (generator && Is_Inside_Point(center))
		{
			int index= generator->Square_Move(drag_from, Point_Square(center));
			if (index >= 0)
			{
				text += Format
					(" %d.%s ",
					 generator->Get_Position().previous_move_count/2+1,
					 generator->Get_Position().previous_move_count%2
					 ?"..":"");
					       
				text += Format_Move_Name
					(generator->Get_Move_Name(index),
					 notation(),
					 notation_settings());
			}
		}
		if (generator)
			text += Piece_Info(Is_Inside_Point(center)
					   ? Point_Square(center)
					   : -1);
		gtk_statusbar_push 
			(statusbar, 0,
			 text());
	}
	
}

void Board::Button_Press(GdkEventButton *event)
{
	GdkPoint p= {(int)event->x, (int)event->y};

	switch (event->button)
	{
	case 1:  /* left */ 
		if (dragging)
			End_Dragging(p, event->state);
		else 
			Begin_Dragging(p);
		break;
	case 2:  /* middle */ 
		if (! Is_Inside_Point(p))  return;
		Interrupt_Dragging();
		drag_from= Point_Square(p);
		Verbose("MIDDLE drag_from= %d", drag_from);
		Parent_Receive_Message(msg_middle);
		break;
	case 3:  /* right */ 
		Interrupt_Dragging();
		if (Open_Menu())
			menu->Popup();
		break;
	}
}

void Board::Button_Release(GdkEventButton *event)
{
	if (event->button == 1)
	{
		GdkPoint p= {(int)event->x, (int)event->y};
		if (want_clear_selection)
		{
			Verbose("RELEASE && WANT_CLEAR_SELECTION, drag_from= %d",
				drag_from);
			Invalidate_Square(drag_from, true, true);
			want_clear_selection= false; 
		}
		else if (dragging)
			End_Dragging(p, event->state);
	}
}

void Board::Begin_Dragging(GdkPoint p)
{
	if (! (drag_img.Is() && board_image.Is()))                   return;
	if (! (drag_img.Has_Canvas() && board_image.Has_Canvas()))   return;
	if (! Is_Inside_Point(p))                                    return;


	drag_from= Point_Square(p);
	Verbose("dragging inside, drag_from = %d", drag_from);
	want_clear_selection= false;

	/* click on destination square when selection is set */ 
	if (selection)
	{
		if (selection == drag_from)
		{
			want_clear_selection= true;
			/* and maybe begin dragging... */ 
			Verbose("  want_clear_selection= true;");
		}
		else
		{
			/* play a move by clicking on the destination square.
			 * If the square is empty, always interpret as a move
			 */  
			if (position()[drag_from] == pi_empty)
			{
				drag_to= drag_from;
				drag_from= selection;
				Verbose("CLICK ON DESTINATION drag_from= %d", drag_from);
				Parent_Receive_Message/*Send*/(msg_move); 
				return; 
			}

			/* If there is a piece, only interpret as a move when
			 * a corresponding move is possible */ 
			if (generator &&
			    Is_Piece_Of_Color(position()[selection], position->To_Move()) &&
			    Is_Piece_Of_NColor(position()[drag_from], position->To_Move()))
			{
				for (int i= 0;  i < *generator(); ++i)
				{
					if (generator()[i].from == selection &&
					    generator()[i].to   == drag_from)
					{
						drag_to= drag_from;
						drag_from= selection;
						Verbose("CLICK CAPTURE drag_from= %d",
							drag_from);
						Parent_Receive_Message/*Send*/(msg_move); 
						return; 
					}
				}
			}
		}
	}

	/* Click on empty square */ 
	dragged_piece= position()[drag_from];
	if (dragged_piece == pi_empty)  
	{
		drag_to= drag_from;
		Parent_Receive_Message/*Send*/(msg_move);
		return;
	}

	/* Remove old focus */ 
	if (has_focus && focus != drag_from)
	{
		int square= focus;
		has_focus= 0;
		Draw_Actual_Square(square);
		Invalidate(Square_Rect(square));
	}

	/* Remove old selection */ 
	if (selection && selection != drag_from)
	{
		int square= selection;
		selection= 0;
		Draw_Actual_Square(square);
		Invalidate(Square_Rect(square));
	}
	selection= focus= drag_from; 
	has_focus= true;

	gtk_widget_grab_focus(Get());

	/* Draw from square */ 
	Draw_Square(board_image(), 
		    drag_from,
		    c_selection);
	GdkRectangle rect= Square_Rect(drag_from);
	Invalidate(rect); 

	/* Draw tutor squares */
	tutor_high.Empty();
//	tutor_highlight= 0;
	tutor_no_square= 0;
	if (generator.Is() && (settings.flags & b_tutor) && generator().Is())
	{
		int count= 0;
		for (int i= 0;  i < *generator();  ++i)
		{
			if ((generator()[i].from == drag_from ||
			     generator()[i].to   == drag_from) 
			    /* && tutor_highlight.Set(*tutor_highlight+1) */)
			{
				++count;
				int square= generator()[i].to;
				if (square == drag_from)
					square= generator()[i].from;
				assert (square != drag_from);
				assert (Is_Inside_Square(square));
				tutor_high.Set(square);
//				tutor_highlight[-1]= square;
				if (generator()[i].promotion & pi_color)
				{
					i += 3;
				}
				/* Draw square */
				Draw_Square(board_image(), square, c_selection);
				GdkRectangle rect= Square_Rect(square);
				Invalidate(rect);
			}
		}
		if (count == 0)// && tutor_highlight.Set(*tutor_highlight+1))
		{
//			tutor_highlight[-1]= drag_from;
			tutor_no_square= drag_from;
			Draw_Square(board_image(), drag_from, c_selection_no);
		}
	}

	drag_img->Set_Function(GDK_COPY);
	drag_img->Draw_Canvas(board_image(),
			      (GdkRectangle){0, 0, Size().x, Size().y}, 
			      (GdkPoint){0,0});
	Draw_Square(drag_img(), drag_from, tutor_no_square?c_selection_no:c_selection, true);

	int w= settings.flags & b_hide_frame ? 0 : frame_width;
	last_org= (GdkPoint){(p.x - w) / square_width * square_width + w,
			(p.y - w) / square_width * square_width + w};
	pointer_pos.x= p.x - last_org.x;
	pointer_pos.y= p.y - last_org.y;

	dragging= true;
	if (statusbar)
	{
		String text= Format("%c%c–", /* UTF-8 ndash */ 
				    File(drag_from)+'a',
				    Rank(drag_from)+'1');
		if (generator)
			text += Piece_Info(drag_from);
		gtk_statusbar_push
			(statusbar, 0, text());
			 
	}
		
}

void Board::End_Dragging(GdkPoint p, guint new_state)
{
	/* P is used to calculate the release square,
	 * but the piece is still drawn at LAST_ORG  */
	Interrupt_Dragging(); 
	GdkPoint middle= {p.x - pointer_pos.x + square_width / 2,
			  p.y - pointer_pos.y + square_width / 2};
	/* Center of the dragged piece  */
	int frame_w= 0;
	if (! (settings.flags & b_hide_frame))
	{
		middle.x -= frame_width;
		middle.y -= frame_width;
		frame_w= frame_width; 
	}
	if (middle.x < 0 || middle.x >= 8 * square_width  ||
	    middle.y < 0 || middle.y >= 8 * square_width)
		return; 
	int file= sq_a + middle.x / square_width;
	int rank= sq_8 - middle.y / square_width;

	/* Square of release  */
	drag_to= Make_Invalid_Square(file, rank);
	if (/*preferences().flags & bt_turned*/turned)
		Square_Turn(drag_to);
	if (! Is_Inside_Square(drag_to))      drag_to= 0;
	
	state= new_state; 

	if (drag_to == drag_from && selection)
	{
		drag_from= selection;
		Verbose("RELEASE ON SAME SQUARE drag_from= %d",
			drag_from);
	}

	Parent_Receive_Message/*Send*/(msg_move);
}

void Board::Set_Focus(int new_focus)
{
	if (new_focus < 0)  new_focus= focus;
	if (has_focus && focus == new_focus)  return;
	if (!Is_Inside_Square(new_focus))  return; 

	if (has_focus)
	{
		invalid.Set(focus);
		GdkRectangle rect= Square_Rect(focus);
		Invalidate(rect);
	}

	focus= new_focus;
	has_focus= true;
	invalid.Set(focus);
	GdkRectangle rect= Square_Rect(focus);
	Invalidate(rect);
}

gboolean Board::Signal_Focus(GtkWidget * /* widget */ ,
			     GdkEventFocus *event,
			     gpointer user_data)
{
	Board &board= *(Board *) user_data;
	
	if (event->in)
		board.Set_Focus(-1);
	else
		board.Clear_Focus(); 

	return TRUE;
}

void Board::Set_Selection(int new_selection)
{
	if (selection)
	{
		invalid.Set(selection);
		GdkRectangle rect= Square_Rect(selection);
		Invalidate(rect);
	}
	selection= new_selection;
	invalid.Set(selection);
	GdkRectangle rect= Square_Rect(selection);
	Invalidate(rect);
}

gboolean Board::Signal_Scroll(GtkWidget *,
		       GdkEventScroll *event,
		       gpointer b)
{
	Board &board= *(Board *)b;
	if (event->direction == GDK_SCROLL_UP)
		board.Parent_Receive_Message/*Send*/(board.msg_wheel_up);
	else if (event->direction == GDK_SCROLL_DOWN)
		board.Parent_Receive_Message/*Send*/(board.msg_wheel_down); 
	return TRUE; 
}

void Board::Save_Settings()
{
	Settings_Simple_Save(settings, "board_settings");
}

String Board::Piece_Info(int to_square)
{
	assert (generator);
	Verbose_Function();
	Verbose("to_square = %d", to_square);
	int piece= generator->Get_Position()[drag_from];
	Verbose("\tpiece = %d", piece);

	/* Empty square */ 
	if (piece & pi_empty)
		return String(" ")+String(_("(empty square)"));

	Verbose("\tPiece_Color(piece) = %d", Piece_Color(piece));
	Verbose_Int(generator->Get_Position().To_Move());

	/* Not to move */ 
	if (Piece_Color(piece) != generator->Get_Position().To_Move())
	{
		return String(" ")+(Piece_Color(piece) 
				    ? String(_("(it’s not black’s turn)"))
				    : String(_("(it’s not white’s turn)")));
	}

	/* Outside of board */ 
	if (to_square < 0)
		return "";

	/* Opening name */ 
	Verbose_Pointer(&opening_book());
	if (opening_book)
	{
		move_t small_move= drag_from | to_square<<8;
		Verbose_Int(small_move);
		return String(" ")+opening_book->Name(small_move, 0, true);
	}

	return ""; 
}

