#pragma once
#ifndef BOND_H
#define BOND_H

#include "RNG.h"

class Bond {

public:
	Bond(float*& data, float par, float c, int m, float* y, float sig);

	float price_bond(int offset);		// Price the bond with an random 
										// sequence at offset
	float price_bond_with_delta_yield(float dy);

	// Getters
	int get_m() { return bond_m; }
	float get_sigma() { return sigma; }

private:
	float bond_par;   // Par value of bond
	float bond_c;     // Coupon
	int bond_m;		  // Maturity
	float* bond_y;     // yeild curve
	float sigma;		// Sigma for generate random yield change

	float* rn;		  // Random number sequence
};

#endif // !BOND_H



