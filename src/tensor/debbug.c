void debbug_ts(Ts *ts) {
	//Printing Infos
	printf("\033[95mDebbug Tensor\033[0m\n");
	printf("Type:%i Deep:%li, Shape:(", ts->type, ts->deep);
	for (size_t i=0; i < ts->deep; i++) printf("%li,", ts->shape[i]);
	printf("), len:%li, real_sizes:(", ts->len);
	for (size_t i=0; i < ts->deep; i++) printf("%li,", ts->_real_sizes[i]);
	printf(") pointer ts:%p\n", (void *)ts);
	//
	for (size_t i=0; i < ts->len; i++) printf("%.5g|", ts->arr[i]);
	printf("\n");
	printf("More pointers: shape ptr:%p real_sizes ptr:%p\n", (void*)ts->shape, (void*)ts->_real_sizes);
	printf("\033[95mEnd Debbug\033[0m\n");
};