/*
for X in Y,A,B
All dX->arr[i] += value
+= and not = because A can be used mutiple times:
y0 = 3*a
y1 = 2*a
y2 = a - 4
There a is used 3 times. By using '=' grad(A) will take only grad(y2)
and not grad(y2) + grad(y1)*2 + grad(y0)*3
===================================
Pas le temps de faire de meilleur fonctions plus opti.
*/

bool _same_shapes(Ts *a, Ts *b) {
	if (a->deep != b->deep) return 0;
	for (size_t i=0; i < a->deep; i++)
		if (a->shape[i] != b->shape[i])
			return 0;
	return 1;
};

static const char* lineopstr = "tensor_operations.c:lineop";

STs* lineop(Ts *A, Ts *B, Op op) {
	STs* ret = (STs*)malloc(sizeof(STs));
	//
	ret->ret = cpyts(A);
	if (_same_shapes(A, B)) {
		for (size_t i=0; i < A->len; i++)
			STSPOS(ret->ret, i) = op(GTSPOS(A, i), GTSPOS(B, i));
		//
	} else if (A->len == 1) {
		double x = GTSPOS(A, 0);
		for (size_t i=0; i < B->len; i++)
			STSPOS(ret->ret, i) = op(x, GTSPOS(B, i));
		//
	} else if (B->len == 1) {
		double x = GTSPOS(B, 0);
		for (size_t i=0; i < A->len; i++)
			STSPOS(ret->ret, i) = op(GTSPOS(A, i), x);
		//
	} else {
		_Msg(ERROR, lineopstr, "Operator not same shape.");
		freets(ret->ret);
		goto ERROR;
	}
	ret->isok = true;
	return ret;
ERROR:
	ret->isok = false;
	return ret;
};

static const char* iterstr = "tensor_operations.c:iter";

STs* iter(Ts *ts, Op op) {
	STs* ret = (STs*)malloc(sizeof(STs));
	//
	size_t shape[] = {1};
	size_t pointers[] = {0};
	double arr[1];
	//
	size_t i=0;
	//Initialize operation lists
	if (op == op_add || op == op_sub) arr[0] = 0.0;
	else if (op == op_mul || op == op_div || op == op_pow) {
		arr[0] = GTSPOS(ts, 0);
		i = 1;
	}
	//Make operations (from i=1)
	for (; i < ts->len; i++) arr[0] = op(arr[0], GTSPOS(ts, i));
	//
	ret->ret = mkts(1, shape, arr, pointers, 1, 0);
	ret->isok = true;
	return ret;
};

void _inverse_lst(size_t *lst, size_t *out, size_t n) {
	for (size_t i=0; i < n; i++)
		out[i] = lst[n-i-1];
};

static const char* transposestr = "tensor_operations.c:transpose";

STs* transpose(Ts *ts) {
	STs* ret = (STs*)malloc(sizeof(STs));
	//
	#define Deep ts->deep
	size_t i;
	//Inverse shape
	size_t inv_shape[Deep];
	_inverse_lst(ts->shape, inv_shape, Deep);
	ret->ret = mkts(Deep, inv_shape, 0, 0, 1, 0);
	//
	size_t indexs[Deep];for (i=0;i < Deep;i++) indexs[i]=0;
	size_t inversed[Deep];
	//
	size_t *ptr_indexs;
	size_t inv_ptr_indexs[Deep];
	//
	do {
		//Inverse
		_inverse_lst(indexs, inversed, Deep);
		//Replace
		setv(ret->ret, inversed, ts->arr[realpos(ts, indexs)]);//getv(ts, indexs));
		//
		ptr_indexs = indexspos(ts, ts->pointers[realpos(ts, indexs)]);
		_inverse_lst(ptr_indexs, inv_ptr_indexs, Deep);
		//
		ret->ret->pointers[realpos(ret->ret, inversed)] = realpos(ret->ret, inv_ptr_indexs);
	} while (update(indexs, ts->shape, Deep));
	//
	ret->isok = true;
	return ret;
	#undef Deep
};

bool _does_dot_ok(Ts* a, Ts* b) {
	//After deep checking
	if (a->shape[0] != b->shape[1]) return 0;
	if (a->deep > 2) for (size_t i=2; i < a->deep; i++) if (a->shape[i] != b->shape[i]) return 0;
	return 1;
};

static const char* dotstr = "tensor_operations.c:dot";

STs* dot(Ts *A, Ts *B) {
	STs* ret = (STs*)malloc(sizeof(STs));
	//
	if (A->deep == 1 && B->deep == 1) {	//Special Vector case
		//Spetial functions
		if (A->shape[0] == B->shape[0]) {
			size_t shape[] = {1};
			double arr[1] = {0.0};
			size_t ptr[] = {0};	//point to 0'th pixel
			ret->ret = mkts(1, shape, arr, ptr, 1.0, 0);	//importance set to 1 but not usefull
			for (size_t i=0; i < A->len; i++) STSPOS(ret->ret,0) += GTSPOS(A,i) * GTSPOS(B,i);
		} else {
			_Msg(ERROR, dotstr, "a->deep == b->deep == 1 but shapes[0] != shapes[1] %li != %li",  A->shape[0], B->shape[1]);
			goto ERROR;
		}
	} else if (A->deep != B->deep) {
		_Msg(ERROR, dotstr, "Not sames deep : %li != %li", A->deep, B->deep);
		goto ERROR;
	} else if (_does_dot_ok(A, B)) {
		#define Deep A->deep
		size_t i,c;	//i - just tmpt iterator, c - make swap values a,b=b,a (c=a,a=b,b=c)
		double a,b;	//a,b tmpt double from A and B
		//Set up shape (ex with deep=3, {x,y,z}@{a,b,c} = {a,x,c})
		size_t shape[Deep];
		for (i=0; i < Deep; i++) shape[i] = B->shape[i];
		shape[1] = A->shape[1];
		//
		ret->ret = mkts(Deep, shape, 0, 0, 1, 0);
		//
		size_t index[Deep];
		memset(index, 0, Deep*sizeof(size_t));
		//
		do {	//For each pixel of ret[index];
			//Make "normal" 2D pseudo-vector product
			for (i=0; i < A->shape[0]; i++) {
				//Set a = A[[i] + index[1:]]
				c = index[0];
				index[0] = i;
				a = getv(A, index);
				index[0] = c;
				//Set b = B[[index[0] + [i] + index[2:]]]
				c = index[1];
				index[1] = i;
				b = getv(B, index);
				index[1] = c;
				//Set ret[index] = ret[index] + a*b;
				setv(ret->ret, index, getv(ret->ret, index) + a*b);
			}
		} while (update(index, ret->ret->shape, Deep));
		#undef Deep
	} else {
		_Msg(ERROR, dotstr, "Corret deeps but invalid combinaison of shapes a->shape[0] != b->shape[1] or not sames shapes after 2nd.");
		goto ERROR;
	}
	ret->isok = true;
	return ret;
ERROR:
	ret->isok = false;
	return ret;
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

bool _incorrect_kernel(Ts* Kernel) {
	for (size_t i=0; i < Kernel->deep; i++) {
		if (Kernel->shape[i] % 2 == 0) {	//shape[i] not in form of 2*k+1  <=> not (shape[i] - 1)/2==1  
			printf("Shape[%li] = %li\n", i, Kernel->shape[i]);
			return true;
		}
	}
	return false;
};

bool _bigger_kernel(Ts* ts, Ts* Kernel) {
	//Does shape_ts >= shape_kernel
	for (size_t i=0; i < ts->deep; i++) {
		if (ts->shape[i] < Kernel->shape[i]) {
			return true;
		}
	}
	return false;
};

static const char* Kconvlstr = "tensor_operations.c:Kconvl";

STs* Kconvl(Ts* A, Ts* Kernel, char* mod) {	//mod = 'valid', 'same' ('valid'-output shape change (no 0 adding), 'same'-output.shape == input.shape)
	#define Deep A->deep
	STs* ret = (STs*)malloc(sizeof(STs));	//Secure return (with kind of garbage collector)
	//
	if (A->deep != Kernel->deep) {
		_Msg(ERROR, Kconvlstr, "Not same deeps %li != %li", A->deep, Kernel->deep);
		goto ERROR;
	} else if (_incorrect_kernel(Kernel)){
		_Msg(ERROR, Kconvlstr, "Shapes are written as (2k), but have to be (2k + 1)");
		goto ERROR;
	} else if (_bigger_kernel(A, Kernel)) {
		_Msg(ERROR, Kconvlstr, "Too big kernel");
		goto ERROR;
	} else if (!strcmp(mod, "valid")) {
		size_t i;
		//Compute border sizes
		size_t borders[Deep];	//Each kernel shape is (2k+1), border is k
		for (i=0; i < Deep; i++) borders[i] = (Kernel->shape[i] - 1)/2;
		//Compute ret shape
		size_t shape[Deep];
		size_t len = 1;
		for (i=0; i < Deep; i++) {
			shape[i] = A->shape[i] - 2*borders[i];
			len *= shape[i];
		}
		ret->ret = mkts(Deep, shape, 0, 0, 1, 0);	//1, 0 : importance, type
		//
		size_t iters[Deep];	//output iters
		memset(iters, 0, Deep*sizeof(size_t));
		//
		size_t kernel_iters[Deep];
		//
		size_t A_iters[Deep];	//Will be recompute each time
		//
		do {
			for (i=0; i < Deep; i++) kernel_iters[i] = 0;
			//
			do {
				for (i=0; i < Deep; i++) A_iters[i] = iters[i] + kernel_iters[i];
				//
				addv(ret->ret, iters, getv(A, A_iters)*getv(Kernel, kernel_iters));
			} while(update(kernel_iters, Kernel->shape, Deep));
			//
		} while (update(iters, ret->ret->shape, Deep));
	} else if (!strcmp(mod, "same")) {
		//With shifting
		//Compute border sizes
		size_t borders[Deep];	//Each kernel shape is (2k+1), border is k
		for (size_t i=0; i < Deep; i++) borders[i] = (Kernel->shape[i] - 1)/2;
		//
		Ts* extended = extend(A, borders, 0.0);
		STs* tmp_ret = Kconvl(extended, Kernel, "valid");	//no ret->isok checking because allready did it
		//
		freets(extended);
		ret->ret = tmp_ret->ret;
		free(tmp_ret);
	} else {
		_Msg(ERROR, Kconvlstr, "Mode '%s' not recognised", mod);
		goto ERROR;
	};
	#undef Deep
	ret->isok = true;
	return ret;
ERROR:
	ret->isok = false;
	return ret;
};///##{file0,file1,file2...}

static const char* convlstr = "tensor_operations.c:convl";
STs* convl(Ts *A, Ts *B, size_t x) {
	STs* ret = (STs*)malloc(sizeof(STs));
	//Only mode aviable is 'valid'
	//
	return ret;
};

static const char* poolstr = "tensor_operations.c:pool";

STs* pool(Ts *A, PoolFn fn, size_t *block_sizes) {
	size_t i;
	STs* ret = (STs*)malloc(sizeof(STs));
	//Check if block_sizes < A->shape
	for (i=0; i < A->deep; i++) {
		if (A->shape[i] < block_sizes[i] || A->shape[i] % block_sizes[i] != 0) {
			_Msg(ERROR, poolstr, "Block bigger than input tensor");
			goto ERROR;
		}
	}
	//
	{
		Ts* taked;
		size_t shape[A->deep];
		for (i=0; i < A->deep; i++) shape[i] = (A->shape[i] / block_sizes[i]);
		//
		ret->ret = mkts(A->deep, shape, 0, 0, 1, 0);
		size_t iters[A->deep];memset(iters, 0, A->deep*sizeof(size_t));
		size_t from[A->deep], to[A->deep];
		//
		do {
			for (i=0; i < A->deep; i++) {
				from[i] = iters[i]*block_sizes[i];
				to[i] = (iters[i]+1)*block_sizes[i];
			}
			taked = takefromts(A, from, to);
			setv(ret->ret, iters, fn(taked));
			//
			freets(taked);
		} while (update(iters, ret->ret->shape, A->deep));
		//
	}
	ret->isok = true;
	return ret;
ERROR:
	ret->isok = false;
	return ret;
};

STs* pyramidals(Ts *a, Ts* W, char* mod, size_t* params) {

};

void dlineop(Ts* Y, Ts* A, Ts* B, Ts* dY, Ts* dA, Ts* dB, Op op) {
	if (op == op_add) {			//	y = a + b
		for (size_t i=0; i < A->len; i++) {
			STSPOS(dA, i) += GTSPOS(dY, i);
			STSPOS(dB, i) += GTSPOS(dY, i);
		}
	} else if (op == op_sub) {	//	y = a - b
		for (size_t i=0; i < A->len; i++) {
			STSPOS(dA, i) += GTSPOS(dY, i);
			STSPOS(dB, i) -= GTSPOS(dY, i);
		}
	} else if (op == op_mul) {	//	y = a * b
		for (size_t i=0; i < dA->len; i++) {
			STSPOS(dA, i) += (GTSPOS(dY, i) * GTSPOS(B, i));
			STSPOS(dB, i) += (GTSPOS(dY, i) * GTSPOS(A, i));
		}
	} else if (op == op_div) {	//	y = a / b
		//y = a/b where (da = dy/b) and (db = dy*a*-1/(b**2 + 1e-10)) +1e-10 to avoid ZeroDivizionError because b**2 will be >= 0
		for (size_t i=0; i < A->len; i++) {
			STSPOS(dA, i) += (GTSPOS(dY, i)/(GTSPOS(B, i)+1e-5));
			STSPOS(dB, i) -= GTSPOS(dY, i) * GTSPOS(A, i) / (pow(GTSPOS(B, i), 2) + 1e-5);	//avoid ZeroDivizionError
		}
	} else if (op == op_pow) {	//	y = a ^ b
		//y = a^b where (da = dy*b*a^(b-1)) and (db = dy*y*ln(a))
		for (size_t i=0; i < Y->arr[i]; i++) {
			STSPOS(dA, i) += GTSPOS(dY, i)*GTSPOS(B, i)*pow(GTSPOS(A, i), GTSPOS(B, i)-1);
			STSPOS(dB, i) += GTSPOS(dY, i)*GTSPOS(Y, i)*ln(GTSPOS(A, i));	//log is ln
		}
	}
};

void diter(Ts* Y, Ts* A, Ts* dY, Ts* dA, Op op) {
	size_t tmp_double0, tmp_double1;
	if (op == op_add || op == op_sub) {
		for (size_t i=0; i < A->len; i++)
			STSPOS(dA, 0) += op(0, GTSPOS(dY,0));
	} else if (op == op_mul) {
		//da = dy*a/y (where y->len == 1
		for (size_t i=0; i < A->len; i++)
			STSPOS(dA,i) += GTSPOS(dY,0)*GTSPOS(A,i)/(GTSPOS(Y,0)+1e-5);
		//
	} else if (op == op_div) {	//div
		//y = x0 / (x1 / (x2/ x3 ...))
		tmp_double0 = GTSPOS(Y,0);
		tmp_double1 = GTSPOS(dY,0);
		for (int i=A->len-1; i >= 0; i--) {
			tmp_double0 *= GTSPOS(A,i);
			STSPOS(dA, i) += -tmp_double0*tmp_double1/pow(GTSPOS(A,i), 2);
			tmp_double1 /= GTSPOS(A,i);
		}
	} else if (op == op_pow) { //pow
		tmp_double0 = GTSPOS(Y,0);
		tmp_double1 = 1.0;
		// y = a**b
		for (int i=A->len-1; i >= 0; i--) {
			STSPOS(dA, i) += -tmp_double1*tmp_double0*ln(pow(tmp_double0, 1/GTSPOS(A,i)));	//e*y*ln(y**(1/b))
			tmp_double0 = pow(tmp_double0, 1/GTSPOS(A,i));
			tmp_double1 *= GTSPOS(A,i) * pow(tmp_double0, GTSPOS(A,i)-1);
		}
	}
};

void dtranspose(Ts* dY, Ts* dA) {
	STs* tmp = transpose(dY);
	for (size_t i=0; i < dA->len; i++) dA->arr[i] += tmp->ret->arr[i];
	freets(tmp->ret);
	free(tmp);
};

void ddot(Ts *dY, Ts *A, Ts *B, Ts *dA, Ts *dB) {
	if (A->deep == 1 && B->deep == 1) {
		//Spetial function
		for (size_t i=0; i < A->len; i++) {
			STSPOS(dA,i) += GTSPOS(B,i)*GTSPOS(dY,0);
			STSPOS(dB,i) += GTSPOS(A,i)*GTSPOS(dY,0);
		}
	} else {
		#define deep A->deep
		//
		size_t c;	//c is just here to make a,b = b,a (c=a;a=b;b=c)	(Fun fact: to make a,b=b,a we can make (useless x2.5 slower) : a=a-b;b=a+b;a=-a+b)
		size_t i,pos0, pos1;
		double y;	//elm in line or col in mat_a and mat_b
		//
		size_t index[deep];
		//
		while (update(index, dY->shape, deep)) {	//for each in ret
			//Make "normal" 2D pseudo-vector product
			for (i=0; i < A->shape[0]; i++) {
				y = getv(dY, index);
				//Set a = [i] + index[1:]
				c = index[0];
				index[0] = i;
				pos0 = realpos(A, index);
				index[0] = c;
				//Set b = [index[0] + [i] + index[2:]]
				c = index[1];
				index[1] = i;
				pos1 = realpos(A, index);
				index[1] = c;
				//
				STSPOS(dA,pos0) += y * GTSPOS(B,pos1);
				STSPOS(dB,pos1) += y * GTSPOS(A,pos0);
			}
		}
		#undef deep
	}
};

void dconvultion(Ts *dy, Ts *a, Ts *kernel, Ts *da, Ts *dkernel) {

};

void dpool(Ts *a, Ts *dy, Ts *da, PoolFn fn) {

};

void dpyramidals(Ts* Y, Ts* dY, Ts *A, Ts* W, Ts *dA, Ts* dW, char* mod, size_t* params) {

};