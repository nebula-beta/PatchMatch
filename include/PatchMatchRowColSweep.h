#ifndef __PATCH_MATCH_ROW_COL_SWEEP_H__
#define __PATCH_MATCH_ROW_COL_SWEEP_H__

#include "PatchMatchBase.h"

class PatchMatchRowColSweep : public PatchMatchBase
{
public:
	PatchMatchRowColSweep(float alpha, float gamma, float tau_c, float tau_g);
	void SpatialPropagation(int y, int x, int view, int iter);
	void StartIteration(int iter);
};
#endif //__PATCH_MATCH_ROW_COL_SWEEP_H__
