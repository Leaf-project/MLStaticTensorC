#pragma once

#include <math.h>

//Definitions
typedef double (*Op)(double,double);
typedef double (*Fn)(double);
typedef double (*PoolFn)(Ts*);
typedef double (*PoolFn2inp)(Ts*,Ts*);

//Operators
double op_add(double a, double b) {return a+b;};
double op_sub(double a, double b) {return a-b;};
double op_mul(double a, double b) {return a*b;};
double op_div(double a, double b) {return a/b;};
double op_pow(double a, double b) {return pow(a, b);};

Op ops[5] = {op_add, op_sub, op_mul, op_div, op_pow};

//Const strings
const char *str_types[] = {
	"Var", "Weight", "Const", "Un Used", "Noise"
};

const char *str_ops[] = {
	"+", "-", "*", "/", "^"
};

const char *str_opts[] = {
	"Gradient Descent (Gd)",
	"Gd with Momentum (Momentum)",
	"Root Mean Square Propagation (RMSProp)",
	"Adaptive Gradient (AdaGrad)",
	"Adaptive Moment Estimation (Adam)"
};

const char *str_funcs[] = {
	"identity",
	"bin_step",
	"Logistic",
	"Tanh",
	"ReLU",
	"GELU",
	"SoftPlus",
	"ELU",
	"SELU",
	"Leaky_ReLU",
	"Softsign",
	"SQNL",
	"Bent_Ind",
	"Gauss",
	"SQ_RBF",
	"SoftMax",
	"Maxout"
};

const char *str_complexfuncs[] = {
	"Seuil",
};