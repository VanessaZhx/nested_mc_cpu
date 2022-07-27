#include "Bond.h"
#include <math.h>

Bond::Bond(float*& data, float par, float c, int m, float* y, float sig) {
	this->bond_par = par;
	this->bond_c = c;
	this->bond_m = m;
	this->bond_y = y;
	this->sigma = sig;

	this->rn = data;
}

float Bond::price_bond(int offset) {
	return price_bond_with_delta_yield(rn[offset]);
}

float Bond::price_bond_with_delta_yield(float dy) {
	float price = 0.0f;
	// Loop to sum the coupon price until the maturity
	for (int i = 0; i < bond_m; i++) {
		price += bond_c / pow(1.0f + (bond_y[i] + dy) / 100, i + 1);
	}
	// Add the face value
	price += bond_par / pow(1.0f + (bond_y[bond_m - 1] + dy) / 100, bond_m);
	
	return price;
}