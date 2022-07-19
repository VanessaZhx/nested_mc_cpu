#pragma once
#include<math.h>

class Stock
{
public:
	Stock(float*& data, float s0, float mu, float var, int t) {
		this->s0 = s0;
		this->mu = mu;
		this->var = var;
		this->t = t;

		this->rn = data;
	}

	float price_stock(int offset) {
		float price = s0 * exp((mu - 0.5 * var * var) * t + var * sqrtf(t) * rn[offset]);
		return price;
	};
private:
	float s0;
	float mu;
	float var;
	int t;

	float* rn;		  // Random number sequence
};

