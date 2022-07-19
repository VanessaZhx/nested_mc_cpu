#include "Bond.h"
#include <math.h>

Bond::Bond(float*& data, float par, float c, int m, float y) {
	this->bond_par = par;
	this->bond_c = c;
	this->bond_m = m;
	this->bond_y = y;

	this->rn = data;
}

float Bond::price_bond(int offset) {
	float price = 0.0;
	// Loop to sum the coupon price until the maturity
	for (int i = 1; i < bond_m + 1; i++) {
		price += bond_c / pow(1.0f + bond_y + rn[bond_m * offset + i - 1], i);
	}
	// Add the face value
	price += bond_par / pow(1.0f + bond_y + rn[bond_m * offset + bond_m - 1], bond_m);
	
	return price;
}