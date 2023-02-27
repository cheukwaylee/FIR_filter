#include "BVector_Filter.h"

data_c in_data[NUM_ROW][NUM_COLUM];
data_c out_data[NUM_ROW * 4][NUM_COLUM];
data_d in_coeff[NUM_COEFF];
data_c mat_out[NUM_ROW * 4][NUM_COLUM];
int Num = NUM_ROW;

// used to store data from matlab file to compare
data_d data_in_real[NUM_ROW][NUM_COLUM];
data_d data_in_imag[NUM_ROW][NUM_COLUM];
data_d data_out_real[NUM_ROW * 4][NUM_COLUM];
data_d data_out_imag[NUM_ROW * 4][NUM_COLUM];

std::vector<data_d> readFile(const char *filename)
{
	// open the file:
	std::streampos fileSize;
	std::ifstream file(filename, std::ios::binary);

	// get its size:
	file.seekg(0, std::ios::end);
	fileSize = file.tellg();
	file.seekg(0, std::ios::beg);

	// read the data:
	std::vector<data_d> fileData(fileSize);
	file.read((char *)&fileData[0], fileSize);

	return fileData;
}

int main()
{
	// To gengrate the coeff
	int count = 1;
	for (int i = 0; i < NUM_COEFF; i++)
	{
		in_coeff[i] = count;
		count++;
	}

	// read input from file which is same as Matlab input
	std::vector<data_d> inputTR = readFile("../input_real.bin");
	std::vector<data_d> inputTI = readFile("../input_imag.bin");

	// Transpose the bin matrix
	for (int j = 0; j < NUM_COLUM; j++)
	{
		for (int i = 0; i < NUM_ROW; i++)
		{
			data_in_real[i][j] = inputTR[i + j * NUM_ROW];
			data_in_imag[i][j] = inputTI[i + j * NUM_ROW];
		}
	}

	// Merge the real and imag part
	for (int i = 0; i < NUM_ROW; i++)
	{
		for (int j = 0; j < NUM_COLUM; j++)
		{

			in_data[i][j].real(data_in_real[i][j]); // = RY[i][j];
			in_data[i][j].imag(data_in_imag[i][j]); //= IY[i][j];
		}
	}

	Top((data_c *)in_data, (data_c *)out_data, in_coeff, Num);

	// read output from  Matlab input
	std::vector<data_d> outputTR = readFile("../output_real.bin");
	std::vector<data_d> outputTI = readFile("../output_imag.bin");

	// Transpose the bin matrix
	for (int j = 0; j < NUM_COLUM; j++)
	{
		for (int i = 0; i < NUM_ROW * 4; i++)
		{

			data_out_real[i][j] = outputTR[i + j * NUM_ROW * 4];
			data_out_imag[i][j] = outputTI[i + j * NUM_ROW * 4];
		}
	}

	// Merge the real and imag part
	for (int i = 0; i < NUM_ROW * 4; i++)
	{
		for (int j = 0; j < NUM_COLUM; j++)
		{

			mat_out[i][j].real(data_out_real[i][j]);
			mat_out[i][j].imag(data_out_imag[i][j]);
		}
	}

	// to compare the output and calculatthe error percentage
	int match = 0;
	int mismatch = 0;
	data_d real_error;
	data_d imag_error;

	for (int i = 0; i < NUM_ROW * 4; i++)
	{
		for (int j = 0; j < NUM_COLUM; j++)
		{
			// for real part
			if (mat_out[i][j].real() == 0)
				real_error = fabs(out_data[i][j].real());
			else
				real_error = fabs((out_data[i][j].real() - mat_out[i][j].real()) / mat_out[i][j].real());

			// for imag part
			if (mat_out[i][j].imag() == 0)
				imag_error = fabs(out_data[i][j].imag());
			else
				imag_error = fabs((out_data[i][j].imag() - mat_out[i][j].imag()) / mat_out[i][j].imag());

			if ((real_error < ERR) && (imag_error < ERR))
				match++;
			else
				mismatch++;
		}
	}

	printf("OUTPUT match:%d \t mismatch:%d \t", match, mismatch);

	return 0;
}
