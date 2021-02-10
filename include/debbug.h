#pragma once

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#define MIN_RND -1
#define MAX_RND 1

typedef enum {
	OK,
	ERROR,	//can have label ERROR: and ERROR in enum
	INFO,
	WARRNING
} MSG_TYPE;

void _Msg(MSG_TYPE type, const char* fromwhere, const char* text, ...) {
	if (type==OK) printf("[\033[92;42mOk\033[0m]:\033[96m%s\033[92m ", fromwhere);//92
	else if (type == ERROR) printf("[\033[30;101mError\033[0m]:\033[96m%s\033[91m ", fromwhere);
	else if (type == INFO) printf("[\033[1;30;46mInfo\033[0m]:\033[96m%s\033[34m ", fromwhere);
	else if (type == WARRNING) printf("[\033[35;1;41mWarrning\033[0m]:\033[96m%s\033[35m ", fromwhere);
	//
	va_list valist;
	va_start(valist, text);
	printf(text, valist);
	va_end(valist);
	printf("\033[0m\n");
};

double rand_double() {
	double scale = rand() / (double) RAND_MAX; /* [0, 1.0] */
	return MIN_RND + scale * ( MAX_RND - MIN_RND );      /* [min, max] */
};	//return random.uniform(-1, 1)

void sumarr(size_t* arr0, size_t* arr1, size_t len) {
	for (size_t i=0; i < len; i++)
		arr0[i] += arr1[i];
};

void subarr(size_t* arr0, size_t* arr1, size_t len) {
	for (size_t i=0; i < len; i++)
		arr0[i] -= arr1[i];
};

void mularr(size_t* arr0, size_t* arr1, size_t len) {
	for (size_t i=0; i < len; i++)
		arr0[i] *= arr1[i];
};

void divarr(size_t* arr0, size_t* arr1, size_t len) {
	for (size_t i=0; i < len; i++)
		arr0[i] /= arr1[i];
};

void plumarr(size_t* arr, size_t len) {
	for (size_t i=0; i < len; i++)
		printf("%li,", arr[i]);
	printf("\n");
};

void fillarr(size_t *arr, size_t n, size_t v) {
	for (size_t i=0; i < n; i++)
		arr[i] = v;
};	//fill array of n lenght with v

size_t mul(size_t *lst, size_t len) {
	size_t ret=1;
	for (size_t i=0; i < len; i++) ret *= lst[i];
	return ret;
};	// {1,2,3,4} => 1*2*3*4

double sign(double x) {
	if (x >= 0) return 1.0;
	else return -1.0;
};

char *dtoa(double a, const char* s) {	//double to string
	size_t len = snprintf(NULL, 0, s, a);
	char* str = malloc( len + 1 );
	snprintf(str, len+1, s, a);
	return str;
};

char *sttoa(size_t a, const char* s) {	//size_t to string
	size_t len = snprintf(NULL, 0, s, a);
	char* str = malloc(len + 1);
	snprintf(str, len+1, s, a);
	return str;
};

void freestrs(char* *strs) {
	char *str; size_t lp=0;
	while ((str = strs[lp++])[0] != '\0') free(str);
	free(strs);
};

void plumstrs(char **strs) {
	char *str; size_t lp=0;
	while ((str = strs[lp++])[0] != '\0') {
		printf("%s|\n", str);
	};	
};

char *newline(size_t len) {
	char *ret = malloc(len);
	memset(ret, ' ', len-1);
	ret[len-1]='\0';
	return ret;
};

size_t lineslen(char* *strss) {
	char *str;size_t lines = 0;
	while ((str = strss[lines++])[0] != '\0'){};
	return lines-1;
};