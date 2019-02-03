#ifndef __PATCH_MATCH_RED_BLACK_H__
#define __PATCH_MATCH_RED_BLACK_H__
#include "PatchMatchBase.h"

class PatchMatchRedBlack : public PatchMatchBase
{
public:
	PatchMatchRedBlack(float alpha, float gamma, float tau_c, float tau_g);
	void SpatialPropagation(int y, int x, int view, int iter);
	void StartIteration(int iter);
};
#endif //__PATCH_MATCH_RED_BLACK_H__
