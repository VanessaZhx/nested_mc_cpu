#include "Bond.h"
#include <math.h>

Bond::Bond(float*& data, float par, float c, int m, float* y) {
	this->bond_par = par;
	this->bond_c = c;
	this->bond_m = m;
	this->bond_y = y;

	this->rn = data;
}

float Bond::price_bond(int offset) {
	return price_bond_with_yield(&rn[bond_m * offset]);
}

float Bond::price_bond_with_yield(float* dy) {
	float price = 0.0;
	// Loop to sum the coupon price until the maturity
	for (int i = 0; i < bond_m; i++) {
		price += bond_c / pow(1.0f + bond_y[i] / 100 + dy[i] / 100, i + 1);
	}
	// Add the face value
	price += bond_par / pow(1.0f + bond_y[bond_m - 1] / 100 + dy[bond_m - 1] / 100, bond_m);

	return price;
}