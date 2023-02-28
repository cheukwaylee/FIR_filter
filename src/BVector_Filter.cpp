#include "BVector_Filter.h"

void Top(data_c *tb_in, data_c *tb_out, data_d *coeff, int N)
{
#pragma HLS INTERFACE axis port = tb_in depth = 1966080
#pragma HLS INTERFACE axis port = tb_out depth = 7864320
#pragma HLS INTERFACE m_axi port = coeff depth = 257 max_read_burst_length = 256

    static data_c zeros[NUM_COLUM] = {0};

    static data_d B[NUM_COEFF];
#pragma HLS ARRAY_PARTITION variable = B type = complete dim = 1
    for (int i = 0; i < NUM_COEFF; i++)
    {
#pragma HLS pipeline II = 1
        B[i] = coeff[i]; // read the coefficient as local variable
    }

    data_c X[NUM_COLUM];
    data_c Y[NUM_COLUM];
#pragma HLS ARRAY_PARTITION variable = X type = complete
#pragma HLS ARRAY_PARTITION variable = Y type = complete

    for (int i = 0; i < N; i++) // N=61k, the number of row
    {
#pragma HLS LOOP_TRIPCOUNT min = 61440 max = 61440 avg = 61440
        // read in
        for (int j = 0; j < NUM_COLUM; j++)
        {
#pragma HLS PIPELINE II = 1 rewind
            X[j] = tb_in[j + i * NUM_COLUM]; // take all elements from each row of the tb_in
        }

    loopCompute:
        for (int k = 0; k < 4; k++) //  mix upsample and normal fir
        {
            if (k == 0)
                Fir(X, Y, B); //  normal fir
            else
                Fir(zeros, Y, B); //  upsample: expands the vector with 0

            // write out
            for (int j = 0; j < NUM_COLUM; j++)
            {
#pragma HLS pipeline II = 1 rewind
                tb_out[j + k * NUM_COLUM + i * NUM_COLUM * 4] = Y[j];
            }
        }
    }
}

void Fir(data_c X[NUM_COLUM], data_c Y[NUM_COLUM], data_d B[NUM_COEFF])
{
#pragma HLS ARRAY_PARTITION variable = X type = complete dim = 1
#pragma HLS ARRAY_PARTITION variable = Y type = complete dim = 1

    // history of input X
    static int oldest_Z_idx = 0;
    static data_c Z[NUM_COLUM][NUM_COEFF - 1]; // 32-by-256
#pragma HLS ARRAY_PARTITION variable = Z type = complete dim = 1
#if Z_DIM2_CYCLIC_ENABLE == 1
#pragma HLS ARRAY_PARTITION variable = Z type = cyclic factor = MAC_unroll_factor dim = 2
#endif

    data_c part_sum[NUM_COLUM];
#pragma HLS ARRAY_PARTITION variable = part_sum type = complete

VEC_INIT:
    for (int j = 0; j < NUM_COLUM; j++)
    {
#pragma HLS UNROLL factor = 32 // max=32
        part_sum[j].real(0.0);
        part_sum[j].imag(0.0);

        // Y[j] = X[j] * B[0];
        Y[j].real(X[j].real() * B[0]);
        Y[j].imag(X[j].imag() * B[0]);
    }

MAC: // multi-accumulator
    for (int i = NUM_COEFF - 1; i != 0; i--)
    {
#pragma HLS pipeline II = 1
#pragma HLS unroll factor = MAC_unroll_factor // max=256

        // i (old->new): 256: -1: 1
        // i_complement: 0  :  1: 255
        int i_complement = (NUM_COEFF - 1) - i;

    VEC_MAC:
        for (int j = 0; j < NUM_COLUM; j++)
        {
            int current_Z_idx = oldest_Z_idx + i_complement;
            current_Z_idx %= (NUM_COEFF - 1);

            // Y[j] += Z[j][i - 1] * B[i];
            part_sum[j].real(part_sum[j].real() + Z[j][current_Z_idx].real() * B[i]);
            part_sum[j].imag(part_sum[j].imag() + Z[j][current_Z_idx].imag() * B[i]);

            if ((i % MAC_unroll_factor == 1) || (MAC_unroll_factor == 1))
            {
                Y[j].real(Y[j].real() + part_sum[j].real());
                Y[j].imag(Y[j].imag() + part_sum[j].imag());
                part_sum[j].real(0.0);
                part_sum[j].imag(0.0);
            }
        }
    }

VEC_SHIFT_INIT:
    for (int j = 0; j < NUM_COLUM; j++)
    {
#pragma HLS UNROLL factor = 32 // max=32
        Z[j][oldest_Z_idx] = X[j];
    }
    oldest_Z_idx++;
    oldest_Z_idx %= (NUM_COEFF - 1);
}
