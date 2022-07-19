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

int NestedMonteCarloVaR::execute() {
	float* loss = (float*)malloc((size_t)path_ext * sizeof(float));
	// Fill loss with negtive today's price of the portfolio
	for (int i = 0; i < path_ext; i++) {
		loss[i] = (float)port_p0;
	}
	// Random number preperation
	RNG* rng = new RNG;
	rng->generate_sobol_cpu(bond_rn, bond->get_m(), path_ext);
	rng->convert_normal(bond_rn, bond->get_m() * path_ext, 0.015f);

	for (int i = 0; i < path_ext; i++) {
		for (int j = 0; j < bond->get_m(); j++) {
			cout << bond_rn[i * bond->get_m() + j] << " ";
		}
		cout << endl;
	}

	// Outter Monte Carlo Simulation
	for (int i = 0; i < path_ext; i++) {
		// Bond
		// Store to first row of prices matrix
		prices[i] = bond->price_bond(i);
		cout << prices[i] << " ";
	}

	// Loss
	// Todo: change 1 to e^rT
	// [Test]can e^rT be ignored here to reduce calculation?
	// it doesn't affect the sorting result
	// 
	// [path_ext*n]*[n * 1]
	// Loss = price+(-p0)
	cout << port_p0 << endl;
	cblas_sgemv(CblasRowMajor, CblasTrans, port_n, path_ext, -1, prices, path_ext, port_w, 1, 1, loss, 1);

	cout << endl;
	for (int i = 0; i < path_ext; i++) {
		cout << loss[i] << " ";
	}

	// Sort
	std::sort(loss, loss + path_ext);
	cout << endl;
	for (int i = 0; i < path_ext; i++) {
		std::cout << loss[i] << " ";
	}

	// Calculate var and cvar
	int pos = (int)floor(path_ext * var_per);

	float var = loss[pos];
	float cvar = 0;
	for (int i = pos; i < path_ext; i++) {
		cvar += loss[i];
	}
	cvar /= path_ext - pos;

	cout << endl;
	cout << "var:" << var << endl;
	cout << "cvar:" << cvar << endl;

	free(loss);

	return 0;
}
