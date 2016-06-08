#include "stdafx.h"
#include "global.h"

double Length(int indx1, int indx2)
{
	const int x = coordinates[indx2].first - coordinates[indx1].first;
    const int y = coordinates[indx2].second - coordinates[indx1].second;
    return sqrt(x*x + y*y);
}