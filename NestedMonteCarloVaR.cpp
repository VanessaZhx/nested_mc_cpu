#include "NestedMonteCarloVaR.h"

NestedMonteCarloVaR::NestedMonteCarloVaR(int pext, int pint, 
	int t, float per, int port_n, float* weight, float risk_free) {
	this->path_ext = pext;
	this->path_int = pint;
	this->var_t = t;
	this->var_per = per;
	this->port_n = port_n;
	this->port_w = weight;
	this->risk_free = risk_free;

	prices = (float*)malloc((size_t)path_ext * port_n * sizeof(float));
}

NestedMonteCarloVaR::~NestedMonteCarloVaR() {
	free(prices);
	free(bond_rn);
	free(stock_rn);
	free(bskop_rn);
}

void NestedMonteCarloVaR::bond_init(float bond_par, float bond_c, int bond_m, 
	float* bond_y,float sig, int idx) {
	// Memory allocation
	bond_rn = (float*)malloc((size_t)path_ext * sizeof(float));

	// Product initiation
	bond = new Bond(bond_rn, bond_par, bond_c, bond_m, bond_y, sig);

	// add start price to the portfolio start price
	// Start price for bond is priced with original yield curve
	this->port_p0 += bond->price_bond_with_delta_yield(0) * port_w[idx];
}

void NestedMonteCarloVaR::stock_init(float stock_s0, float stock_mu, 
	float stock_var, int stock_x, int idx) {
	// Memory allocation
	stock_rn = (float*)malloc((size_t)path_ext * sizeof(float));

	// Product initiation
	stock = new Stock(stock_rn, stock_s0, stock_mu, stock_var, stock_x);

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
	// {S0 - K, 0}, K should be lagerer than S0, so start price would be 0 
	float stock_p = 0.0f;
	for (int i = 0; i < bskop_n; i++) {
		stock_p += bskop_stocks[i].get_s0() * bskop_stocks[i].get_x()
			* bskop_w[i];
	}
	this->port_p0 += (exp(risk_free * var_t) - 1) * stock_p  * port_w[idx];
	//this->port_p0 += 0;
}

int NestedMonteCarloVaR::execute() {
	// TODO: CLEAN CLASS DEFINATION && REDUCE FUNCTION CALL!!!
	// ===================================================================
	// Random number preperation
	RNG* rng = new RNG;
	
	// Todo: fix RN seed
	// 
	// == BOND ==
	// RN is used to move yield curve up/down, N~(0, sigma^2)
	// [path_ext, 1]
	rng->generate_sobol_cpu(bond_rn, 1, path_ext);
	rng->convert_normal(bond_rn, path_ext, bond->get_sigma());

	// == STOCK ==
	// RN is used as the external path
	// For stock pricing only, there's no need to generate the inner path now
	// [path_ext, var_t]
	rng->generate_sobol_cpu(stock_rn, var_t, path_ext);
	rng->convert_normal(stock_rn, path_ext);

	cout << "Random Numbers:" << endl;
	for (int i = 0; i < path_ext; i++) {
		cout << bond_rn[i] << " ";
	}
	cout << endl;

	// Basket Option
	// [path_ext, [path_int, n]] => [path_ext * path_int, n]
	int bsk_n = bskop->get_n();
	float* bskop_tmp_rn = (float*)malloc((size_t)path_ext * path_int * bsk_n * sizeof(float));
	rng->generate_sobol_cpu(bskop_tmp_rn, bsk_n, path_ext * path_int);

	/*cout << "Random Numbers:" << endl;
	for (int i = 0; i < path_ext * path_int; i++) {
		for (int j = 0; j < bsk_n; j++) {
			cout << bskop_tmp_rn[i * bsk_n + j] << " ";
		}
		cout << endl;
	}*/

	rng->convert_normal(bskop_tmp_rn, path_ext * path_int * bsk_n);

	/*cout << "Random Numbers:" << endl;
	for (int i = 0; i < path_ext* path_int; i++) {
		for (int j = 0; j < bsk_n; j++) {
			cout << bskop_tmp_rn[i * bsk_n + j] << " ";
		}
		cout << endl;
	}*/

	/*cout << "A:" << endl;
	for (int i = 0; i < bsk_n; i++) {
		for (int j = 0; j < bsk_n; j++) {
			cout << bskop->get_A()[i * bsk_n + j] << " ";
		}
		cout << endl;
	}*/

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

	/*cout << "Random Numbers:" << endl;
	for (int i = 0; i < bsk_n; i++) {
		for (int j = 0; j < path_ext * path_int; j++) {
			cout << bskop_rn[i * path_ext * path_int + j] << " ";
		}
		cout << endl;
	}*/

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
		prices[row_idx * path_ext + i] = stock->price_stock(i, var_t);
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
	cout << endl << "Today Price:" << endl;
	cout << port_p0 << endl;

	// Fill loss with negtive today's price of the portfolio
	float* loss = (float*)malloc((size_t)path_ext * sizeof(float));
	for (int i = 0; i < path_ext; i++) {
		*(loss + i) = port_p0;
	}

	// prices[port_n][path_ext]
	// prices[path_ext * port_n] * w[port_n * 1]
	// Loss = -(p-p0) = p0-p
	// Loss = p0 - e^-rT * (price*w) = loss + (- e^-rT) *(price*w)
	// haven't get ln here, but doesn't affect sorting
	cblas_sgemv(CblasRowMajor,			// Specifies row-major
		CblasTrans,						// Specifies whether to transpose matrix A.
		port_n,							// A rows
		path_ext,						// A col
		-exp(-1 * risk_free* var_t),	// alpha
		prices,							// A
		path_ext,						// The size of the first dimension of matrix A.
		port_w,							// Vector X.
		1,								// Stride within X. 
		1,								// beta
		loss,							// Vector Y
		1);								// Stride within Y
					
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
