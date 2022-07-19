#include "NestedMonteCarloVaR.h"

NestedMonteCarloVaR::NestedMonteCarloVaR(int pext, int pint, int t, float per, int port_n, float* weight) {
	this->path_ext = pext;
	this->path_int = pint;
	this->var_t = t;
	this->var_per = per;
	this->port_n = port_n;
	this->port_w = weight;

	prices = (float*)malloc((size_t)path_ext * port_n * sizeof(float));
}

NestedMonteCarloVaR::~NestedMonteCarloVaR() {
	free(prices);
	free(bond_rn);
	free(stock_rn);
}

void NestedMonteCarloVaR::bond_init(float bond_par, float bond_c, int bond_m, 
	float bond_y, float bond_x, int idx) {
	// Memory allocation
	bond_rn = (float*)malloc((size_t)path_ext * bond_m * sizeof(float));

	// Product initiation
	bond = new Bond(bond_rn, bond_par, bond_c, bond_m, bond_y);

	// add to the portfolio price
	this->port_p0 += bond_x * port_w[idx];
}

void NestedMonteCarloVaR::stock_init(float stock_s0, float stock_mu, 
	float stock_var, int stock_t, int stock_x, int idx) {
	// Memory allocation
	stock_rn = (float*)malloc((size_t)path_ext * sizeof(float));

	// Product initiation
	stock = new Stock(stock_rn, stock_s0,stock_mu, stock_var, stock_t, stock_x);

	// add to the portfolio price
	this->port_p0 += stock_s0 * stock_x * port_w[idx];
}

int NestedMonteCarloVaR::execute() {
	float* loss = (float*)malloc((size_t)path_ext * sizeof(float));
	
	//// Fill loss with negtive today's price of the portfolio
	for (int i = 0; i < path_ext; i++) {
		*(loss+i) = port_p0;
	}

	//// Random number preperation
	RNG* rng = new RNG;
	
	// bond
	rng->generate_sobol_cpu(bond_rn, bond->get_m(), path_ext);
	rng->convert_normal(bond_rn, bond->get_m() * path_ext, 0.015f);
	// stock
	rng->generate_sobol_cpu(stock_rn, 1, path_ext);
	rng->convert_normal(stock_rn, path_ext);

	cout << "Random Numbers:" << endl;
	for (int i = 0; i < path_ext; i++) {
		for (int j = 0; j < 1; j++) {
			cout << stock_rn[i * 1 + j] << " ";
		}
		cout << endl;
	}

	//// Outter Monte Carlo Simulation
	// Store by row
	int row_idx = 0;

	// Bond
	for (int i = 0; i < path_ext; i++) {
		// Store to first row of prices matrix
		prices[row_idx * path_ext + i] = bond->price_bond(i);
		
	}
	row_idx++;
	
	// Stock
	for (int i = 0; i < path_ext; i++) {
		// Store to the next row of price matrix
		prices[row_idx * path_ext + i] = stock->price_stock(i);
	}

	// Reset
	row_idx = 0;

	cout << endl << "Prices:" << endl;
	for (int i = 0; i < port_n; i++) {
		for (int j = 0; j < path_ext; j++) {
			cout << prices[i * path_ext + j] << " ";
		}
		cout << endl;
	}
	

	// Loss
	// Todo: change 1 to e^rT
	// [Test]can e^rT be ignored here to reduce calculation?
	// it doesn't affect the sorting result
	// 
	// [path_ext*n]*[n * 1]
	// Loss = price+(-p0)
	cout << endl << "Today Price:" << endl;
	cout << port_p0 << endl;
	cblas_sgemv(CblasRowMajor, CblasTrans, port_n, path_ext, -1, prices, path_ext, port_w, 1, 1, loss, 1);

	cout << endl << "Loss:" << endl;
	for (int i = 0; i < path_ext; i++) {
		cout << loss[i] << " ";
	}
	cout << endl;

	// Sort
	std::sort(loss, loss + path_ext);

	cout << endl << "Sorted Loss:" << endl;
	for (int i = 0; i < path_ext; i++) {
		std::cout << loss[i] << " ";
	}
	cout << endl;

	// Calculate var and cvar
	int pos = (int)floor(path_ext * var_per);

	float var = loss[pos];
	float cvar = 0;
	for (int i = pos; i < path_ext; i++) {
		cvar += loss[i];
	}
	cvar /= path_ext - pos;

	cout << endl << endl;
	cout << "var:" << var << endl;
	cout << "cvar:" << cvar << endl;

	free(loss);

	return 0;
}
