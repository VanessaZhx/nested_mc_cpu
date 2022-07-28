﻿#include "NestedMonteCarloVaR.h"

int main()
{
	const int path_ext = 5;  // Number of the outer MC loops
	const int path_int = 3;  // Number of the inner MC loops

	const int var_t = 1;					// VaR duration
	const float var_per = 0.95f;				// 1-percentile

	const int port_n = 3;					// Number of products in the portfolio
	float port_w[port_n] = { 0.0f, 1.0f, 0.0f };		// Weights of the products in the portfolio
														// { bond, stock, basket option}
	const float risk_free = 0.02f;

	const float bond_par = 1000.0f;			// Par value of bond
	const float bond_c = 100.0f;			// Coupon
	const int bond_m = 10;					// Maturity
	float bond_y[bond_m] = { 
			5.00f, 5.69f, 6.09f, 6.38f, 6.61f,
			6.79f, 6.94f, 7.07f, 7.19f, 7.30f 
	};										// yeild curve
	const float sigma = 1.5f;				// sigma

	const float stock_s0 = 300.0f;			// Start value of stock
	const float stock_mu = risk_free;			// risk free(or mean)
	const float stock_var = 0.13f;			// Volatility
	const int stock_x = 20;					// Number of shares

	Stock* s1 = new Stock(stock_s0, stock_mu, stock_var, 100);
	Stock* s2 = new Stock(stock_s0, stock_mu, stock_var, 100);
	const int bskop_n = 2;								// Number of stocks in basket
	const float bskop_k = 31000.0f;						// Execution price
	const int bskop_t = 1;								// Maturity of basket option
	Stock bskop_stocks[bskop_n] = { *s1, *s2 };			// List of stocks
	float bskop_cov[bskop_n * bskop_n] = { 1.0f, 0.5f,	
										   0.5f, 1.0f };	// Covariance matrix
	float bskop_w[bskop_n] = { 0.5f, 0.5f };				// weight
		
	NestedMonteCarloVaR* mc = new NestedMonteCarloVaR(
		path_ext,path_int,
		var_t, var_per,
		port_n, port_w,
		risk_free
	);
	mc->bond_init(bond_par, bond_c, bond_m, bond_y, sigma, 0);
	mc->stock_init(stock_s0, stock_mu, stock_var, stock_x, 1);
	mc->bskop_init(bskop_n, bskop_stocks, bskop_cov, bskop_k, bskop_w, bskop_t, 2);
	mc->execute();
    return 0;
}