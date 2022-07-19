#pragma once
#ifndef BASKET_OPTION
#define BASKET_OPTION

#include "Stock.h"
#include <ccholesky.h>
using namespace splab;

class BasketOption {
public:
	BasketOption(float*& data, int n, Stock* stocks, float* cov, float k, float* w) {
		this->n = n;
		this->stocks = stocks;
		this->cov = cov;
		this->k = k;
		this->w = w;

		rn = data;

		// Cholesky
		Matrix<float> C(n, n, cov), ca(n, n);

		CCholesky<float> cho;
		cho.dec(C);
		if (!cho.isSpd())
			cout << "Factorization was not complete." << endl;
		else{
			ca = cho.getL();
			for (int i = 0; i < n; i++) {
				for (int j = 0; j < n; j++) {
					A[i * n + j] = ca[i][j];
				}
			}
		}
	}

	float price_basket_option(int path_int, int offset) {
		return 0;
	}
private:
	int n;				// Number of stocks in the basket
	Stock* stocks;		// Underlying stocks
	float* cov;			// Covariance matrix
	float k;			// Option execute price(strike)
	float* w;			// Weight of each stock

	float* A;			// Cholesky decomposistion

	float* rn;			// Random number list

};


#endif // !BASKET_OPTION

/*****************************************************************************
 *                             ccholesky_test.cpp
 *
 * Comnplex Cholesky class testing.
 *
 * Zhang Ming, 2010-12, Xi'an Jiaotong University.
 *****************************************************************************/


//#define BOUNDS_CHECK
//
//#include <iostream>
//#include <iomanip>
//#include <ccholesky.h>
//
//
//using namespace std;
//using namespace splab;
//
//
//typedef double  Type;
//const   int     N = 5;
//
//
//int main()
//{
//	cout << setiosflags(ios::fixed) << setprecision(3);
//	Matrix<complex<Type> > A(N, N), L(N, N);
//	Vector<complex<Type> > b(N);
//
//	for (int i = 1; i < N + 1; ++i)
//	{
//		for (int j = 1; j < N + 1; ++j)
//			if (i == j)
//				A(i, i) = complex<Type>(N + i, 0);
//			else
//				if (i < j)
//					A(i, j) = complex<Type>(Type(i), cos(Type(i)));
//				else
//					A(i, j) = complex<Type>(Type(j), -cos(Type(j)));
//
//		b(i) = i * (i + 1) / 2 + i * (N - i);
//	}
//
//	cout << "The original matrix A : " << A << endl;
//	CCholesky<complex<Type> > cho;
//	cho.dec(A);
//	if (!cho.isSpd())
//		cout << "Factorization was not complete." << endl;
//	else
//	{
//		L = cho.getL();
//		cout << "The lower triangular matrix L is : " << L << endl;
//		cout << "A - L*L^H is : " << A - L * trH(L) << endl;
//
//		Vector<complex<Type> > x = cho.solve(b);
//		cout << "The constant vector b : " << b << endl;
//		cout << "The solution of Ax = b : " << x << endl;
//		cout << "The solution of Ax - b : " << A * x - b << endl;
//
//		Matrix<complex<Type> > IA = cho.solve(eye(N, complex<Type>(1, 0)));
//		cout << "The inverse matrix of A : " << IA << endl;
//		cout << "The product of  A*inv(A) : " << A * IA << endl;
//	}
//
//	return 0;
//}



