#pragma once

typedef struct {	//Return Tensor outout and if there is panic info
	Ts* ret;
	bool isok;	//If all correct 1, else 0 for panic to free all malloced memory. Kind of manual garbage collector. 
} STs;

/*RSA algorithm
===========Generation==============
Random (p,q) a couple of large prime numers p!=q by convention p > q
	p = randprime(lenght=1024)
	while (q=randprime(lenght=1024) == p) : {}
	return min(p,q), max(p,q)
n = p*q
phy = (p-1)*(q-1)
e as gcd(phy, e) == 1; 1 < e < phy
d*e % phy = 1  <=>  de=1+k*phy <=> d=(1+k*phy)/e 	(de congrue a 1 modulo phy)
Pub : {e,n}
Priv: {d,n}
==========Encription==============
M < n
C = M**e % n
==========Decription==============
M = C**d % n

typedef size_t RSA_TYPE;

typedef struct {
	RSA_TYPE n, e, d;
} Key;

RSA_TYPE gcd(RSA_TYPE a, RSA_TYPE b) {
	//pgcd(a, b)	ou a < b
	//b = a*k0 + r0
	//k0 = r0*k1 + r1
	//k1 = r1*k2 + r2
	//...
	//kn-1 = rn-1*kn + rn 	rn = 0
	
	RSA_TYPE c;
	while (a != 0) {	//b = a*k + r
		c = a;			//a, b = a%b, a
		a = a % b;		//
		b = a;			//
	}
	return b;
}

Key mkkey(size_t lenght, size_t seed) {	//generating random key

};

RSA_TYPE encrypt(RSA_TYPE M, RSA_TYPE e, RSA_TYPE n) {
	return 1;//pow(M, e) % n;
};

RSA_TYPE decript(RSA_TYPE C, RSA_TYPE d, RSA_TYPE n) {
	return 1;//pow(C, d) % n;
};*/