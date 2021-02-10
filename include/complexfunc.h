#pragma once

#include "tensor.h"

typedef Ts* (*ComplexActivation)(Ts**, size_t);

Ts* Seuil(Ts** xs, size_t n);	//Take only the 0th for x's and 1rst for seuil's
#include "../src/func/complexfunc.c"
ComplexActivation complexfuncs[1] = {Seuil};

//Derivatives of
Ts* dSeuil(Ts** xs, size_t n);	//Take only the 0th for x's and 1rst for seuil's
#include "../src/func/dcomplexfunc.c"

ComplexActivation dcomplexfuncs[1] = {dSeuil};