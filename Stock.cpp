#include "Stock.h"

Stock::Stock(float*& data, float s0, float mu, float var, int t, int x) {
	this->s0 = s0;
	this->mu = mu;
	this->var = var;
	this->t = t;
	this->x = x;

	this->rn = data;
}


float Stock::price_stock(int offset) {
	float price = s0 * exp((mu - 0.5f * var * var) * t
		+ var * sqrtf(t) * rn[offset]);

	return price * x;
}

float Stock::price_single_stock_with_z(float z) {
	float price = s0 * exp((mu - 0.5f * var * var) * t
		+ var * sqrtf(t) * z);
	return price * x;
}