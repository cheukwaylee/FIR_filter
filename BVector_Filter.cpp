#include "BVector_Filter.h"

void Top(data_c *tb_in, data_c *tb_out, data_d *coeff, int N)
{

#pragma HLS INTERFACE axis port=tb_in depth=1966080
#pragma HLS INTERFACE axis port=tb_out depth=7864320
#pragma HLS INTERFACE m_axi port=coeff depth=257  max_read_burst_length=256
//#pragma HLS INTERFACE mode=s_axilite port=return


    static data_c zeros[NUM_COLUM];
    static data_d B[NUM_COEFF];
#pragma HLS ARRAY_PARTITION variable=B type=complete dim=1

    for(int i = 0; i< NUM_COEFF; i++)
    {
#pragma  HLS pipeline II = 1
        B[i] = coeff[i];  // read the coefficient as local variable
    }
//  memcpy(B, coeff, sizeof(data_d)*NUM_COEFF);

    data_c X[NUM_COLUM];
    data_c Y[NUM_COLUM];
#pragma HLS ARRAY_PARTITION variable=X type=complete
#pragma HLS ARRAY_PARTITION variable=Y type=complete


    for (int i = 0; i < N; i++) // N is the number of row. so the operation focus on every row
//    for(int i= 0; i <61440;i++)
    {
#pragma HLS LOOP_TRIPCOUNT  min=61440 max=61440 avg=61440


    	// read in
        for (int j = 0; j < NUM_COLUM; j++)
        {
#pragma HLS PIPELINE II=1 rewind
            X[j] = tb_in[j + i * NUM_COLUM]; // take all elements from each row of the tb_in
        }

        // computation
        loopCompute:
		for (int k = 0; k < 4; k++) //  mix upsample and normal fir
        {
            if(k == 0)
                Fir(X, Y, B); //  normal fir
            else 
                Fir(zeros, Y, B);  //  upsample: expands the vector with 0
            
            // write out
            for ( int j = 0; j< NUM_COLUM; j++)
            {
#pragma  HLS pipeline II = 1 rewind
                tb_out[j + k *NUM_COLUM + i * NUM_COLUM * 4] = Y[j]; //  TO CONFIRM AGAIN
            }
        }
    }
}



void Fir(data_c X[NUM_COLUM], data_c Y[NUM_COLUM], data_d B[NUM_COEFF])
{
	const int unroll_factor = 16;

#pragma HLS ARRAY_PARTITION variable=X type=complete dim=1
#pragma HLS ARRAY_PARTITION variable=Y type=complete dim=1
//#pragma HLS ARRAY_PARTITION variable=B type=complete dim=1


	static data_c Z[NUM_COLUM][NUM_COEFF-1];  // NUM_COLUMN= 32
//#pragma HLS ARRAY_RESHAPE variable=Z type=complete dim=1

#pragma HLS ARRAY_PARTITION variable=Z type=complete dim=1
//#pragma HLS ARRAY_PARTITION variable=Z type=cyclic factor=32 dim=2
//#pragma HLS ARRAY_PARTITION variable=Z type=complete  dim=2


	data_c part_sum[NUM_COLUM] ;
#pragma HLS ARRAY_PARTITION variable=part_sum type=complete

//	data_c Z_buffer[NUM_COLUM * unroll_factor]; //used to store the part of Z for satisfing the unroll
//#pragma HLS ARRAY_PARTITION variable=Z_buffer type=complete

    VEC_INIT:
    for( int j = 0; j < NUM_COLUM; j++)
    {
#pragma HLS PIPELINE II=1 rewind
        part_sum[j].real(0.0);
        part_sum[j].imag(0.0);

        // Y[j] = X[j] * B[0];
        Y[j].real(X[j].real() * B[0]);
        Y[j].imag(X[j].imag() * B[0]);
    }

    MAC:  // multi-accumulator
    for (int i = NUM_COEFF-1; i != 0; i--)
    {
#pragma HLS pipeline II=4
#pragma HLS unroll factor=unroll_factor
        VEC_MAC:
        for (int j = 0; j < NUM_COLUM; j++)
        {
			// Y[j] += Z[j][i - 1] * B[i];
			part_sum[j].real(part_sum[j].real() + Z[j][i - 1].real() * B[i]);
			part_sum[j].imag(part_sum[j].imag() + Z[j][i - 1].imag() * B[i]);

			if(i % unroll_factor == 1)  // i= 4,3,2,1 -> i%4= 0,3,2,1; i%2= 0,1,0,1
            {
                Y[j].real(Y[j].real() + part_sum[j].real());
                Y[j].imag(Y[j].imag() + part_sum[j].imag());
                part_sum[j].real(0.0);
                part_sum[j].imag(0.0);
            }
        }
    }


    SHIFT:
    for ( int i = NUM_COEFF - 2; i != 0; i--)
    {
#pragma HLS PIPELINE II=1
//#pragma HLS UNROLL factor=32
#pragma HLS DEPENDENCE variable=Z inter false
    	VEC_SHIFT:
        for ( int j = 0; j < NUM_COLUM; j++)
        {
        	Z[j][i] = Z[j][i - 1];
        }
    }

    VEC_SHIFT_INIT:
    for (int j = 0; j < NUM_COLUM; j++)
    {
#pragma HLS PIPELINE II=1 rewind
//#pragma HLS DEPENDENCE variable=Z inter false
        Z[j][0] = X[j];
//    	  Z[j][0] = X_buffer[j];
    }

}


