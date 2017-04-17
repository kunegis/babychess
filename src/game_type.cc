#include "game_type.hh"

/* including nothing */ 
int Pure_Light_Pieces(const Material &material, int w, int b)
{
	int w_pieces= material(w, pi_B) + material(w, pi_N);
	int b_pieces= material(b, pi_N) + material(b, pi_N);

	if (w_pieces == b_pieces)
	{
		return gt_end; 
	}
	
	if (w_pieces < b_pieces)  return 0;

	int diff= w_pieces - b_pieces;
	if (diff <= 2)  return gt_KM_K + w;
	return gt_end;
}

int Game_Type(const Material &material)
{
	assert (material(pi_wK) == 1);
	assert (material(pi_bK) == 1);

	/* queens */ 
	if (material(pi_wQ) || material(pi_bQ))  
	{
		if (material(pi_wQ) && material(pi_bQ))   return gt_middle;
		else if (material(pi_wQ))                 return gt_KM_K;
		else                                      return gt_K_KM;
	}

	/* rooks */ 
	if (material(pi_wR) || material(pi_bR))  
	{
		if (material(pi_wR) && material(pi_bR))   return gt_end;
		else if (material(pi_wQ))                 return gt_KR_K;
		else                                      return gt_K_KR;
	}
	
	if (material(pi_wP) == 0 && material(pi_bP) == 0)
	{
		return Pure_Light_Pieces(material, 0, 1) || Pure_Light_Pieces(material, 1, 0);
	}

	return gt_end;
}
