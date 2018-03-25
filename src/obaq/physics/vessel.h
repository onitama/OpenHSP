#ifndef VESSEL_H

//
// OBAQ physics library
// Copyright   : Zener Works, Inc/kuni/onitama
// License     : CC BY-NC-ND 4.0
// See OBAQlicense.txt for information
// This is a part of HSP related product.
// http://hsp.tv/
//
//

#include <stdio.h>
#include <stdlib.h>

#include "rock.h"
#include "ptttypes.h"

#include "gamePhysics.h"

class VESSEL {
	public:
		ROCKBASE		rockbase;
		void proc(int procCount);
};

#endif
#define VESSEL_H
