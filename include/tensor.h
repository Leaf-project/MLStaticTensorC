#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#define ln(x) log(x)
#define log(x) log10(x)

typedef struct {	//Ts[z][y][x] == getv(Ts, {x,y,z})
	//A tensor can be : [[[1,2,3,4],[4,5,6,7]]] shape:(4,2,1) _real_sizes:(4,8)
	size_t deep;
	size_t *shape;			//x,y,z ... (from smallest to biggest)
	size_t *_real_sizes;	//how many doubles contain one of block of shape[n]
	//
	size_t len;				//can be computed with : _real_shapes[0] * shape[0]
	double *arr;
	//Pointers to whitch value pos real point in arr (can be [0,1,2,3,4...])
	size_t *pointers;
	//
	char type;	/*
0 - mut 			(Var)
1 - pseudo-mutable 	(Memory Cell)
2 - imutable 		(Const)
3 - pseudo-imutable (Weights)
4 - un-used			(Garbadge)
	*/
	//"Plugin" value 		Set to 0 by default drop of value of array (not written to files)
	bool *droped_out;	//0 meen no drop-out (ignoring drop_out step)
	float importance;	// update : change_in_w *= importance
} Ts;	//Tensor

#include "const.h"
#include "debbug.h"
#include "security.h"

//Controle
//Get pos of arr from pointers (if droped : return 0.0)
//STSPOS : statement        GTSPOS : value
#define STSPOS(ts, x) ts->arr[ts->pointers[x]]
#define GTSPOS(ts, x) (ts->droped_out[ts->pointers[x]] ? 0.0 : STSPOS(ts, x))
//definitions ends with POS but it return DOUBLE value
//
size_t realpos(Ts* ts, size_t *indexs);		//compute pos is 1-dimention array from multi dim indexs {x,y,z...}
size_t* indexspos(Ts* ts, size_t pos);		//Reciprocal from `realpos` 
double getv(Ts* ts, size_t *pos);			//Get value from {x,y,z...}
void setv(Ts* ts, size_t *pos, double v);	//Set value to {x,y,z...}
void addv(Ts* ts, size_t *pos, double v);
void filltslist(Ts *ts, double *value);
void fillts(Ts *ts, double value);
void noisets(Ts *ts);						//Set random values to each value of array
void noisedrop(Ts *ts);						//Set random 0 or 1 to each dropout
void setdrop(Ts *ts, bool v);
#include "../src/tensor/controle.c"

//Iterate through Tensor values with multi-dim index's
bool update(size_t *indexs, size_t *max, size_t n);	//0 if end else 1
#include "../src/tensor/iterator.c"

//Memory
Ts* mkts(size_t deep, size_t *shape, double *arr, size_t *ptrs, float importance, char type);
Ts* cpyts(Ts *ts);
void freets(Ts *ts);
Ts* takefromts(Ts *ts, size_t *froms, size_t *tos);	//take part of a tensor (like right corner)
Ts* extend(Ts *ts, size_t *ns, double v);		//Extend by n filling by v
#include "../src/tensor/mem.c"

//io operations
void writets(Ts *ts, FILE* fp);
//void rsa_writets(Ts *ts, FILE *fp, size_t key)
Ts* readts(FILE *fp);
//void rsa_readts(FILE *fp, size_t key);
#include "../src/tensor/io.c"

//Str
char** strts(Ts *ts);
void plumts(Ts *ts);		//print tensor
void plumshape(Ts *ts);
//
void show2Dimg(Ts *ts);
void showRGBimg(Ts *ts);
#include "../src/tensor/str.c"

//Operations (nodes in tree wiew)	
STs* lineop(Ts *a, Ts *b, Op op);	//linear operator	: y = a (op) b 		(+,-,*,/,^)
STs* iter(Ts *ts, Op op);			//iter operator	 	: y = (op)iter a    (+,-,*,/,^)
STs* transpose(Ts *ts);				//.T 				: y = a.T
STs* dot(Ts *a, Ts *b);				//Tensor dot		: y = a @ b
STs* Kconvl(Ts *a, Ts *kernel, char* mod);	//Kernel convolution : y = a | kernel ('same':(out.sh == inp.sh), 'valid':no padding)
STs* convl(Ts *a, Ts *b, size_t x);	//Simple convolution : y = a #x b where (2x+1) is number of connections and x level of connections (like size of kernel for Kconvl)
STs* pool(Ts *a, PoolFn fn, size_t *block_sizes);	//max, average, ... : y = pool(a, size:arr)
STs* pyramidals(Ts *a, Ts* W, char* mod, size_t* params);
//
void dlineop(Ts* Y, Ts* A, Ts* B, Ts* dY, Ts* dA, Ts* dB, Op op);	//(y=a+b;da=dy;db=dy);(y=a-b;da=dy;db=-dy);(y=a*b;da=dy*b;db=dy*a);(y=a/b;da=dy/b;db=-a*dy/b**2);(y=a^b;da=dy*b*a^(b-1);db=dy*y*ln(a))
void diter(Ts* y, Ts* a, Ts* dy, Ts* da, Op op);
void dtranspose(Ts* dY, Ts* dA);
void ddot(Ts *dy, Ts *a, Ts *b, Ts *da, Ts *db);
void dKconvl(Ts* dy, , Ts* b);
void dconv(Ts *dy, Ts *a, Ts *b, Ts *da, Ts *db);
void dpool(Ts *a, Ts *dy, Ts *da, PoolFn fn, size_t* block_sizes);
void dpyramidals(Ts* Y, Ts* dY, Ts *A, Ts* W, Ts *dA, Ts* dW, char* mod, size_t* params);
#include "../src/tensor/operations.c"

//Debbug
void debbug_ts(Ts* ts);
#include "../src/tensor/debbug.c"

#include "func.h"	//<-- here is defined all funcs
#include "complexfunc.h"	//<-- here is defined all funcs