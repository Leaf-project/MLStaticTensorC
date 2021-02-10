/*
In this file no need for STs because only error that can occur is core dumped or something like this.
There is no condition-argument error
*/

void _cmp_real_sizes(Ts *ts) {
	ts->_real_sizes[0] = 1;
	for (size_t i=1; i < ts->deep; i++)
		ts->_real_sizes[i] = ts->_real_sizes[i-1]*ts->shape[i-1];
};

Ts* mkts(size_t deep, size_t *shape, double *arr, size_t *ptrs, float importance, char type) {
	size_t i;
	//all null pointers arrays will be set to 0 (arr, ptrs)
	Ts *ts = (Ts*)malloc(sizeof(Ts));
	//
	ts->deep = deep;
	ts->shape = (size_t*)malloc(deep*sizeof(size_t));
	ts->_real_sizes = (size_t*)malloc(deep*sizeof(size_t));
	memcpy(ts->shape, shape, deep*sizeof(size_t));
	_cmp_real_sizes(ts);
	//
	ts->len = shape[deep-1]*ts->_real_sizes[deep-1];
	ts->arr = (double*)malloc(ts->len * sizeof(double));
	if (arr == NULL) memset(ts->arr, 0, ts->len*sizeof(double));
	else memcpy(ts->arr, arr, ts->len*sizeof(double));
	//
	ts->pointers = (size_t*)malloc(ts->len * sizeof(size_t));
	if (ptrs == NULL) for (i=0; i < ts->len; i++) ts->pointers[i] = i;
	else memcpy(ts->pointers, ptrs, ts->len*sizeof(size_t));
	//
	ts->droped_out = (bool*)malloc(ts->len * sizeof(bool));
	memset(ts->droped_out, false, ts->len);
	//
	ts->importance = importance;
	//
	ts->type = type;
	return ts;
};

Ts* cpyts(Ts *ts) {
	Ts *ret = (Ts*)malloc(sizeof(Ts));
	//
	ret->deep = ts->deep;
	//
	ret->shape = (size_t*)malloc(ret->deep * sizeof(size_t));
	//
	ret->_real_sizes = (size_t*)malloc(ret->deep * sizeof(size_t));
	//
	for (size_t i=0; i < ret->deep; i++) {
		ret->shape[i] = ts->shape[i];
		ret->_real_sizes[i] = ts->_real_sizes[i];
	}
	//
	ret->len = ret->shape[ret->deep-1]*ret->_real_sizes[ret->deep-1];
	ret->arr = (double*)malloc(ret->len * sizeof(double));
	for (size_t i=0; i < ret->len; i++)
		ret->arr[i] = ts->arr[i];
	//
	ret->pointers = (size_t*)malloc(ret->len * sizeof(size_t));
	for (size_t i=0; i < ret->len; i++)
		ret->pointers[i] = ts->pointers[i];
	//
	ret->droped_out = (bool*)malloc(ret->len * sizeof(bool));
	for (size_t i=0; i < ret->len; i++)
		ret->droped_out[i] = ts->droped_out[i];
	//
	ret->importance = ts->importance;
	//
	ret->type = ts->type;
	return ret;
};

void freets(Ts *ts) {
	free(ts->droped_out);
	free(ts->pointers);
	free(ts->arr);
	free(ts->shape);
	free(ts->_real_sizes);
	free(ts);
};

Ts* takefromts(Ts* ts, size_t* from, size_t* to) {
	size_t i;
	//
	size_t shape[ts->deep];
	for (size_t i=0; i < ts->deep; i++)
		shape[i] = to[i] - from[i];
	//
	Ts* ret = mkts(ts->deep, shape, 0, 0, 1, 0);
	size_t iters[ts->deep];memset(iters, 0, ts->deep*sizeof(size_t));
	size_t ts_iters[ts->deep];memcpy(ts_iters, from, ts->deep*sizeof(size_t));
	//
	do {
		for (i=0; i < ts->deep; i++) {
			iters[i] = ts_iters[i] - from[i];
		}
		setv(ret, iters, getv(ts, ts_iters));
	} while(update(ts_iters, to, ts->deep));// && update(iters, ret->shape, ret->deep));
	//
	return ret;
};

Ts* extend(Ts *ts, size_t* ns, double v) {
	size_t i;
	bool ok;
	double tmp;
	//
	size_t new_shape[ts->deep];
	for (i=0; i < ts->deep; i++)
		new_shape[i] = ts->shape[i] + 2*ns[i];
	//TODO
	Ts* ret = mkts(ts->deep, new_shape, 0, 0, 1, 0);	//arr,ptrs,importance,type
	//
	size_t iters[ts->deep];
	memset(iters, 0, ts->deep*sizeof(size_t));
	//
	do {
		ok = false;	//is it border ?
		for (i=0; i < ts->deep; i++) {
			if (iters[i] < ns[i] || iters[i] >= new_shape[i] - ns[i]) {
				ok = true;
				break;
			}
		}
		//
		if (ok) setv(ret, iters, v);
		else {//ret->pointers[realpos(ret, pos)] = ts->pointers[] - ;  IDK how to do.
			subarr(iters, ns, ts->deep);
			tmp = getv(ts, iters);
			sumarr(iters, ns, ts->deep);
			setv(ret, iters, tmp);
		}
	} while (update(iters, new_shape, ts->deep));
	//
	return ret;
};