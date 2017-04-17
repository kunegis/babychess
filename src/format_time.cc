#include "format_time.hh"

#include "format.hh"
#include "verbose.hh"
#include "gettext.hh"

#include <locale.h>

String Format_Time(int s,
		   int ms_digits,
		   int ms)
{
	Verbose("Format_Time(%d, %d, %d)", s, ms_digits, ms);

	assert (s >= 0);
	assert (ms_digits >= -1);
	assert (ms >= 0 && ms < 1000);

	String result;
	String unit;

	int min= s / 60;
	if (min != 0)
	{
		int h= min / 60;
		if (h != 0)
		{
			unit= " h";
			int day= h / 24;
			if (day != 0)
			{
				result += Format("%d", day);
				result += Format(" %s ", _("days"));
			}
			result += Format("%d", h % 24);
			result += ':';
			result += (char)('0' + min % 60 / 10);
			result += (char)('0' + min % 10);
		}
		else
		{
			unit= " ";
			unit += "min";
			result += Format("%d", min % 60);
		}
		result += (char)':';
		result += (char)('0' + s % 60 / 10);
		result += (char)('0' + s % 10);
	}
	else
	{
		unit= " s";
		result += Format("%d", s % 60);
	}

	if (ms_digits < 0)
	{
		if (s == 0)       ms_digits= 2;
		else if (s < 10)  ms_digits= 1;
		else              ms_digits= 0;
	}
	if (ms_digits)
	{
		result += localeconv()->decimal_point;
		result += (char)('0' + ms / 100);
		if (ms_digits >= 2)
		{
			result += (char)('0' + ms / 10 % 10);
			if (ms_digits >= 3)
				result += (char)('0' + ms % 10);
		}
	}

	
	Verbose("\twithout unit: \"%s\"", result());
	Verbose_Str(unit()); 
	result += unit;
	Verbose("\twith unit: \"%s\"", result());
	
	return result;
}
