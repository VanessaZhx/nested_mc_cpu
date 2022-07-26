﻿#include "NestedMonteCarloVaR.h"


#if defined(WIN64)
#include <intrin.h>
#endif

void getInfo();

int main(int argc, char* argv[])
{
	int exp_times = 0;   // Total times of MC

	int path_ext = 10;  // Number of the outer MC loops
	int path_int = 10;  // Number of the inner MC loops

	// input: path_ext, path_int, exp_times
	/*if (argc >= 2)  path_ext = atoi(argv[1]);
	if (argc >= 3)  path_int = atoi(argv[2]);
	if (argc >= 4)  exp_times = atoi(argv[3]);*/

	int cnt = 0;

	for (int i = 1; i < argc; i++) {
		char* pchar = argv[i];
		switch (pchar[0]) {			// Decide option type
		case '-': {					// For option
			switch (pchar[1]) {
			default:		// unrecognisable - show usage
				cout << endl << "===================== USAGE =====================" << endl;
				cout << "Enter up to 3 numbers for [path_ext, path_int, exp_times]" << endl;
				cout << "Default setup: [" << path_ext << ", " << path_int << ", "
					<< exp_times << "]" << endl;
				return 0;
			}
			break;
		}
		default:				// For numbers(not concerning rubust so
								// char will be treated as numbers)
			switch (cnt) {
			case 0:
				path_ext = atoi(argv[i]);
				break;
			case 1:
				path_int = atoi(argv[i]);
				break;
			case 2:
				exp_times = atoi(argv[i]);
				break;
			default:
				break;
			}
			cnt++;
		}
	}

	cout << endl << "== SET UP ==" << endl;
	cout << "Experiment Times: " << exp_times << endl;
	cout << "Path External: " << path_ext << endl;
	cout << "Path Internal: " << path_int << endl;

	cout << endl << "== DEVICE ==" << endl;
	getInfo();

	const int var_t = 1;					// VaR duration
	const float var_per = 0.95f;				// 1-percentile

	const int port_n = 4;					// Number of products in the portfolio
	float port_w[port_n] = { 0.2f, 0.1f, 0.35f, 0.35f };		// Weights of the products in the portfolio
														// { bond, stock, basket option, barrier option}
	//float port_w[port_n] = { 0.0f, 0.0f, 0.0f, 1.0f };
	const float risk_free = 0.01f;

	const float bond_par = 1000.0f;			// Par value of bond
	const float bond_c = 100.0f;			// Coupon
	const int bond_m = 10;					// Maturity
	float bond_y[bond_m] = { 
			5.00f, 5.69f, 6.09f, 6.38f, 6.61f,
			6.79f, 6.94f, 7.07f, 7.19f, 7.30f 
	};										// yeild curve
	const float sigma = 1.5f;				// sigma

	const float stock_s0 = 300.0f;			// Start value of stock
	const float stock_mu = risk_free;			// risk free(or mean)
	const float stock_var = 0.03f;			// Volatility
	const int stock_x = 1;					// Number of shares

	Stock* s0 = new Stock(stock_s0, stock_mu, stock_var, 10);
	Stock* s1 = new Stock(500.0f, risk_free, 0.02f, 10);
	Stock* s2 = new Stock(700.0f, risk_free, 0.01f, 10);
	const int bskop_n = 2;								// Number of stocks in basket
	const float bskop_k = 390.0f;						// Execution price
	const int bskop_t = 1;								// Maturity of basket option
	Stock bskop_stocks[bskop_n] = { *s1, *s2 };			// List of stocks
	float bskop_cov[bskop_n * bskop_n] = { 1.0f, 0.5f,	
										   0.5f, 1.0f };	// Covariance matrix
	float bskop_w[bskop_n] = { 0.8f, 0.2f };				// weight

	const float barop_k = 310.0f;				// Execution price
	const float barop_h = 320.0f;				// Barrier
	const int barop_t = 30;						// Maturity(steps of inner path)
		
	NestedMonteCarloVaR* mc = new NestedMonteCarloVaR(
		path_ext,path_int,
		var_t, var_per,
		port_n, port_w,
		risk_free
	);
	mc->bond_init(bond_par, bond_c, bond_m, bond_y, sigma, 0);
	mc->stock_init(stock_s0, stock_mu, stock_var, stock_x, 1);
	mc->bskop_init(bskop_n, bskop_stocks, bskop_cov, bskop_k, bskop_w, bskop_t, 2);
	mc->barop_int(s0, barop_k, barop_h, barop_t, 3);

	cout << endl << "== EXECUTION ==" << endl;

	// Warm up
	mc->execute();

	double exe_time = 0.0;
	for (int i = 0; i < exp_times; i++) {
		exe_time += mc->execute();
		if ((i == 0) || (i + 1 == exp_times)
			|| (exp_times <= 10)
			|| (exp_times > 10 && i % (exp_times / 10) == 0)) {
			cout << "Experiment # " << i << " finished." << endl;
		}
	}
	cout << "AVG EXECUTION TIME: " << exe_time / exp_times << " ms." << endl;
    return 0;
}

#if defined(WIN64)
void getInfo() {
	int cpuInfo[4] = { -1 };
	char cpu_manufacture[32] = { 0 };
	char cpu_type[32] = { 0 };
	char cpu_freq[32] = { 0 };

	__cpuid(cpuInfo, 0x80000002);
	memcpy(cpu_manufacture, cpuInfo, sizeof(cpuInfo));

	__cpuid(cpuInfo, 0x80000003);
	memcpy(cpu_type, cpuInfo, sizeof(cpuInfo));

	__cpuid(cpuInfo, 0x80000004);
	memcpy(cpu_freq, cpuInfo, sizeof(cpuInfo));

	cout << "CPU manufacture: " << cpu_manufacture << std::endl;
	cout << "CPU type: " << cpu_type << std::endl;
	cout << "CPU main frequency: " << cpu_freq << std::endl;
}
#elif defined(__linux__)
void getInfo()
{
	FILE* fp = fopen("/proc/version", "r");
	if (NULL == fp)
		printf("failed to open version\n");
	char szTest[1000] = { 0 };
	while (!feof(fp))
	{
		memset(szTest, 0, sizeof(szTest));
		fgets(szTest, sizeof(szTest) - 1, fp); // leave out \n  
		printf("%s", szTest);
	}
	fclose(fp);

	fp = fopen("/proc/cpuinfo", "r");
    if (NULL == fp)
		printf("failed to open cpuinfo\n");
	//char szTest[1000] = { 0 };
	// read file line by line   
	while (!feof(fp))
	{
		memset(szTest, 0, sizeof(szTest));
		fgets(szTest, sizeof(szTest) - 1, fp); // leave out \n  
		printf("%s", szTest);
	}
	fclose(fp);

	//fp = fopen("/proc/meminfo", "r");
	//if (NULL == fp)
	//	printf("failed to open meminfo\n");
	//char szTest[1000] = { 0 };
	//while (!feof(fp))
	//{
	//	memset(szTest, 0, sizeof(szTest));
	//	fgets(szTest, sizeof(szTest) - 1, fp);
	//	printf("%s", szTest);
	//}
	//fclose(fp);
}
#endif

