#pragma once
#ifndef NMC_H
#define NMC_H

#include "RNG.h"
#include "Bond.h"
#include "Stock.h"
#include <mkl.h>
#include <math.h>
#include <algorithm>

using namespace std;

class NestedMonteCarloVaR {
public:
	NestedMonteCarloVaR(int pext, int pint, 
		int t, float per, int port_n, float* weight);

	~NestedMonteCarloVaR();

	void bond_init(float bond_par, float bond_c, int bond_m, 
					float bond_y, float bond_x, int idx);

	void stock_init(float stock_s0, float stock_mu, float stock_var, 
					int stock_t, int stock_x, int idx);


	int execute();

private:
	int path_ext = 0;  // Number of the outer MC loops
	int path_int = 0;  // Number of the inner MC loops

	int var_t = 0;			// VaR duration
	float var_per = 0;		// 1-percentile

	int port_n = 0;			// Number of products in the portfolio
	float* port_w = 0;		// Weights of the products in the portfolio
	float port_p0 = 0;		// Today's price of the portfolio

	Bond* bond = NULL;
	float* bond_rn = NULL;	   // Pointer to the bond's RN sequence

	Stock* stock = NULL;
	float* stock_rn = NULL;


	float* prices; // Pointer to the matrix of each of the product's prices
};


#endif // !NMC_H

