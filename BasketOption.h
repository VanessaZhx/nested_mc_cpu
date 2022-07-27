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
		for (int i = 0; i < n; i++) {
			for (int j = 0; j < n; j++) {
				cout << cov[i * n + j] << " ";
			}
			cout << endl;
		}

		// Copy the cholesky result to A
		A = (float*)malloc((size_t)n * n * sizeof(float));
		for (int i = 0; i < n; i++) {
			for (int j = 0; j < n; j++) {
				A[i * n + j] = (i < j) ? 0 : cov[i * n + j];
			}
		}
		/*for (int i = 0; i < n; i++) {
			for (int j = 0; j < n; j++) {
				cout << A[i * n + j] << " ";
			}
			cout << endl;
		}*/

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
		// random numbers[n][path_int]
		for (int i = 0; i < n; i++) {
			for (int j = 0; j < path_int; j++) {
				int rn_offset = offset * path_int * n + i * path_int + j;
				value_each[i * path_int + j] = stocks[i].price_single_stock_with_z(rn[rn_offset]);
				//cout << value_each[i * path_int + j] << ' ';
			}
			//cout << endl;
		}

		// Multiply with weight
		// Value_each[n][path_int]
		// value_each[inter * n] * weight[n * 1]
		float va[2] = { 100, 200 };
		float temp[2] = { 0, 1 };
		cblas_sgemv(CblasRowMajor,		// Specifies row-major
			CblasTrans,					// Specifies whether to transpose matrix A.
			n,					// A rows
			path_int,							// A col
			1,							// alpha	
			value_each,					// A
			path_int,					// The size of the first dimension of matrix A.
			w,							// Vector X.
			1,						// Stride within X. 
			0,						// beta
			value_weighted,			// Vector Y
			1						// Stride within Y
		);
		
		/*cout << "vw:" << endl;
		for (int i = 0; i < path_int; i++) {
			cout << value_weighted[i] << ' ';
		}
		cout << endl;*/

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

