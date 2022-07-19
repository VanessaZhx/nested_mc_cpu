#pragma once
#ifndef BASKET_OPTION
#define BASKET_OPTION

#include "Stock.h"
#include <mkl.h>
#include<math.h>
#include <curand_kernel.h>

#include<iostream>

using namespace std;

class BasketOption {
public:

	BasketOption(float*& data, int n, Stock* stocks, float* cov, float k, float* w) {
		this->n = n;
		this->stocks = stocks;
		this->k = k;
		this->w = w;

		rn = data;

		//// Cholesky decompose by lapack
		// The result will be storeed in cov
		// However the upper part won't be set to 0 automatically
		LAPACKE_spotrf(LAPACK_ROW_MAJOR, 'L', n, cov, n);

		// Copy the cholesky result to A
		A = (float*)malloc((size_t)n * n * sizeof(float));
		for (int i = 0; i < n; i++) {
			for (int j = 0; j < n; j++) {
				A[i * 4 + j] = (i < j) ? 0 : cov[i * n + j];
			}
		}

		return;
	}

	~BasketOption() {
		free(A);
	}

	float* get_A() { return A; }
	int get_n() { return n; }

	float price_basket_option(int path_int, int offset) {
		float *value_each = (float*)malloc((size_t)path_int * n * sizeof(float));
		float *value_weighted = (float*)malloc((size_t)path_int* sizeof(float));

		// The random number has already been transformed with cov
		// Calculate each value with the correlated-random numbers
		for (int i = 0; i < path_int; i++) {
			for (int j = 0; j < n; j++) {
				int rn_offset = offset * path_int * n + i * n + j;
				value_each[i * n + j] = stocks[j].price_single_stock_with_z(rn[rn_offset]);
			}
		}

		// Multiply with weight
		cblas_sgemv(CblasRowMajor, CblasTrans, 
			n, path_int, 1, value_each, path_int, w, 1, 1, value_weighted, 1);

		// Determine the price with stirke
		// If the price is less than k, don't execute
		float call = 0.0f;
		for (int i = 0; i < path_int; i++) {
			call += (value_weighted[i] > k) ? (value_weighted[i] - k) : 0;
		}

		// Return mean value
		call /= n;

		free(value_each);
		free(value_weighted);

		return call;
	}
private:
	int n;				// Number of stocks in the basket
	Stock* stocks;		// Underlying stocks
	float k;			// Option execute price(strike)
	float* w;			// Weight of each stock

	float* A;			// Cholesky decomposistion of the covariance matrix
	float* rn;			// Random number list

};


#endif // !BASKET_OPTION

