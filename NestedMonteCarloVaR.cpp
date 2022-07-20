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
	free(bskop_rn);
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

void NestedMonteCarloVaR::bskop_init(int bskop_n, Stock* bskop_stocks, 
	float* bskop_cov, float bskop_k, float* bskop_w, int idx) {

	// Memory allocation
	bskop_rn = (float*)malloc((size_t)path_ext * path_int * bskop_n * sizeof(float));

	// Product initiation
	bskop = new BasketOption(bskop_rn, bskop_n, bskop_stocks, bskop_cov,  bskop_k, bskop_w);

	// add to the portfolio price
	for (int i = 0; i < bskop_n; i++) {
		this->port_p0 += bskop_stocks[i].get_s0() * bskop_stocks[i].get_x() 
							* bskop_w[i] * port_w[idx];
	}
}

int NestedMonteCarloVaR::execute() {
	float* loss = (float*)malloc((size_t)path_ext * sizeof(float));
	
	//// Fill loss with negtive today's price of the portfolio
	for (int i = 0; i < path_ext; i++) {
		*(loss+i) = port_p0;
	}

	// ===================================================================
	// Random number preperation
	RNG* rng = new RNG;
	
	// bond
	// [path_ext, m]
	rng->generate_sobol_cpu(bond_rn, bond->get_m(), path_ext);
	rng->convert_normal(bond_rn, bond->get_m() * path_ext, 0.015f);

	// stock
	// [path_ext, n]
	rng->generate_sobol_cpu(stock_rn, 1, path_ext);
	rng->convert_normal(stock_rn, path_ext);

	// Basket Option
	// [path_ext, [path_int, n]] => [path_ext * path_int, n]
	int bsk_n = bskop->get_n();
	float* bskop_tmp_rn = (float*)malloc((size_t)path_ext * path_int * bsk_n * sizeof(float));
	rng->generate_sobol_cpu(bskop_tmp_rn, bsk_n, path_ext * path_int);
	cout << "Random Numbers:" << endl;
	for (int i = 0; i < path_ext * path_int; i++) {
		for (int j = 0; j < bsk_n; j++) {
			cout << bskop_tmp_rn[i * bsk_n + j] << " ";
		}
		cout << endl;
	}

	rng->convert_normal(bskop_tmp_rn, path_ext * path_int * bsk_n);

	cout << "Random Numbers:" << endl;
	for (int i = 0; i < path_ext* path_int; i++) {
		for (int j = 0; j < bsk_n; j++) {
			cout << bskop_tmp_rn[i * bsk_n + j] << " ";
		}
		cout << endl;
	}

	cout << "A:" << endl;
	for (int i = 0; i < bsk_n; i++) {
		for (int j = 0; j < bsk_n; j++) {
			cout << bskop->get_A()[i * bsk_n + j] << " ";
		}
		cout << endl;
	}

	// A[n * n]*rn[n * (path_ext * path_int)]
	
	cblas_sgemm(CblasRowMajor,
		CblasNoTrans,
		CblasTrans,
		bsk_n,								// result row
		path_ext * path_int,				// result col
		bsk_n,								// length of "multiple by"
		1,									// alpha
		bskop->get_A(),						// A
		bsk_n,								// col of A
		bskop_tmp_rn,						// B
		bsk_n,								// col of B
		0,									// beta
		bskop_rn,							// C
		path_ext * path_int					// col of C
	);

	free(bskop_tmp_rn);

	cout << "Random Numbers:" << endl;
	for (int i = 0; i < bsk_n; i++) {
		for (int j = 0; j < path_ext * path_int; j++) {
			cout << bskop_rn[i * path_ext * path_int + j] << " ";
		}
		cout << endl;
	}

	// ===================================================================
	// Outter Monte Carlo Simulation
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
	row_idx++;

	// Basket Options
	for (int i = 0; i < path_ext; i++) {
		// Store to the next row of price matrix
		prices[row_idx * path_ext + i] = bskop->price_basket_option(path_int, i);
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
	
	// ===================================================================
	// Loss
	// Todo: change 1 to e^rT
	// [Test]can e^rT be ignored here to reduce calculation?
	// it doesn't affect the sorting result
	// 
	// [path_ext*n]*[n * 1]
	// Loss = price+(-p0)
	cout << endl << "Today Price:" << endl;
	cout << port_p0 << endl;
	cblas_sgemv(CblasRowMajor, CblasTrans, port_n, path_ext, -1, prices, 
		path_ext, port_w, 1, 0, loss, 1);

	cout << endl << "Loss:" << endl;
	for (int i = 0; i < path_ext; i++) {
		cout << loss[i] << " ";
	}
	cout << endl;

	// ===================================================================
	// Sort
	std::sort(loss, loss + path_ext);

	cout << endl << "Sorted Loss:" << endl;
	for (int i = 0; i < path_ext; i++) {
		std::cout << loss[i] << " ";
	}
	cout << endl;

	// ===================================================================
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
