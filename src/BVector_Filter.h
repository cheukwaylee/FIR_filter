#ifndef __BVECTOR_FILTER_H__
#define __BVECTOR_FILTER_H__

#include <stdio.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <complex>
#include <vector>
#include <math.h>
#include <cstdlib>

using namespace std;

// the input matrix is the initial matrix has been transposed
#define NUM_ROW 61440
#define NUM_COLUM 32
#define NUM_COEFF 257
#define ERR 0.000001

// HLS pragma DSE
#ifndef MAC_UNROLL_FACTOR
#define MAC_UNROLL_FACTOR 16 // 1, 2, 4, 8, 16, 32, 64, 128, 256
#endif

#ifndef Z_DIM2_CYCLIC_ENABLE
#define Z_DIM2_CYCLIC_ENABLE 1 // 0, 1
#endif

typedef complex<double> data_c;
typedef double data_d;

// Top function
void Top(data_c *tb_in, data_c *tb_out, data_d *coeff, int N);

// Sub Function
void Fir(data_c X[NUM_COLUM], data_c Y[NUM_COLUM], data_d B[NUM_COEFF]);

#endif
