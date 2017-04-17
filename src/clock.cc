#include "clock.hh"

#include <math.h>

#include <algorithm>

void Clock::Wdg_Draw(Canvas &canvas)
{
	Verbose_Function(); 
	Verbose_Int(phase);

	GdkColor black;
	gdk_color_black(gdk_colormap_get_system(), &black);
	GdkColor white;
	gdk_color_white(gdk_colormap_get_system(), &white);

	static GdkColor background[precision];
	static bool have= false;
	if (! have)
	{
		for (int i= 0;  i < precision;  ++i)
		{
			Canvas::Alloc_Color
				(background[i], 
				 RGB(0xFF, 0xFF,
				     0xFF-0xE0*abs(i%precision-precision/2)/precision));


		}
		have= true;
	}

//	GdkColor white;
//	gdk_color_white(gdk_colormap_get_system(), &white);

	/* background */ 
	gdk_gc_set_foreground(canvas.Get_GC(), Timer::Is() ?
			      background+phase%precision : &white);
	canvas.Draw_Rect((GdkRectangle){0, 0, Size().x, Size().y});

	/* ticks */ 
	canvas.Set_Line_Mode(std::max(std::min(Size().x, Size().y) / 3 / 10 / 2, 1));

	gdk_gc_set_foreground(canvas.Get_GC(), &black);
	for (int i= 0;  i < 12;  ++i)
	{
		int x=  Size().x / 2 + (int)(((double)Size().x)
						 * sin(M_PI * i / 6) * 0.5);
		int y=  Size().y / 2 - (int)(((double)Size().y)
						 * cos(M_PI * i / 6) * 0.5);
		int ix= Size().x / 2 + (int)(((double)Size().x)
						 * sin(M_PI * i / 6) * 0.4);
		int iy= Size().y / 2 - (int)(((double)Size().y)
						 * cos(M_PI * i / 6) * 0.4);
		gdk_draw_line(canvas.Get_Drawable(), canvas.Get_GC(),
			      x, y, ix, iy);
	}

	/* lines */ 
	int hour_x= Size().x / 2 + (int)(((double)(Size().x / 3)) *
					     sin(M_PI * time / 18360));
	int hour_y= Size().y / 2 - (int)(((double)(Size().y / 3)) *
					     cos(M_PI * time / 18360));
	canvas.Set_Line_Mode(std::max(std::min(Size().x, Size().y) / 3 / 5, 1));
	gdk_gc_set_foreground(canvas.Get_GC(), &black);
	gdk_draw_line(canvas.Get_Drawable(), canvas.Get_GC(),
		      Size().x / 2, Size().y / 2,
		      hour_x, hour_y);
	int min_x= Size().x / 2 + (int)(((double)(Size().x / 2)) *
					    sin(M_PI * time / 1800));
	int min_y= Size().y / 2 - (int)(((double)(Size().y / 2)) *
					    cos(M_PI * time / 1800));
	canvas.Set_Line_Mode(std::max(std::min(Size().x, Size().y) / 3 / 10, 1));
	gdk_draw_line(canvas.Get_Drawable(), canvas.Get_GC(),
		      Size().x / 2, Size().y / 2,
		      min_x, min_y);
	int sec_x= Size().x / 2 + (int)(((double)(Size().x / 2)) *
					    sin(M_PI * time / 30));
	int sec_y= Size().y / 2 - (int)(((double)(Size().y / 2)) *
					    cos(M_PI * time / 30));
	canvas.Set_Line_Mode(std::max(std::min(Size().x, Size().y) / 3 / 10 / 2, 1));
	gdk_draw_line(canvas.Get_Drawable(), canvas.Get_GC(),
		      Size().x / 2, Size().y / 2,
		      sec_x, sec_y);
}

void Clock::Set_Blink(bool blink)
{
	Verbose_Function();
	Verbose_Int(blink);
	if (blink)
	{
		if (Timer::Is())
			return;
		phase= 0;
		/* Length of one period in milliseconds divided by
		 * PRECISION */ 
		Start(1000 * 5/3 / precision);
	}
	else
	{
		if (! Timer::Is()) 
			return; 
		Stop();
		Invalidate();
	}
}

void Clock::Timer_Alarm()
{
	Verbose_Function(); 
	++phase;
	Invalidate();
}

