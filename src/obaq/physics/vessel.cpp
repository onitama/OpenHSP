#include "vessel.h"
#include <math.h>

//
// OBAQ physics library
// Copyright   : Zener Works, Inc/kuni/onitama
// License     : CC BY-NC-ND 4.0
// See OBAQlicense.txt for information
// This is a part of HSP related product.
// http://hsp.tv/
//
//

//
// vessel
//

void VESSEL::proc(int procCount)
{
	rockbase.proc(procCount, true);
	rockbase.proc2();
}

