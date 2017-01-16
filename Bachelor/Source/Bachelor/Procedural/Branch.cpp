#include "Bachelor.h"
#include "Branch.h"

void FBranch::ResetForNextGrowthIteration() {
	GrowDirection = FVector(0);
	GrowCount = 0;
}
