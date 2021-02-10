void writets(Ts *ts, FILE *fp) {
	fwrite(&ts->deep, sizeof(size_t), 1, fp);
	fwrite(ts->shape, sizeof(size_t), ts->deep, fp);
	fwrite(ts->arr, sizeof(double), ts->len, fp);
	fwrite(ts->pointers, sizeof(size_t), ts->len, fp);
	fwrite(ts->droped_out, sizeof(bool), ts->len, fp);
	fwrite(&ts->importance, sizeof(float), 1, fp);
	fwrite(&ts->type, sizeof(char), 1, fp);
};

/*void rsa_writets(Ts *ts, FILE *fp, size_t key) {

};*/

Ts* readts(FILE *fp) {
	Ts* ret = (Ts*)malloc(sizeof(Ts));
	//
	fread(&ret->deep, sizeof(size_t), 1, fp);
	//
	ret->shape = (size_t*)malloc(ret->deep * sizeof(size_t));
	fread(ret->shape, sizeof(size_t), ret->deep, fp);
	//
	ret->_real_sizes = (size_t*)malloc(ret->deep * sizeof(size_t));
	_cmp_real_sizes(ret);
	//
	ret->len = ret->shape[ret->deep-1] * ret->_real_sizes[ret->deep-1];
	ret->arr = (double*)malloc(ret->len * sizeof(double));
	fread(ret->arr, sizeof(double), ret->len, fp);
	//Pointers
	ret->pointers = (size_t*)malloc(ret->len * sizeof(size_t));
	fread(ret->pointers, sizeof(size_t), ret->len, fp);
	//
	ret->droped_out = (bool*)malloc(ret->len * sizeof(bool));
	fread(ret->droped_out, sizeof(bool), ret->len, fp);
	//
	fread(&ret->importance, sizeof(float), 1, fp);
	//
	fread(&ret->type, sizeof(char), 1, fp);
	//
	return ret;
};

/*void rsa_readts(FILE *fp, size_t key) {

};*/