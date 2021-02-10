size_t realpos(Ts *ts, size_t *indexs) {
	size_t ret=0;
	for (size_t i=0; i < ts->deep; i++)
		ret += indexs[i]*ts->_real_sizes[i];
	return ret;
};

size_t* indexspos(Ts* ts, size_t pos) {
	size_t *indexs = (size_t*)malloc(ts->deep * sizeof(size_t));
	size_t tmp;
	//
	for (int i=ts->deep-1; i >= 0; i--) {
		//[0;10;20;30](real_size=10), si pos=24; (24%10)=4 => ((24-4)=20)/10 = 2 => pos=2
		tmp = pos - (pos % ts->_real_sizes[i]);	// it's the 20 (not 24)
		indexs[i] = tmp/ts->_real_sizes[i];
		pos -= tmp;	//tmp = indexs[i]*ts->_real_sizes[i]
	}
	//
	return indexs;
};

double getv(Ts* ts, size_t *pos) {
	return GTSPOS(ts, realpos(ts, pos));
};

void setv(Ts* ts, size_t *pos, double v) {
	STSPOS(ts, realpos(ts, pos)) = v;
};

void addv(Ts* ts, size_t *pos, double v) {
	STSPOS(ts, realpos(ts, pos)) += v;
};

void filltslist(Ts *ts, double *value) {
	for (size_t i=0; i < ts->len; i++) ts->arr[i] = value[i];
};

void fillts(Ts *ts, double value) {
	for (size_t i=0; i < ts->len; i++) ts->arr[i] = value;
};

void noisets(Ts *ts) {
	for (size_t i=0; i < ts->len; i++) ts->arr[i] = rand_double();
};

void noisedrop(Ts *ts) {
	for (size_t i=0; i < ts->len; i++)
		ts->droped_out[i] = rand() & 1;
};

void setdrop(Ts *ts, bool v) {
	for (size_t i=0; i < ts->len; i++)
		ts->droped_out[i] = v;
};