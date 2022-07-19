#pragma once
#include<math.h>

class Stock{

public:
	Stock(float*& data, float s0, float mu, float var, int t, int x);
	float price_stock(int offset);
private:
	float s0;
	float mu;
	float var;
	int t;
	float x;

	float* rn;		  // Random number sequence
};

