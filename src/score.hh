#ifndef SCORE_HH
#define SCORE_HH

/* The score of a chess position.  Scores are of type "int", although
 * "int16_t" would suffice.  
 *
 * See etc/SCORE. 
 */

#include "string.hh"
#include "inline.hh"

enum {score_mate_in_1=         32766,  /* biggest int16_t */
      score_is_mate=          -32767,  /* smallest int16_t */
      score_almost_mate_in_1=  31999,
      score_almost_is_mate=   -31999,
      score_not_win=           31000,
      score_not_lose=         -31000,
      score_even=                  0};

String Format_Score(int score, int standard= 1);
/* STANDARD=
 *   0:  human-readable, int UTF-8 (with mate indicated by “#6”. 
 *   1:  as used in the CHESS_EVALUATOR protocol.
 *   2:  human-readable, long version (with full sentences like “mate in 6”. 
 */


Inline int Score_Mate_In(int n)
{
	assert (n >= 1);
	return score_mate_in_1 - 2 * (n - 1);
}

Inline int Score_Almost_Mate_In(int n)
{
	assert (n >= 1);
	return score_almost_mate_in_1 - 2 * (n - 1); 
}

Inline int Score_Is_Mate_In(int n)
{
	assert (n >= 0);
	return score_is_mate + 2 * n;
}

Inline int Score_Almost_Is_Mate_In(int n)
{
	assert (n >= 0);
	return score_almost_is_mate + 2 * n;
}

Inline bool Is_Sure_Score(int score)
	/* Whether SCORE is win or lose */
{
	return (score < score_almost_is_mate ||
		score > score_almost_mate_in_1);
}

Inline bool Is_Sure_Win(int score)
{
	return score > score_almost_mate_in_1;
}

Inline bool Is_Sure_Lose(int score)
{
	return score < score_almost_is_mate;
}

Inline bool Is_Win_Lose(int score)
	/* Whether SCORE is a win or a lose.  Return TRUE even if
	 * SCORE is unsure win or lose.  
	 */
{
	return (score < score_not_lose || score > score_not_win);
}

Inline int Score_Add(int score_a,  int score_b)
	/* "Adds" the two scores.  SCORE_A can
	 * be any score; SCORE_B must not be win or lose.  This is not
	 * an addition of integers, because if SCORE_A is win or lose,
	 * then the return value is SCORE_A, regardless of SCORE_B.  
	 */     
{
	assert (! Is_Win_Lose(score_b));
  
	return (Is_Win_Lose(score_a)
		? score_a
		: score_a + score_b);
}

Inline int Score_Negate(int score)
{
	int result= - score;
	if (result > score_not_win)
		-- result;
	else if (result < score_not_lose)
		++ result;
	return result;
}

int Parse_Score(String text);
/* TEXT is a score in textual representation as returned by
 * Format_Score().  Return SCORE_NO on parse error. 
 */

#endif /* ! SCORE_HH */
