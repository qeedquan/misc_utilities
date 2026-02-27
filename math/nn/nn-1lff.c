// based on https://www.cs.bham.ac.uk/~jxb/INC/nn.html
// single layer feed forward neural network with backpropagation

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define NUMPAT 4
#define NUMIN 2
#define NUMHID 2
#define NUMOUT 1

double
randu(void)
{
	double n, d;

	n = rand();
	d = RAND_MAX;
	return n / (d + 1);
}

double
rands(void)
{
	return 2 * (randu() - 0.5);
}

void
Dump(int Rows, int Cols, double Array[][Cols])
{
	int i, j;

	for (i = 0; i < Rows; i++) {
		for (j = 0; j < Cols; j++) {
			printf("%.6f ", Array[i][j]);
		}
		printf("\n");
	}
	printf("\n");
}

void
InitWeights(int Rows, int Cols, double Weights[][Cols + 1], double DeltaWeights[][Cols + 1])
{
	static const double smallwt = 0.5;
	int row, col;

	memset(Weights, 0, (Rows + 1) * (Cols + 1) * sizeof(double));
	memset(DeltaWeights, 0, (Rows + 1) * (Cols + 1) * sizeof(double));

	for (col = 1; col <= Cols; col++) {
		for (row = 0; row <= Rows; row++) {
			Weights[row][col] = rands() * smallwt;
			DeltaWeights[row][col] = 0.0;
		}
	}
}

void
Shuffle(int Size, int Pattern[Size + 1])
{
	int p, np, op;

	for (p = 1; p <= Size; p++) {
		Pattern[p] = p;
	}

	for (p = 1; p <= Size; p++) {
		np = p + randu() * (Size + 1 - p);
		op = Pattern[p];
		Pattern[p] = Pattern[np];
		Pattern[np] = op;
	}
}

void
Activate(int Index, int NumInput, int NumWeights,
         double Input[][NumInput + 1],
         double Weights[][NumWeights + 1],
         double Output[][NumWeights + 1])
{
	int i, j;
	double Sum;

	for (j = 1; j <= NumWeights; j++) {
		Sum = Weights[0][j];
		for (i = 1; i <= NumInput; i++) {
			Sum += Input[Index][i] * Weights[i][j];
		}
		Output[Index][j] = 1.0 / (1.0 + exp(-Sum));
	}
}

void
ComputeError(double *Error, int NumOutput, double Output[NumOutput + 1], double Target[NumOutput + 1], double Delta[NumOutput + 1])
{
	int i;

	for (i = 1; i <= NumOutput; i++) {
		*Error += 0.5 * (Target[i] - Output[i]) * (Target[i] - Output[i]);
		Delta[i] = (Target[i] - Output[i]) * Output[i] * (1.0 - Output[i]);
	}
}

void
BackPropagate(int NumHidden, int NumOutput,
              double Hidden[NumHidden + 1],
              double Weight[][NumOutput + 1],
              double DeltaH[NumHidden + 1],
              double DeltaO[NumOutput + 1])
{
	int i, j;
	double Sum;

	for (i = 1; i <= NumHidden; i++) {
		Sum = 0.0;
		for (j = 1; j <= NumOutput; j++) {
			Sum += Weight[i][j] * DeltaO[j];
		}
		DeltaH[i] = Sum * Hidden[i] * (1.0 - Hidden[i]);
	}
}

void
UpdateWeights(int NumHidden, int NumInput,
              double Input[NumInput + 1],
              double Weight[][NumHidden + 1],
              double DeltaWeight[][NumHidden + 1],
              double DeletaChange[NumHidden + 1])
{
	double eta = 0.5, alpha = 0.9;
	int i, j;

	// update weights based on learning rate (eta) and momentum (alpha)
	for (j = 1; j <= NumHidden; j++) {
		DeltaWeight[0][j] = eta * DeletaChange[j] + alpha * DeltaWeight[0][j];
		Weight[0][j] += DeltaWeight[0][j];
		for (i = 1; i <= NumInput; i++) {
			DeltaWeight[i][j] = eta * Input[i] * DeletaChange[j] + alpha * DeltaWeight[i][j];
			Weight[i][j] += DeltaWeight[i][j];
		}
	}
}

int
main(void)
{
	int i, k, p, np, ranpat[NUMPAT + 1], epoch;
	int NumPattern = NUMPAT, NumInput = NUMIN, NumHidden = NUMHID, NumOutput = NUMOUT;

	double Input[NUMPAT + 1][NUMIN + 1] = {{0, 0, 0}, {0, 0, 0}, {0, 1, 0}, {0, 0, 1}, {0, 1, 1}};
	double Target[NUMPAT + 1][NUMOUT + 1] = {{0, 0}, {0, 0}, {0, 1}, {0, 1}, {0, 0}};

	double WeightIH[NUMIN + 1][NUMHID + 1];
	double DeltaWeightIH[NUMIN + 1][NUMHID + 1];

	double WeightHO[NUMHID + 1][NUMOUT + 1];
	double DeltaWeightHO[NUMHID + 1][NUMOUT + 1];

	// output of the activation function
	double Hidden[NUMPAT + 1][NUMHID + 1];
	double Output[NUMPAT + 1][NUMOUT + 1];

	// stores change in weights to feedback for back propagation
	double DeltaO[NUMOUT + 1];
	double DeltaH[NUMHID + 1];

	double Error;

	srand(time(NULL));
	InitWeights(NumInput, NumHidden, WeightIH, DeltaWeightIH);
	InitWeights(NumHidden, NumOutput, WeightHO, DeltaWeightHO);

	// try to find the best weights over a set of runs
	for (epoch = 0; epoch < 1000000; epoch++) {
		// randomize order of training samples
		Shuffle(NumPattern, ranpat);

		// accumulate errors over all training pattern runs
		Error = 0.0;
		for (np = 1; np <= NumPattern; np++) {
			p = ranpat[np];

			Activate(p, NumInput, NumHidden, Input, WeightIH, Hidden);
			Activate(p, NumHidden, NumOutput, Hidden, WeightHO, Output);

			ComputeError(&Error, NumOutput, Output[p], Target[p], DeltaO);
			BackPropagate(NumHidden, NumOutput, Hidden[p], WeightHO, DeltaH, DeltaO);

			UpdateWeights(NumHidden, NumInput, Input[p], WeightIH, DeltaWeightIH, DeltaH);
			UpdateWeights(NumOutput, NumHidden, Hidden[p], WeightHO, DeltaWeightHO, DeltaO);
		}

		if (epoch % 100 == 0)
			fprintf(stdout, "\nEpoch %-5d :   Error = %f", epoch, Error);

		if (Error < 1e-6)
			break;
	}

	fprintf(stdout, "\n\nNETWORK DATA - EPOCH %d\n\nPat\t", epoch);
	for (i = 1; i <= NumInput; i++) {
		fprintf(stdout, "Input%-4d\t", i);
	}
	for (k = 1; k <= NumOutput; k++) {
		fprintf(stdout, "Target%-4d\tOutput%-4d\t", k, k);
	}
	for (p = 1; p <= NumPattern; p++) {
		fprintf(stdout, "\n%d\t", p);
		for (i = 1; i <= NumInput; i++) {
			fprintf(stdout, "%f\t", Input[p][i]);
		}
		for (k = 1; k <= NumOutput; k++) {
			fprintf(stdout, "%f\t%f\t", Target[p][k], Output[p][k]);
		}
	}
	fprintf(stdout, "\n\nGoodbye!\n\n");
	return 1;
}
