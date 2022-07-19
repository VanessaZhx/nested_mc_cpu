#pragma once
#include<math.h>

class Stock{

public:
	Stock(float*& data, float s0, float mu, float var, int t, int x);

	float get_s0() { return s0;}

	float get_x() { return x; }

	float price_stock(int offset);

	float price_single_stock_with_z(float z);

private:
	float s0;
	float mu;
	float var;
	int t;
	float x;

	float* rn;		  // Random number sequence
};

