#include "NestedMonteCarloVaR.h"

int main()
{
	const int path_ext = 5;  // Number of the outer MC loops
	const int path_int = 2;  // Number of the inner MC loops

	const int var_t = 1;					// VaR duration
	const float var_per = 0.95f;				// 1-percentile

	const int port_n = 3;					// Number of products in the portfolio
	float port_w[port_n] = { 0.0f, 0.0f, 1.0f };		// Weights of the products in the portfolio
											// { bond, stock }

	const float bond_par = 1000.0f;			// Par value of bond
	const float bond_c = 100.0f;			// Coupon
	const int bond_m = 10;					// Maturity
	const float bond_y = 0.08f;				// yeild
	const float bond_x = 1134.2f;			// today's price

	const float stock_s0 = 300.0f;			// Start value of stock
	const float stock_mu = 0.01f;			// risk free(or mean)
	const float stock_var = 0.13f;			// Volatility
	const int stock_t = 1;					// Steps(trade days)
	const int stock_x = 20;					// Number of shares

	Stock* s1 = new Stock(stock_s0, stock_mu, stock_var, stock_t, 100);
	Stock* s2 = new Stock(stock_s0, stock_mu, stock_var, stock_t, 100);
	const int bskop_n = 2;
	const float bskop_k = 31000.0f;
	Stock bskop_stocks[bskop_n] = { *s1, *s2 };
	float bskop_cov[bskop_n * bskop_n] = { 1.0f, 0.5f,
										   0.5f, 1.0f };
	float bskop_w[bskop_n] = { 0.5f, 0.5f };
		
	NestedMonteCarloVaR* mc = new NestedMonteCarloVaR(path_ext,path_int,var_t, var_per, port_n, port_w);
	mc->bond_init(bond_par, bond_c, bond_m, bond_y, bond_x, 0);
	mc->stock_init(stock_s0, stock_mu, stock_var, stock_t, stock_x, 1);
	mc->bskop_init(bskop_n, bskop_stocks, bskop_cov, bskop_k, bskop_w, 2);
	mc->execute();
    return 0;
}