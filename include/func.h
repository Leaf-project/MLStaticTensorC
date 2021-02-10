#pragma once

#include "tensor.h"

//Functions
typedef Ts* (*Activation)(Ts*);

//Repited Patern
Ts* func(Ts* x, Fn fn);
//Classic
Ts* identity(Ts *x);
Ts* xor_fn(Ts *x);
Ts* bin_step(Ts *x);
Ts* Logistic(Ts *x);
Ts* Tanh(Ts *x);
Ts* ReLU(Ts *x);
Ts* GELU(Ts *x);
Ts* SoftPlus(Ts *x);
Ts* ELU(Ts *x);
Ts* SELU(Ts *x);
Ts* Leaky_ReLU(Ts *x);
Ts* Softsign(Ts *x);
Ts* SQNL(Ts *x);
Ts* Bent_Ind(Ts *x);
Ts* Gauss(Ts *x);
Ts* SQ_RBF(Ts *x);
//Spetials
Ts* SoftMax(Ts *x);
Ts* Maxout(Ts *x);
#include "../src/func/func.c"

Activation funcs[18] = {identity, xor_fn, bin_step, Logistic, Tanh, ReLU, GELU, SoftPlus, ELU, SELU, Leaky_ReLU, Softsign, SQNL, Bent_Ind, Gauss, SQ_RBF, SoftMax, Maxout};

//1st order Derivative
Ts* didentity(Ts* x);
Ts* dxor_fn(Ts *x);
Ts* dbin_step(Ts* x);
Ts* dLogistic(Ts* x);
Ts* dTanh(Ts* x);
Ts* dReLU(Ts* x);
Ts* dGELU(Ts* x);
Ts* dSoftPlus(Ts* x);
Ts* dELU(Ts* x);
Ts* dSELU(Ts* x);
Ts* dLeaky_ReLU(Ts* x);
Ts* dSoftsign(Ts* x);
Ts* dSQNL(Ts* x);
Ts* dBent_Ind(Ts* x);
Ts* dGauss(Ts* x);
Ts* dSQ_RBF(Ts* x);
//
Ts* dSoftMax(Ts* x);
Ts* dMaxout(Ts* x);
#include "../src/func/dfunc.c"

Activation dfuncs[18] = {didentity, dxor_fn, dbin_step, dLogistic, dTanh, dReLU, dGELU, dSoftPlus, dELU, dSELU, dLeaky_ReLU, dSoftsign, dSQNL, dBent_Ind, dGauss, dSQ_RBF, dSoftMax, dMaxout};

//Pool functions
double pool_max(Ts* ts) {
	double max = 0.0;
	for (size_t i=0; i < ts->len; i++) {
		if (GTSPOS(ts, i) > max) max = GTSPOS(ts, i);
	}
	return max;
};

double pool_mean(Ts* ts) {
	double mean = 0.0;
	for (size_t i=0; i < ts->len; i++) {
		if (GTSPOS(ts, i) > max) max = GTSPOS(ts, i);
	}
	return max;
};