//This is x1.5 faster than using iterator struct and next 
bool update(size_t *indexs, size_t *max, size_t n) {
	for (size_t i=0; i < n; i++) {	//size_t is faster than int. Using updata don't instead classic while loop isn't realy afecting time
		indexs[i]++;					//Update i cursor
		if (indexs[i] != max[i]) break;	//If undex max : go to next update call
		else if (i == n-1) return 0;	//Else : Overflow of current cursor. If last => end of loop
		else indexs[i] = 0;				//Last case : 
	}
	return 1;	//Loop isn't ended, can next update()
};	//To use update(), creat indexs and fill by 0, set max, then : while (updata(i,max,n)) {};