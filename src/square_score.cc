#include "square_score.hh"

#include "square.hh"
#include "square_value.hh"
#include "game_type.hh"

int square_scores[gt_count][2][pi_count]=
{
	/*P        N       B       R       Q   K                     */
	{{sv_wP,   sv_g_c, sv_g_c, -1,     -1, sv_wK   } , /* middle */
	 {sv_bP,   sv_g_c, sv_g_c, -1,     -1, sv_bK   }}, 
	{{-1,      -1,     -1,     -1,     -1, sv_g_c  } , /* KM-K   */
	 {-1,      -1,     -1,     -1,     -1, sv_dd_c }},
	{{-1,      -1,     -1,     -1,     -1, sv_dd_c } , /* K-KM   */
	 {-1,      -1,     -1,     -1,     -1, sv_g_c  }},
	{{-1,      -1,     -1,     -1,     -1, -1      } , /* KR-K   */
	 {-1,      -1,     -1,     -1,     -1, sv_dd_c }},
	{{-1,      -1,     -1,     -1,     -1, sv_dd_c } , /* K-KR   */
	 {-1,      -1,     -1,     -1,     -1, -1      }},
	{{sv_wrace,sv_g_c, sv_g_c, sv_g_c, -1, sv_g_c  } , /* end    */ 
	 {sv_brace,sv_g_c, sv_g_c, sv_g_c, -1, sv_g_c  }},
};

int Square_Score(int square,
		 int piece,
		 int game_type)
{
	int pt= piece & pi_type; 
	assert (pt >= 0 && pt < pi_count);
	assert (game_type >= 0 && game_type < gt_count);
	assert (Is_Inside_Square(square)); 

	int sv= square_scores[game_type][Piece_Color(piece)][pt];
	if (sv < 0)  return 0;
	return Square_Value(square, sv);
}
