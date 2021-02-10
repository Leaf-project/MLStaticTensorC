#pragma once

#include "tensor.h"

typedef struct {
	//Array of Instructions (Math code)
	size_t insts;	//each command have a length (can be variable or even mutable)
	size_t **inst;	/*[command id][param id]
	Forward is just execution from 0 to last.
	Backward is execution from last to 0.
	*/
	size_t inputs, outputs;	//How many inputs and outputs (lenght of Code.input & Code.output arrays)
	//
	size_t *input, *output;	//Whitch id correspond tho 0'th input, 1'rst input ...
} Insts;	//Instructions

typedef struct {
	//Main
	Ts* *Mvar;	//	Main vars
	Ts* *Mgrad;	//	Main grads; it will be sum (or other) of grad
	//Times
	Ts* **var;	//	var[time][var id]
	Ts* **grad;	//	1st order gradient [time][var id]
	//
	size_t times;	//lenght of {var & grad} arrays
	size_t vars;	//lenght of {var[time] & grad[time]} arrays
	//
	size_t grnd;	//global srand of model
} Model;

//Security Mesure
void _track_unused(Model *mdl, Insts *insts);	//track unused tensors in Instructions and set thoes vars to unused type
//void _track_nan(Model *mdl);	//Replace -nan by -1e6 and +nan by 1e6
#include "../src/model_security.c"

//Memory io
Model* loadmdl(char *file);	//Load only one grad
Model* cpymdl(Model *mdl);
void freemdl(Model *mdl);
void savemdl(Model *mdl, char *file);
//
Model* loadinsts(char *file);
Model* cpyinsts(Model *mdl);
void freeinsts(Model *mdl);
void saveinsts(Model *mdl, char *file);
#include "../src/model_mem.c"

//Controle
void rnd_noise_cells(Model *mdl, size_t time);
void restart(Model *mdl, size_t time);
void clear(Model *mdl);	//clear from 1 to ses->times (not the first)
void expand(Model *mdl);
void mkgrads(Model *mdl, size_t time);	//after forward (to provide non-static methodes)
void sum_wgrads(Model* mdl);	//sum only weights gradients
#include "../src/model_controle.c"

//Str
void plumgrads(Model *mdl);
void plumvars(Model *mdl);
void pluminsts(Insts *insts);
void plummdl(Model *mdl);
#include "../src/model_str.c"

//Compute
void forward(Model *mdl, Insts *insts, size_t time);
void backward(Model *mdl, Insts *insts, size_t time);
#include "../src/model_compute.c"