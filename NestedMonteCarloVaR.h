#pragma once
#ifndef NMC_H
#define NMC_H

#include "RNG.h"
#include "Bond.h"
#include "Stock.h"
#include "BasketOption.h"
#include <mkl.h>
#include <math.h>
#include <algorithm>

using namespace std;

class NestedMonteCarloVaR {
public:
	NestedMonteCarloVaR(int pext, int pint, 
		int t, float per, int port_n, float* weight, float risk_free);

	~NestedMonteCarloVaR();

	void bond_init(float bond_par, float bond_c, int bond_m, 
					float* bond_y, float sig, int idx);

	void stock_init(float stock_s0, float stock_mu, float stock_var, 
					int stock_x, int idx);

	void bskop_init(int bskop_n, Stock* bskop_stocks, float* bskop_cov, 
					float bskop_k, float* bskop_w, int bskop_t, int idx);

	int execute();

private:
	int path_ext = 0;  // Number of the outer MC loops
	int path_int = 0;  // Number of the inner MC loops

	int var_t = 0;			// VaR duration
	float var_per = 0;		// 1-percentile

	int port_n = 0;			// Number of products in the portfolio
	float* port_w = 0;		// Weights of the products in the portfolio
	float port_p0 = 0;		// Today's price of the portfolio

	float risk_free = 0;	// Risk free rate

	Bond* bond = NULL;
	float* bond_rn = NULL;	   // Pointer to the bond's RN sequence

	Stock* stock = NULL;
	float* stock_rn = NULL;		// Pointer to the stock's RN sequence

	BasketOption* bskop = NULL;
	int bskop_t = 0;					// Maturity of option
	float* bskop_rn = NULL;		// Pointer to the basket option's RN sequence


	float* prices; // Pointer to the matrix of each of the product's prices
};


#endif // !NMC_H

