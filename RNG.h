#pragma once
#ifndef RNG_H
#define RNG_H

#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include <curand_kernel.h>
#include <curand.h>

#include "cuda_helper.cuh"


class RNG{
public:
    // m - simulation times
    // n - dimention
    int generate_sobol_cpu(float*& data, int m, int n, int seed = 1024, int offset = 1024);
    int convert_normal(float*& data, int length, float sigma);
};

#endif

