#ifndef __BVECTOR_FILTER_H__
#define __BVECTOR_FILTER_H__

#include <iostream>
#include <stdio.h>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <complex>
#include "math.h"
#include <cstdlib>
#include <string.h>

using namespace std;

//the input matrix is the initial matrix has been tranposed
#define NUM_ROW  61440 //6
#define NUM_COLUM  32  //3
#define NUM_COEFF  257  //5
#define ERR 0.000001

//used to assign the unroll and cyclic factor for pragma
#ifndef MAC_unroll_factor
#define MAC_unroll_factor 1
#endif

#ifndef Z_cyclic_factor
#define Z_cyclic_factor 1
#endif

#ifndef SHIFT_unroll_factor
#define Z_SHIFT_unroll_factor 1
#endif

typedef complex<double>   data_c;
//typedef double           data_c;
typedef double            data_d;


// Top function

void Top(data_c *tb_in, data_c *tb_out, data_d *coeff, int N);

//Sub Function
void Fir(data_c X[NUM_COLUM], data_c Y[NUM_COLUM], data_d B[NUM_COEFF]);

 #endif
