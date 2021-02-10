#pragma once

#include "model.h"
#include "data.h"

typedef Tensor* (*Opti)(Tensor*,Tensor*,double**);
//Optimizers
void Sgd(Tensor* w, Tensor* dw, double** pass);
void Momentum(Tensor* w, Tensor* dw, double** pass);
void RMSProp(Tensor* w, Tensor* dw, double** pass);
void Adam(Tensor* w, Tensor* dw, double** pass);
#include "../src/model/opti.c"

typedef struct {
	//Population
	Model** mdl;	//mdl[id]
	size_t mdls;
	//Code
	Insts* insts;	//One set of instructions for all models
	//Scores
	double** scores;	//scores[mdl][batch] -> double
	double* gscors;		//global scores (sum/median/average...) gscors[mdl] -> double
	//Data
	Data* data;		//one data slot
	Data* test;		//test data
	//
	Opti opti;			//one optimizer for all models
	double drop_out;	//set to 0.0 by default (meen no dropout)
	//
	double**** pass;	//pass[mdl][var][arr id][pass id]
} Session;	//Session including some models (can be theaded)

//Compute scores
void compute_scores(Session* ses);	//
#include "../src/model/scores.c"

//Update weights
void update(Model *mdl, Opti opti, double** *pass);	//update from grad[grad_time] (should be 0) to var[var_t]
#include "../src/model/update.c"

//Update population (weight agnostic)
void wa_nothing(Session* ses);	//Do nothing
void wa_randomw(Session* ses);	//Randomly inter-change weights	(when all vars are same)
void wa_elite(Session* ses, size_t p);	//Get 30% best and duplicate to get 100% off old best
void wa_code(Session* ses);				//add nodes, change activation function, cells ...
#include "../src/model/uppop.c"

typedef struct {
	Session* *ses;	//Array of sessions
	size_t sess;
	//
} MetaSession;	//Can have multiple codes

typedef struct {
	MetaSession* metases;
} Server;	//Start server on metasession to make more in-deep train, visualisation, online-train