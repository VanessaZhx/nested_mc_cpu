#pragma once
#ifndef BOND_H
#define BOND_H

#include "RNG.h"

class Bond {

public:
	Bond(float*& data, float par, float c, int m, float* y);

	float price_bond(int offset);		// Price the bond with an random 
										// sequence at offset
	float price_bond_with_yield(float *dy);

	int get_m() { return bond_m; }

private:
	float bond_par;   // Par value of bond
	float bond_c;     // Coupon
	int bond_m;		  // Maturity
	float* bond_y;     // yeild curve

	float* rn;		  // Random number sequence
};

#endif // !BOND_H



