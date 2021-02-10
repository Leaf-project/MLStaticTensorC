/*
for X in Y,A,B
All dX->arr[i] += value
+= and not = because A can be used mutiple times:
y0 = 3*a
y1 = 2*a
y2 = a - 4
There a is used 3 times. By using '=' grad(A) will take only grad(y2)
and not grad(y2) + grad(y1)*2 + grad(y0)*3
*/

char _same_shapes(Ts *a, Ts *b) {
	if (a->deep != b->deep) return 0;
	for (size_t i=0; i < a->deep; i++)
		if (a->shape[i] != b->shape[i])
			return 0;
	return 1;
};

Ts* lineop(Ts *a, Ts *b, Op op) {
	Ts *ret = cpyts(a);
	if (_same_shapes(a, b)) {
		for (size_t i=0; i < a->len; i++) {
			ret->arr[i] = op(a->arr[i], b->arr[i]);
		}
	} else if (a->len == 1) {
		for (size_t i=0; i < b->len; i++)
			ret->arr[i] = op(a->arr[0], b->arr[i]);
	} else if (b->len == 1) {
		for (size_t i=0; i < a->len; i++)
			ret->arr[i] = op(a->arr[i], b->arr[0]);
	} else {
		printf("Operator not same shape.\n");
		freets(ret);
		exit(1);
	}
	return ret;
};

Ts* iter(Ts *ts, Op op) {
	size_t shape[] = {1};
	double arr[1];
	size_t i=0;
	if (op == op_add || op == op_sub) arr[0] = 0.0;
	else if (op == op_mul || op == op_div || op == op_pow) {
		arr[0] = ts->arr[0];
		i = 1;
	}
	//
	for (; i < ts->len; i++) arr[0] = op(arr[0], ts->arr[i]);
	//
	Ts *ret = mkts(1, shape, arr, ts->type);
	return ret;
};

Ts* activate(Ts *ts, Fn fn) {
	Ts *ret = cpyts(ts);
	for (size_t i=0; i < ts->len; i++) ret->arr[i] = fn(ts->arr[i]);
	return ret;
};

Ts* transpose(Ts *a) {
	//Temporaty
	int i;size_t j;
	//Mk tensor with inversed shape
	Ts *ret = (Ts*)malloc(sizeof(Ts));
	ret->deep = a->deep;
	ret->shape = (size_t*)malloc(ret->deep * sizeof(size_t));
	ret->_real_sizes = (size_t*)malloc(ret->deep * sizeof(size_t));
	for (j=0; j < a->deep; j++) ret->shape[j] = a->shape[a->deep-j-1]; //Inverse shape
	_cmp_real_sizes(ret);
	ret->len = ret->_real_sizes[ret->deep-1] * ret->shape[ret->deep-1];
	ret->arr = (double*)malloc(ret->len * sizeof(double));
	ret->type = 0; //because change
	//indexes
	size_t *iters = (size_t*)malloc(ret->deep * sizeof(size_t));	//Reversed
	size_t *invers_iters = (size_t*)malloc(a->deep * sizeof(size_t));
	//
	for (j=0; j < ret->deep; j++) {
		iters[j]=0;invers_iters[j]=0;
	}
	//
	char loop = 1;
	//
	while (loop) {
		//inverse iter
		for (j=0; j < ret->deep; j++) invers_iters[ret->deep-j-1] = iters[j];
		//
		ret->arr[realpos(ret, invers_iters)] = a->arr[realpos(a, iters)];	//Transposed pos
		//Update iters (+1)
		i = a->deep;		//a->deep-1  (like i=0)
		//Loop to make +1 to lst (with limits on each case)
		while (--i >= 0) {	//i >= 0; to the end of while add --i;
			iters[i] += 1;
			if (iters[i] != a->shape[i]) break;	//Chech is under limite (<)
			else if (i == 0) loop = 0;
			else iters[i] = 0;
		}
	}
	return ret;
};

char _ok_dot(Ts* a, Ts* b) {
	if (a->shape[0] != b->shape[1]) return 0;
	if (a->deep > 2) for (size_t i=2; i < a->deep; i++) if (a->shape[i] != b->shape[i]) return 0;
	return 1;
};

Ts* dot(Ts* a, Ts* b) {
	Ts* ret;
	if (a->deep == 1 && b->deep == 1) {
		//Spetial functions
		if (a->shape[0] == b->shape[0]) {
			size_t shape[] = {1};
			double arr[1] = {0.0};
			ret = mkts(1, shape, arr, 0);
			for (size_t i=0; i < a->len; i++) ret->arr[0] += a->arr[i] * b->arr[i];
		} else {
			printf("Dot product, a->deep == b->deep == 1, shapes : %li %li\n", a->shape[0], b->shape[1]);
			exit(1);
		}
	} else if (a->deep != b->deep) {
		printf("Dot product not same deep : %li != %li\n", a->deep, b->deep);
		exit(1);
	} else if (_ok_dot(a, b)) {
		size_t tmp_;	//tmp_ is just here to make a,b = b,a (tmp_ is c in :c=a;a=b;b=c)	(but to a,b=b,a we can make (useless x2 slower) : a=a-b;b=a+b;a=-a+b)
		size_t j;
		double tmp0, tmp1;	//elm in line or col in mat_a and mat_b
		//
		size_t* shape = (size_t*)malloc(a->deep * sizeof(size_t));
		for (j=0; j < a->deep; j++) shape[j] = b->shape[j];
		shape[1] = a->shape[1];
		//
		size_t len = mul(shape, a->deep);
		double *arr = (double*)malloc(len * sizeof(double));
		for (j=0; j < len; j++) arr[j] = 0.0;
		ret = mkts(a->deep, shape, arr, 0);
		//
		size_t* iters = (size_t*)malloc(a->deep * sizeof(size_t));
		for (j=0; j < a->deep; j++) iters[j] = 0;
		//
		char loop = 1;
		while (loop) {	//for each in ret
			/* Algo: make normal 2-dim dot product but repeat at highter layer */
			for (j=0; j < a->shape[0]; j++) {
				//get tmp0 from a
				tmp_ = iters[0];
				iters[0] = j;
				tmp0 = a->arr[realpos(a, iters)];
				iters[0] = tmp_;
				//get tmp1 from b
				tmp_ = iters[1];
				iters[1] = j;
				tmp1 = b->arr[realpos(b, iters)];
				iters[1] = tmp_;
				//Add
				ret->arr[realpos(ret, iters)] += tmp0 * tmp1;
			}
			//
			for (j=0; j < a->deep; j++) {
				iters[j]++;
				if (iters[j] != ret->shape[j]) break;
				else if (j == a->deep-1) loop = 0;
				else iters[j] = 0;
			}
		};
	} else {
		printf("Dot correct deeps but Invalid shapes deep:%li and a->shape[0] != b->shape[1] (%li != %li)\n", a->deep, a->shape[0], b->shape[1]);
		exit(1);
	}
	return ret;
};

void ddot(Ts* dY, Ts* A, Ts* B, Ts* dA, Ts* dB) {
	if (A->deep == 1 && B->deep == 1) {
		//Spetial function
		for (size_t i=0; i < A->len; i++) {
			dA->arr[i] += B->arr[i]*dY->arr[0];
			dB->arr[i] += A->arr[i]*dY->arr[0];
		}
	} else {
		size_t tmp_;	//tmp_ is just here to make a,b = b,a (tmp_ is c in :c=a;a=b;b=c)	(but to a,b=b,a we can make (useless x2 slower) : a=a-b;b=a+b;a=-a+b)
		size_t j,tmp_pos_0, tmp_pos_1;
		double tmp_dbl;	//elm in line or col in mat_a and mat_b
		//
		size_t* iters = (size_t*)malloc(A->deep * sizeof(size_t));
		for (j=0; j < A->deep; j++) iters[j] = 0;
		//
		char loop = 1;
		while (loop) {	//for each in ret
			/* Algo: make normal 2-dim dot product but repeat at highter layer */
			for (j=0; j < A->shape[0]; j++) {
				//get tmp0 from a
				tmp_ = iters[0];
				iters[0] = j;
				tmp_pos_0 = realpos(A, iters);
				iters[0] = tmp_;
				//get tmp1 from b
				tmp_ = iters[1];
				iters[1] = j;
				tmp_pos_1 = realpos(B, iters);
				iters[1] = tmp_;
				//Add
				tmp_dbl = dY->arr[realpos(dY, iters)];
				dA->arr[tmp_pos_0] += tmp_dbl * B->arr[tmp_pos_1];
				dB->arr[tmp_pos_1] += tmp_dbl * A->arr[tmp_pos_0];
			}
			//
			for (j=0; j < A->deep; j++) {
				iters[j]++;
				if (iters[j] != dY->shape[j]) break;
				else if (j == A->deep-1) loop = 0;
				else iters[j] = 0;
			}
		};
	}
};

void dlineop(Ts* Y, Ts* A, Ts* B, Ts* dY, Ts* dA, Ts* dB, Op op) {
	if (op == op_add) {			//	y = a + b
		for (size_t i=0; i < A->len; i++) {
			dA->arr[i] += dY->arr[i];
			dB->arr[i] += dY->arr[i];
		}
	} else if (op == op_sub) {	//	y = a - b
		for (size_t i=0; i < A->len; i++) {
			dA->arr[i] += dY->arr[i];
			dB->arr[i] += dY->arr[i];
		}
	} else if (op == op_mul) {	//	y = a * b
		for (size_t i=0; i < dA->len; i++) {
			dA->arr[i] += (dY->arr[i] * B->arr[i]);
			dB->arr[i] += (dY->arr[i] * A->arr[i]);
		}
	} else if (op == op_div) {	//	y = a / b
		//y = a/b where (da = dy/b) and (db = dy*a*-1/(b**2 + 1e-10)) +1e-10 to avoid ZeroDivizionError because b**2 will be >= 0
		for (size_t i=0; i < A->len; i++) {
			dA->arr[i] += (dY->arr[i]/B->arr[i]);
			dB->arr[i] -= dY->arr[i] * A->arr[i] / (pow(B->arr[i], 2)+1e-10);
		}
	} else if (op == op_pow) {	//	y = a ^ b
		//y = a^b where (da = dy*b*a^(b-1)) and (db = dy*y*ln(a))
		for (size_t i=0; i < Y->arr[i]; i++) {
			dA->arr[i] += dY->arr[i]*B->arr[i]*pow(A->arr[i], B->arr[i]-1);
			dB->arr[i] += dY->arr[i]*Y->arr[i]*log(A->arr[i]);
		}
	}
};

void diter(Ts* Y, Ts* A, Ts* dY, Ts* dA, Op op) {
	size_t tmp_double0, tmp_double1;
	if (op == op_add || op == op_sub) {
		for (size_t i=0; i < A->len; i++)
			dA->arr[i] += op(0, dY->arr[0]);
	} else if (op == op_mul) {
		//da = dy*a/y (where y->len == 1
		for (size_t i=0; i < A->len; i++) {
			dA->arr[i] += dY->arr[0]*A->arr[i]/Y->arr[0];
		}
	} else if (op == op_div) {	//div
		//y = x0 / (x1 / (x2/ x3 ...))
		tmp_double0 = Y->arr[0];
		tmp_double1 = dY->arr[0];
		for (int i=A->len-1; i >= 0; i--) {
			tmp_double0 *= A->arr[i];
			dA->arr[i] += -tmp_double0*tmp_double1/pow(A->arr[i], 2);
			tmp_double1 /= A->arr[i];
		}
	} else if (op == op_pow) { //pow
		tmp_double0 = Y->arr[0];
		tmp_double1 = 1.0;
		// y = a**b
		for (int i=A->len-1; i >= 0; i--) {
			dA->arr[i] += -tmp_double1*tmp_double0*log(pow(tmp_double0, 1/A->arr[i]));	//e*y*log(y**(1/b))
			tmp_double0 = pow(tmp_double0, 1/A->arr[i]);
			tmp_double1 *= A->arr[i] * pow(tmp_double0, A->arr[i]-1);
		}
	}
};

void dtranspose(Ts* dY, Ts* dA) {
	Ts* tmp = transpose(dY);
	for (size_t i=0; i < dA->len; i++) dA->arr[i] += tmp->arr[i];
	freets(tmp);
};

/*
def convmat(arr, kernel):
	bordx, bordy = int((len(kernel)-1)/2), int((len(kernel[0])-1)/2)	#bord du kernel en x et y
	frmx, tox = bordx, len(arr)-bordx
	frmy, toy = bordy, len(arr[0])-bordy
	ret = [[0 for _ in range(frmy, toy)] for _ in range(frmx, tox)]
	print(ret)
	for i in range(frmx, tox):
		for j in range(frmy, toy):
			for k in range(len(kernel)):
				for l in range(len(kernel[0])):
					ret[i-frmx][j-frmy] += arr[i+k-bordx][j+l-bordy] * kernel[k][l]
	return ret
convmat([[1,2,3],[1,2,3],[1,2,3]], [[1,2,3],[1,2,3],[1,2,3]]) => 42
*/
/*
Algo Kconvl:	for Y = A | K
for pixel in Y:
	pixel = sum(A.pix[pixel.cord].extend((K.sh[0]-1)/2) * K)

Algo convl:		for Y = A #x B
for pixel in Y:
	pixel = sum()
*/

Ts* Kconvl(Ts *a, Ts *kernel) {
	Ts* ret;
	if (a->deep != kernel->deep) {
		printf("Can't make convultion because not same deep %li != %li\n", a->deep, kernel->deep);
		exit(1);
	} else {
		size_t i,pos;
		#define deep a->deep
		//Borders
		size_t border_sizes[deep];	//borders of the kernel
		for (i=0; i < deep; i++) border_sizes[i] = (kernel->shape[i]-1)/2;
		//From
		size_t from[deep];	//like init or iters
		for (i=0; i < deep; i++) from[i] = border_sizes[i];
		//TO
		size_t to[deep];	//like max
		for (i=0; i < deep; i++) to[i] = a->shape[i] - border_sizes[i];
		//Shape
		size_t len = 1;
		size_t shape[deep];
		for (i=0; i < deep; i++) {
			shape[i] = to[i] - from[i];
			len *= shape[i];
		}
		//Arr
		double arr[len];
		//Kernel bars
		size_t kfrom[deep];size_t kto[deep];
		//
		while (from[deep-1] < to[deep-1]) {
			//dot of kernel and selected area from a
			//restart kfrom and kto
			for (i=0; i < deep; i++) {
				kfrom[i] = 0;
				kto[i] = kernel->shape[i];
			}
			//
			while (kfrom[deep-1] < kto[deep-1]) {
				//arr pos
				for (i=0; i < deep; i++) from[i] -= border_sizes[i];
				pos = realpos(a, from);	//just arr[i-1][j-1] where 1 is border size
				for (i=0; i < deep; i++) from[i] += border_sizes[i];
				arr[pos] += a->arr[realpos(a, from)] * kernel->arr[realpos(kernel, kfrom)];
				//update
				printf("%li %li %li %li\n", kfrom[0], kfrom[1], kto[0], kto[1]);
				for (i=0; i < deep; i++) {
					if (++kfrom[i] <= kto[i]) break;
					else kfrom[i] = 0;
				}
			}
			//Update 'from' bars or a area selected
			for (i=0; i < deep; i++) {
				if (++from[i] < to[i]) break;
				else from[i] = 0;
			}
		}
		//
		ret = mkts(deep, shape, arr, 0);
		//
		#undef deep
	}
	return ret;
};

Ts* convl(Ts *a, Ts *b, size_t x) {

};

Ts* convultion(Ts *a, Ts *kernel) {
	Ts* ret;
	if (a->deep != kernel->deep) {
		printf("Can't make convultion because not same deep %li != %li\n", a->deep, kernel->deep);
		exit(1);
	} else {
		size_t i,pos;
		#define deep a->deep
		//Borders
		size_t border_sizes[deep];	//borders of the kernel
		for (i=0; i < deep; i++) border_sizes[i] = (kernel->shape[i]-1)/2;
		//From
		size_t from[deep];	//like init or iters
		for (i=0; i < deep; i++) from[i] = border_sizes[i];
		//TO
		size_t to[deep];	//like max
		for (i=0; i < deep; i++) to[i] = a->shape[i] - border_sizes[i];
		//Shape
		size_t len = 1;
		size_t shape[deep];
		for (i=0; i < deep; i++) {
			shape[i] = to[i] - from[i];
			len *= shape[i];
		}
		//Arr
		double arr[len];
		//Kernel bars
		size_t kfrom[deep];size_t kto[deep];
		//
		while (from[deep-1] < to[deep-1]) {
			//dot of kernel and selected area from a
			//restart kfrom and kto
			for (i=0; i < deep; i++) {
				kfrom[i] = 0;
				kto[i] = kernel->shape[i];
			}
			//
			while (kfrom[deep-1] < kto[deep-1]) {
				//arr pos
				for (i=0; i < deep; i++) from[i] -= border_sizes[i];
				pos = realpos(a, from);	//just arr[i-1][j-1] where 1 is border size
				for (i=0; i < deep; i++) from[i] += border_sizes[i];
				arr[pos] += a->arr[realpos(a, from)] * kernel->arr[realpos(kernel, kfrom)];
				//update
				printf("%li %li %li %li\n", kfrom[0], kfrom[1], kto[0], kto[1]);
				for (i=0; i < deep; i++) {
					if (++kfrom[i] <= kto[i]) break;
					else kfrom[i] = 0;
				}
			}
			//Update 'from' bars or a area selected
			for (i=0; i < deep; i++) {
				if (++from[i] < to[i]) break;
				else from[i] = 0;
			}
		}
		//
		ret = mkts(deep, shape, arr, 0);
		//
		#undef deep
	}
	return ret;
};

void dconvultion(Ts *dY, Ts *a, Ts *kernel, Ts *dA, Ts *dkernel) {
	if (a->deep != kernel->deep) {
		printf("Can't make convultion because not same deep %li != %li\n", a->deep, kernel->deep);
		exit(1);
	} else {
		size_t i,pos;
		#define deep a->deep
		//Borders
		size_t border_sizes[deep];	//borders of the kernel
		for (i=0; i < deep; i++) border_sizes[i] = (kernel->shape[i]-1)/2;
		//From
		size_t from[deep];	//like init or iters
		for (i=0; i < deep; i++) from[i] = border_sizes[i];
		//TO
		size_t to[deep];	//like max
		for (i=0; i < deep; i++) from[i] = a->shape[i] - border_sizes[i];
		//Shape
		size_t len = 1;
		size_t shape[deep];
		for (i=0; i < deep; i++) {
			shape[i] = to[i] - from[i];
			len *= shape[i];
		}
		//Arr
		double arr[len];
		//Kernel bars
		size_t kfrom[deep];size_t kto[deep];
		//
		while (from[deep-1] < to[deep-1]) {
			//dot of kernel and selected area from a
			//restart kfrom and kto
			for (i=0; i < deep;i++) {
				kfrom[i] = 0;
				kto[i] = kernel->shape[i]-1;
			}
			//
			while (kfrom[deep-1] < kto[deep-1]) {
				//arr pos
				for (i=0; i < deep; i++) from[i] -= border_sizes[i];
				pos = realpos(a, from);	//just arr[i-1][j-1] where 1 is border size
				for (i=0; i < deep; i++) from[i] += border_sizes[i];
				//===========
				//arr[pos] += a->arr[realpos(a, from)] * kernel->arr[realpos(kernel, kfrom)];
				dA->arr[realpos(a, from)] += dY->arr[pos] * kernel->arr[realpos(kernel, kfrom)];
				dkernel->arr[realpos(kernel, kfrom)] += dY->arr[pos] * a->arr[realpos(a, from)];
				//===========
				//update
				for (i=0; i < deep; i++) {
					if (++kfrom[i] < kto[i]) break;
					else kfrom[i] = 0;
				}
			}
			//Update 'from' bars or a area selected
			for (i=0; i < deep; i++) {
				if (++from[i] < to[i]) break;
				else from[i] = 0;
			}
		}
		//
		#undef deep
	}
};

Ts* pool(Ts *a, PoolFn fn, size_t *block_size) {
	(void*)a;
	(void*)fn;
	return a;
};

void dpool(Ts *a, Ts *dy, Ts *da, PoolFn fn) {

};