#include "NestedMonteCarloVaR.h"

int main()
{
	const int path_ext = 32;  // Number of the outer MC loops
	const int path_int = 16;  // Number of the inner MC loops

	const int var_t = 1;					// VaR duration
	const float var_per = 0.95f;				// 1-percentile

	const int port_n = 2;					// Number of products in the portfolio
	float port_w[2] = { 0.5f, 0.5f };		// Weights of the products in the portfolio
											// { bond, stock }

	const float bond_par = 1000.0f;			// Par value of bond
	const float bond_c = 100.0f;			// Coupon
	const int bond_m = 10;					// Maturity
	const float bond_y = 0.08f;				// yeild
	const float bond_x = 1134.2f;			// today's price

	const float stock_s0 = 300.0f;			// Start value of stock
	const float stock_mu = 0.01f;			// risk free(or mean)
	const float stock_var = 0.13;			// Volatility
	const int stock_t = 1;					// Steps(trade days)
	const int stock_x = 20;					// Number of shares
    
		
	NestedMonteCarloVaR* mc = new NestedMonteCarloVaR(path_ext,path_int,var_t, var_per, port_n, port_w);
	mc->bond_init(bond_par, bond_c, bond_m, bond_y, bond_x, 0);
	mc->stock_init(stock_s0, stock_mu, stock_var, stock_t, stock_x, 1);
	//mc->execute();
    return 0;
}

/*****************************************************************************
 *                               fir_test.cpp
 *
 * FIR class testing.
 *
 * Zhang Ming, 2010-03
 *****************************************************************************/

//
//#define BOUNDS_CHECK
//
//#include <iostream>
//#include <fir.h>
//
//
//using namespace std;
//using namespace itlab;
