#include "NestedMonteCarloVaR.h"

int main()
{
	const int path_ext = 32;  // Number of the outer MC loops
	const int path_int = 16;  // Number of the inner MC loops

	const int var_t = 1;					// VaR duration
	const float var_per = 0.95f;				// 1-percentile

	const int port_n = 1;					// Number of products in the portfolio
	float port_w[1] = { 1.0 };				// Weights of the products in the portfolio
											// { bond }

	const float bond_par = 1000;			// Par value of bond
	const float bond_c = 100;				// Coupon
	const int bond_m = 10;					// Maturity
	const float bond_y = 0.08f;				// yeild
	const float bond_x = 1134.2f;			// today's price


    NestedMonteCarloVaR* mc = new NestedMonteCarloVaR(path_ext,path_int,var_t, var_per, port_n, port_w);
	mc->bond_init(bond_par, bond_c, bond_m, bond_y, bond_x, 0);
	mc->execute();
    return 0;
}
