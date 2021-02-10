#include <stdio.h>
#include "include/tensor.h"

void test_convolution();
void test_line_op();
void test_transpose();
void test_dot();

static const char* str_test_convolution = "test.c:test_convolution";
void test_convolution() {
	_Msg(INFO, str_test_convolution, "Start");
	size_t shape[] = {6,6};
	size_t kshape[] = {3,3};
	//
	size_t ptrs[6*6];for (size_t i=0; i < 36; i++) ptrs[i] = i;
	size_t kptrs[3*3];for (size_t i=0; i < 9; i++) kptrs[i] = i;
	//
	double arr[36];for (size_t i=0; i < 36; i++) arr[i] = i;
	double karr[] = {1,0,0,0,1,0,0,0,1};
	//
	Ts* a = mkts(2, shape, arr, ptrs, 1, 0);
	Ts* kernel = mkts(2, kshape, karr, kptrs, 1, 0);
	//
	//show2Dimg(a);
	//printf("Convolution with  MODE:same\n");
	//show2Dimg(kernel);
	//
	STs* ret = Kconvl(a, kernel, "same");	//same do valid => no need to check valid
	//
	if (ret->isok) {
		//printf("Gives\n");
		//show2Dimg(ret->ret);
		freets(ret->ret);
		free(ret);
		_Msg(OK, str_test_convolution, "Convolution end correctly.");
	} else {
		_Msg(ERROR, str_test_convolution, "Error occurs in convolution.");
	}
	freets(a);
	freets(kernel);
};

static const char* str_test_line_op = "test.c:test_line_op";
void test_line_op() {
	_Msg(INFO, str_test_line_op, "Starting Line op");
	size_t shape[] = {3,2};
	//
	double arr0[6];for (size_t i=0; i < 6; i++) arr0[i] = i;
	double arr1[6];for (size_t i=0; i < 6; i++) arr1[i] = 2*i;
	//
	Ts* a = mkts(2, shape, arr0, 0, 1, 0);
	Ts* b = mkts(2, shape, arr1, 0, 1, 0);
	Ts* da = mkts(2, shape, 0, 0, 1, 0);
	Ts* db = mkts(2, shape, 0, 0, 1, 0);
	//
	//printf("y = a * b; (plum a, b, y)\n");
	//plumts(a);
	//plumts(b);
	//
	//Ts* y = lineop(a, b, op_mul);
	//Ts* dy = lineop(da, y, op_sub);	//0-y (because da is 0)
	//Computing y = a * b
	STs* ret_y = lineop(a, b, op_mul);	//y = ret->ret
	if (ret_y->isok == false) {
		_Msg(ERROR, str_test_line_op, "Error occurs in convolution.");
		goto ERROR;
	}
	//Computing -y
	STs* ret_dy = lineop(da, ret_y->ret, op_sub);	//dy = ret->ret
	if (ret_dy->isok == false) {
		_Msg(ERROR, str_test_line_op, "Error occurs in dy = da - y.");
		goto ERROR;
	}
	//plumts(y);
	dlineop(ret_y->ret, a, b, ret_dy->ret, da, db, op_mul);
	//
	//printf("===========dy = 0 - y;========plum dy,da,db\n");
	//plumts(dy);
	//plumts(da);
	//plumts(db);
	//
	freets(a);
	freets(b);
	//freets(ret_y->ret);
	freets(da);
	freets(db);
	freets(ret_dy->ret);
	free(ret_dy);
	_Msg(OK, str_test_line_op, "Line_op and it's derivative all correct.");
	return;
ERROR:
	freets(a);
	freets(b);
	freets(da);
	freets(db);
	_Msg(ERROR, str_test_line_op, "End with Errors");
};

void test_transpose() {
	_Msg(INFO, "test.c:test_transpose", "Start transpose");
	size_t shape[] = {3,4,2};
	double arr[24];for (size_t i=0; i < 24; i++) arr[i] = i;
	Ts* a = mkts(3, shape, arr, 0, 1, 0);
	//plumts(a);
	STs* ret_a_T = transpose(a);
	if (ret_a_T->isok) {
		//plumts(ret_a_T->ret);
		freets(ret_a_T->ret);
		free(ret_a_T);
		_Msg(OK, "test.c:test_transpose", "Transpose (Tensor conjugate) OK");
	} else {
		_Msg(ERROR, "test.c:test_transpose", "Transpose (Tensor conjugate)not ok");
	}
	freets(a);
};

void test_dot() {
	_Msg(INFO, "test.c:test_dot", "Start Dot product");
	//A
	size_t shapeA[] = {4,3};
	size_t shapeB[] = {3,4};
	//
	double arr[12];for (size_t i=0; i < 12; i++) arr[i] = i+1;	//same for A and B
	//
	Ts* a = mkts(2, shapeA, arr, 0, 1.0, 0);
	Ts* b = mkts(2, shapeB, arr, 0, 1.0, 0);
	//
	STs* ret = dot(a, b);
	//
	if (ret->isok) {
		freets(ret->ret);
		free(ret);
		_Msg(OK, "test.c:test_dot", "Dot product ok");
	} else {
		_Msg(ERROR, "test.c:test_dot", "Dot product not ok");
	}
	freets(a);
	freets(b);
};

void test_pool() {
	_Msg(INFO, "test.c:test_pool", "Start Pool testing");
	//A
	double arr[4*4*4];
	for (size_t i=0; i < 4*4*4; i++) arr[i] = i;
	//Block sizes
	size_t blsizes[] = {2,2,2};
	size_t shape[] = {4,4,4};
	Ts* A = mkts(3, shape, arr, 0, 1, 0);
	//
	STs* ret = pool(A, pool_max, blsizes);
	//
	if (!ret->isok) {
		_Msg(ERROR, "test.c:test_pool", "Error in Pool Max");
		freets(A);
		free(ret);
	} else {
		//printf("Max from 1,2,10,11,17,18,20,21 is 21\n");
		//plumts(A);
		//plumts(ret->ret);
		freets(A);
		freets(ret->ret);
		free(ret);
		_Msg(OK, "test.c:test_pool", "Max pooling ok");
	}
};

int main() {
	test_dot();
	test_transpose();
	test_line_op();
	test_convolution();
	test_pool();
	return 0;
}