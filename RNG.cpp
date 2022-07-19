#include "RNG.h"

int RNG::generate_sobol_cpu(float*& data, int m, int n, int seed, int offset) {
    /* Create pseudo-random number generator */
    curandGenerator_t gen;
    CURAND_CALL(curandCreateGeneratorHost(&gen,
        CURAND_RNG_QUASI_SOBOL32));

    /* Set offset and seed */
    //CURAND_CALL(curandSetPseudoRandomGeneratorSeed(gen, offset));
    CURAND_CALL(curandSetGeneratorOffset(gen, offset));

    /* Set dimention */
    CURAND_CALL(curandSetQuasiRandomGeneratorDimensions(gen, n));

    /* Generate m floats on device */
    CURAND_CALL(curandGenerateUniform(gen, data, m * n));

    /* Cleanup */
    CURAND_CALL(curandDestroyGenerator(gen));
}

int RNG::convert_normal(float*& data, int length, float sigma) {
    for (int i = 0; i < length; i++) {
        data[i] = normcdfinvf(data[i]) * sigma;
    }
    
    return 0;
}