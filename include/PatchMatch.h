#ifndef __PATCH_MATCH_H__
#define __PATCH_MATCH_H__

#include "PatchMatchBase.h"

class PatchMatch : public PatchMatchBase
{
public:
	PatchMatch(float alpha, float gamma, float tau_c, float tau_g);
	void SpatialPropagation(int y, int x, int view, int iter);
	void StartIteration(int iter);

};

#endif // __PATCH_MATCH_H__
