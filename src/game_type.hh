#ifndef GAME_TYPE_HH
#define GAME_TYPE_HH

/* classification of endgame positions */ 

/* light pieces := bishop, knight
 * pure         := whithout pawns
 */

#include "material.hh"

/* when changing this:  change also square_score.cc */ 

enum {  gt_middle  ,   

	gt_KM_K    ,   /* mate with no centralisation of own pieces,
		        * decentralisation of opp. king is more
		        * important than own king centralisation */ 
	gt_K_KM    ,   

	gt_KR_K    ,   /* mate with rooks */
	gt_K_KR    ,   

	gt_end     ,   /* endgame:  promote pawns and centralize */ 
	
	gt_count
};

int Game_Type(const Material &);

#endif /* ! GAME_TYPE_HH */ 
