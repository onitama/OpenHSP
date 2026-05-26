#ifndef RANDOM_H

#include <stdio.h>

class RAND {
	public:

	RAND(int _seed = 0);

	void setseed(int _seed);
	int get(int n);
	void dummyget(int n);

	int	seed;
};

#endif
#define RANDOM_H
