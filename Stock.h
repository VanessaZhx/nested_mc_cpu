#pragma once
#include<math.h>

class Stock
{
public:
	Stock(float*& data, float s0, float mu, float var, int t, int x) {
		this->s0 = s0;
		this->mu = mu;
		this->var = var;
		this->t = t;
		this->x = x;

		this->rn = data;
	}

	float price_stock(int offset) {
		float price = s0 * exp((mu - 0.5f * var * var) * t 
						+ var * sqrtf(t) * rn[offset]);

		return price * x;
	};
private:
	float s0;
	float mu;
	float var;
	int t;
	float x;

	float* rn;		  // Random number sequence
};

