#include "score.hh"

#include "format.hh"
#include "gettext.hh"

#include <stdio.h>
#include <locale.h>

String Format_Score(int score, int standard)
{
	if (score < score_not_win)
	{
		if (score > score_not_lose)
		{
			if (standard==1)
				return Format("%d", score);
			else
			{
				int sign= score;
				score= abs(score); 
				return Format("%s%d%s%c%c",
					      sign < 0 ? "−" : sign ? "+" : "±",
					      score / 100,
					      localeconv()->decimal_point,
					      '0' + score / 10 % 10,
					      '0' + score % 10);
			}
		}
		else
		{
			if (score >= score_almost_is_mate)
			{
				int n= (score - score_almost_is_mate) / 2;
				assert (n >= 0 && n < 100);
				return Format(standard==2 ? _("probably is mate in %d")
					      : "~%%%d", n);
			}
			else
			{
				int n= (score - score_is_mate) / 2;
				assert (n >= 0);
				return Format(standard==2?_("is mate in %d")
					      : "%%%d", n);
			}
		}
	}
	else
	{
		if (score > score_almost_mate_in_1)
		{
			int n= (score_mate_in_1 - score) / 2 + 1;
			assert (n >= 0 && n < 100);
			return Format(standard==2?_("mate in %d")
				      : "#%d", n);
		}
		else
		{
			int n= (score_almost_mate_in_1 - score) / 2 + 1;
			assert (n >= 0 && n < 100);
			return Format(standard==2?_("probably mate in %d")
				      : "~#%d", n);
		}
	}
}

int Parse_Score(String text)
{
	int score;
	if (1 == sscanf(text(), "%d", & score))
	{
		/* ok */ 
	}
	else if (text[0] == '#')
	{
		if (1 == sscanf(text() + 1,
			      "%d",
			      & score))
			score= Score_Mate_In(score);
		else
			score= score_even; 
		
	}
	else if (text[0] == '%')
	{
		if (1 == sscanf(text() + 1,
			      "%d",
			      & score))
			score= Score_Is_Mate_In(score);
		else
			score= score_even; 
					
	}
	else
		score= score_even; 

	/* special support for engines that use mate_in_1 == 29999 */ 
	if (score <= 30000 && score > 29500)
	{
		score += (score_mate_in_1-29999);
	}
	else if (score >= -30000 && score < -29500)
	{
		score += (score_is_mate - -30000);
	}

	return score; 
}
