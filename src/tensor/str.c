#define STR_LINE_SHAPE "Shape:("

char *line0(size_t *shape, size_t deep) {	//First file '(shape:len)'
	char *ret = newline(1);
	strcat(ret, STR_LINE_SHAPE);
	for (size_t i=0; i < deep; i++) strcat(ret, sttoa(shape[i], "%li,"));
	strcat(ret, sttoa(mul(shape, deep), "):%li"));
	return ret;
};

char* *_get_lines(Ts *ts) {
	size_t lines = ts->len / ts->shape[0];
	char *tmp;
	char **line = (char**)malloc((lines+1) * sizeof(char*));
	size_t tmp_,new_size;
	for (size_t i=0; i < lines; i++) {
		line[i] = malloc(2);line[i][0]='|';line[i][1]='\0';
	}
	line[lines] = malloc(1);line[lines][0]='\0';
	//
	size_t lp=0;
	for (size_t i=0; i < ts->len; i++) {
		if (i % ts->shape[0] == 0 && i != 0) lp++;
		tmp = dtoa(GTSPOS(ts,i), "%.4g,");
		tmp_ = strlen(line[lp]);
		line[lp] = realloc(line[lp], (new_size=tmp_+strlen(tmp)+1));
		memcpy(line[lp]+tmp_, tmp, strlen(tmp));
		line[lp][new_size-1] = '\0';
		free(tmp);
	}
	return line;
};

char* *strts(Ts* ts) {
	size_t lines = ts->len / ts->shape[0];
	size_t shape1 = 1;
	if (ts->deep > 1) shape1 = ts->shape[1];
	size_t blocks = lines / shape1;
	//
	size_t length = (1 + lines + (blocks+1) + 1);
	char* *strs = (char**)malloc(length * sizeof(char*));
	size_t max,tmp,line_,last_sep;
	//Write shape and len
	strs[0] = line0(ts->shape, ts->deep);
	max = strlen(strs[0])+1;
	//
	char* *line = _get_lines(ts);
	//Getting max line length
	for (size_t i=0; i < lines; i++) {
		tmp = strlen(line[i]);
		if (tmp > max) {
			max = tmp+1;
		}
	}
	//Adding spaces to max
	tmp = strlen(strs[0]);
	if (max > tmp) {
		strs[0] = realloc(strs[0], max);
		memset(strs[0]+tmp, ' ', max-tmp);
		strs[0][max-1] = '\0';
	};
	//
	tmp = 0;
	line_ = 0;
	last_sep = 0;	//bool : is last while iter was separator adding ? 
	while (tmp++ != length-2) {
		strs[tmp] = newline(max);
		if (line_ % shape1 == 0 && last_sep == 0) {	//new separator ====
			memset(strs[tmp], '=', max-1);
			strs[tmp][0] = '|';
			last_sep = 1;
		} else {	//write in block
			memcpy(strs[tmp], line[line_], strlen(line[line_]));
			line_++;
			last_sep = 0;
		};
	};
	//Freeing
	freestrs(line);
	//
	strs[length-1] = newline(1);
	//
	return strs;
};

void plumts(Ts *ts) {
	char* *strs = strts(ts);
	plumstrs(strs);
	freestrs(strs);
};

void plumshape(Ts *ts) {
	printf("(");
	for (size_t i=0; i < ts->deep; i++) printf("%li,", ts->shape[i]);
	printf(")");
};

//Shell rendering scale : 232 - 255
void draw_pixel(double min, double max, double value) {
	printf("\033[48;5;%lim  \033[0m", 255 - (size_t)(23*(value - min)/(max - min)));
};

void show2Dimg(Ts *ts) {
	if (ts->deep != 2) {
		_Msg(WARRNING, "tensor_str.c:show2Dimg", "Can't render %li tensors", ts->deep);
		return;
	}
	double min=0.0, max=0.0;
	//Get max and min
	for (size_t i=0; i < ts->len; i++) {
		if (GTSPOS(ts, i) > max) max = GTSPOS(ts, i);
		else if (GTSPOS(ts, i) < min) min = GTSPOS(ts, i);
	};
	printf("\033[38;5;232;48;5;255m%.3g\033[0m \033[38;5;255;48;5;232m%.3g\033[0m\n", min, max);
	//
	for (size_t y=0; y < ts->shape[1]; y++) {
		for (size_t x=0; x < ts->shape[0]; x++) {
			draw_pixel(min, max, GTSPOS(ts, y*ts->shape[1] + x));
		}
		printf("\n");
	}
};

void showRGBimg(Ts *ts) {

};