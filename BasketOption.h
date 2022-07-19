#pragma once
#ifndef BASKET_OPTION
#define BASKET_OPTION

#include "Stock.h"
#include <mkl.h>
#include <curand_kernel.h>

#include<iostream>

using namespace std;

class BasketOption {
public:
	//BasketOption() {
	//	this->n = 4;
	//	this->stocks = NULL;
	//	this->k = 0;
	//	this->w = 0;

	//	rn = NULL;

	//	float cov[16] = { 18, 22, 54, 42,22, 70, 86, 62,
	//						54, 86, 174, 134, 42, 62, 134, 106 };

	//	for (int i = 0; i < 4; i++) {
	//		for (int j = 0; j < 4; j++) {
	//			cout << cov[i * 4 + j] << " ";
	//		}
	//		cout << endl;
	//	}

	//	//A = (float*)malloc((size_t)n * n * sizeof(float));

	//	LAPACKE_spotrf(LAPACK_ROW_MAJOR, 'U', n, cov, n);

	//	A = (float*)malloc((size_t)n * n * sizeof(float));
	//	// Copy the cholesky result to A
	//	for (int i = 0; i < n; i++) {
	//		for (int j = 0; j < n; j++) {
	//			A[i * 4 + j] = (i > j) ? 0 : cov[i * n + j];
	//		}
	//	}

	//	for (int i = 0; i < 4; i++) {
	//		for (int j = 0; j < 4; j++) {
	//			cout << A[i * 4 + j] << " ";
	//		}
	//		cout << endl;
	//	}

	//	return;
	//}

	BasketOption(float*& data, int n, Stock* stocks, float* cov, float k, float* w) {
		this->n = n;
		this->stocks = stocks;
		this->k = k;
		this->w = w;

		rn = data;

		//// Cholesky decompose by lapack
		// The result will be storeed in cov
		// However the lower part won't be set to 0 automatically
		LAPACKE_spotrf(LAPACK_ROW_MAJOR, 'U', n, cov, n);

		// Copy the cholesky result to A
		A = (float*)malloc((size_t)n * n * sizeof(float));
		for (int i = 0; i < n; i++) {
			for (int j = 0; j < n; j++) {
				A[i * 4 + j] = (i > j) ? 0 : cov[i * n + j];
			}
		}

		return;
	}

	~BasketOption() {
		free(A);
	}

	float price_basket_option(int path_int, int offset) {
		return 0;
	}
private:
	int n;				// Number of stocks in the basket
	Stock* stocks;		// Underlying stocks
	float k;			// Option execute price(strike)
	float* w;			// Weight of each stock

public:
	float* A;			// Cholesky decomposistion of the covariance matrix

	float* rn;			// Random number list

};


#endif // !BASKET_OPTION

