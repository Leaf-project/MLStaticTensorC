double d_identity(double x) {
	(void)x;
	return 1.0;
};

double d_xor_fn(double x) {
	return 400*x / pow(pow(x, 2) + 100, 2);
};

double d_bin_step(double x) {
	(void)x;
	return 0.0;
};

double d_logistic(double x) {
	double a = _logistic(x);
	return a*(1 - a);
};

double d_tanh(double x) {
	return 1.0 - pow(tanh(x), 2);
};

double d_ReLU(double x) {
	if (x < 0) return 0;
	else return 1;
};

double d_GELU(double x) {
	double tmp0 = tanh(pow(2,0.5) * (0.044715*pow(x,3))/pow(M_PI, 0.5));
	return (pow(2,0.5)*x*(1-pow(tmp0, 2))*(0.134145*pow(x, 2) + 1)/pow(M_PI, 0.5) + tmp0 + 1) / 2;
};

double d_SoftPlus(double x) {
	return 1 / (1 + exp(-x));
};

double d_ELU(double x) {
	double a=1;
	if (x < 0) return a*exp(x);
	else return 1;
};

double d_SELU(double x) {
	double lambda=1.0507, alpha=1.67326;
	if (x < 0) return lambda * alpha * exp(x);
	else return lambda;
};

double d_Leaky_ReLU(double x) {
	if (x < 0) return 0.01;
	else return 1;
};

double d_Softsign(double x) {
	return 1 / pow(1 + fabs(x), 2);
};

double d_SQNL(double x) {
	return 1 + x/2;
};

double d_Bent_Ind(double x) {
	return x / (2 * pow(pow(x, 2)+1, 0.5)) + 1;
};

double d_Gauss(double x) {
	return -2*x*exp(-pow(x, 2));
};

double d_SQ_RBF(double x) {
	if (fabs(x) <= 1) return -x;
	else if (1 < fabs(x) && fabs(x) < 2) return x - 2*sign(x);
	else return 0;
};

//
Ts* didentity(Ts* x)	{return func(x, &d_identity);};
Ts* dxor_fn(Ts *x)		{return func(x, &d_xor_fn);};
Ts* dbin_step(Ts* x)	{return func(x, &d_bin_step);};
Ts* dLogistic(Ts* x)	{return func(x, &d_logistic);};
Ts* dTanh(Ts* x) 		{return func(x, &d_tanh);};
Ts* dReLU(Ts* x)		{return func(x, &d_ReLU);};
Ts* dGELU(Ts* x)		{return func(x, &d_GELU);};
Ts* dSoftPlus(Ts* x)	{return func(x, &d_SoftPlus);};
Ts* dELU(Ts* x)			{return func(x, &d_ELU);};
Ts* dSELU(Ts* x)		{return func(x, &d_SELU);};
Ts* dLeaky_ReLU(Ts* x)	{return func(x, &d_Leaky_ReLU);};
Ts* dSoftsign(Ts* x)	{return func(x, &d_Softsign);};
Ts* dSQNL(Ts* x)		{return func(x, &d_SQNL);};
Ts* dBent_Ind(Ts* x)	{return func(x, &d_Bent_Ind);};
Ts* dGauss(Ts* x)		{return func(x, &d_Gauss);};
Ts* dSQ_RBF(Ts* x)		{return func(x, &d_SQ_RBF);};
//
Ts* dSoftMax(Ts* x) {
	/*
	def softmax(arr):
		arr = np.exp(arr - max(arr))
		return arr / sum(arr)

	def dsoftmax(arr):
		arr = np.exp(arr - max(arr))
		sum = sum(arr)
		return arr * (sum - arr) / sum**2
	*/
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
	for (i=0; i < x->len; i++) lst[i]  = lst[i] * (tmp - lst[i]) / pow(tmp, 2);
	//
	Ts *ret = cpyts(x);
	filltslist(ret, lst);
	free(lst);
	return ret;
};

Ts* dMaxout(Ts *x) {
	size_t i;
	double* lst = (double*)malloc(x->len * sizeof(double));
	size_t max=0;	//here max
	for (i=0; i < x->len; i++) if (x->arr[i] > max) max = x->arr[i];
	//
	for (i=0; i < x->len; i++) {
		if (i == max) lst[i] = 1.0;
		else lst[i] = 0.0;
	};
	Ts *ret = cpyts(x);
	filltslist(ret, lst);
	free(lst);
	return ret;
};