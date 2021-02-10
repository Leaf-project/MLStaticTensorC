Ts* func(Ts* x, Fn fn) {
	double* lst = (double*)malloc(x->len * sizeof(double));
	for (size_t i=0; i < x->len; i++) lst[i] = fn(x->arr[i]);
	Ts* ret = cpyts(x);
	filltslist(ret, lst);
	free(lst);
	return ret;
};

double _identity(double x) {
	return x;
};

double _xor_fn(double x) {
	x = pow(x, 2);
	return (x - 100)/(x + 100);
};

double _bin_step(double x) {
	if (x < 0) x = 0;
	else x = 1;
	return x;
};

double _logistic(double x) {	// 1 / (1 + e**(-x))
	return 1 / (1 + exp(-x));
};

double _tanh(double x) {
	return tanh(x);
};

double _ReLU(double x) {
	if (x <= 0) return 0;
	else return x;
};

double _GELU(double x) {
	return 0.5*x + 0.5*x*tanh(pow(2/M_PI, 0.5) * (x + 0.044715*pow(x, 3)));
};

double _SoftPlus(double x) {
	return log(1 + exp(x));
};

double _ELU(double x) {
	double a=1;	//(-a,inf)
	if (x <= 0) return a*(exp(x)-1);
	else return x;
};

double _SELU(double x) {
	double lambda=1.0507, alpha=1.67326;
	if (x <= 0) return lambda*alpha*(exp(x)-1);
	else return lambda*x;
};

double _Leaky_ReLU(double x) {
	if (x < 0) return 0.01*x;
	else return x;
};

double _Softsign(double x) {
	return x / (1 + fabs(x));
};

double _SQNL(double x) {
	if (x > 2) return 1;
	else if (x >= 0 && x <= 2) return x - pow(x, 2)/4;
	else if (x < 0 && -2 <= x) return x + pow(x, 2)/4;
	else return -1;
};

double _Bent_Ind(double x) {
	return (pow(pow(x,2) + 1,0.5) - 1)/2 + x;
};

double _Gauss(double x) {
	return exp(-pow(x, 2));
}

double _SQ_RBF(double x) {
	if (fabs(x) <= 1) return 1 - pow(x, 2)/2;
	else if (1 < fabs(x) && fabs(x) < 2) return pow(2 - fabs(x), 2)/2;
	else return 0;
};

Ts* identity(Ts *x)		{return func(x, &_identity);};
Ts* xor_fn(Ts *x)		{return func(x, &_xor_fn);};
Ts* bin_step(Ts *x)		{return func(x, &_bin_step);};
Ts* Logistic(Ts *x)		{return func(x, &_logistic);};
Ts* Tanh(Ts *x) 		{return func(x, &_tanh);};
Ts* ReLU(Ts *x)			{return func(x, &_ReLU);};
Ts* GELU(Ts *x)			{return func(x, &_GELU);};
Ts* SoftPlus(Ts *x) 	{return func(x, &_SoftPlus);};
Ts* ELU(Ts *x)			{return func(x, &_ELU);};
Ts* SELU(Ts *x)			{return func(x, &_SELU);};
Ts* Leaky_ReLU(Ts *x)	{return func(x, &_Leaky_ReLU);};
Ts* Softsign(Ts *x)		{return func(x, &_Softsign);};
Ts* SQNL(Ts *x)			{return func(x, &_SQNL);};
Ts* Bent_Ind(Ts *x)		{return func(x, &_Bent_Ind);};
Ts* Gauss(Ts *x)		{return func(x, &_Gauss);};
Ts* SQ_RBF(Ts *x)		{return func(x, &_SQ_RBF);};

Ts* SoftMax(Ts *x) {
	double *lst = (double*)malloc(x->len * sizeof(double));
	//
	size_t i;
	double tmp = 0.0;	//here max
	for (i=0; i < x->len; i++) if (x->arr[i] > tmp) tmp = x->arr[i];
	//
	for (i=0; i < x->len; i++) lst[i] = exp(x->arr[i]-tmp);
	//	here sum
	tmp = 0.0;
	for (i=0; i < x->len; i++) tmp += lst[i];
	for (i=0; i < x->len; i++) lst[i] /= tmp;
	//
	Ts *ret = cpyts(x);
	filltslist(ret, lst);
	free(lst);
	return ret;
};

Ts* Maxout(Ts *x) {
	size_t i;
	double *lst = (double*)malloc(x->len * sizeof(double));
	size_t max=0;	//here max
	for (i=0; i < x->len; i++) if (x->arr[i] > max) max = x->arr[i];
	//
	for (i=0; i < x->len; i++) lst[i] = x->arr[max];
	Ts *ret = cpyts(x);
	filltslist(ret, lst);
	free(lst);
	return ret;
};