from .etc import *

arr	= lambda len,rnd=False: [(0.0 if not rnd else random.uniform(-1, 1)) for _ in range(len)]
#struct: [((2,1,),'f'), ...]
def fullcnd(*struct, rnd=True, rnn=False):	#fc:'fully connected' (tensors)
	model = {
		'code' : [],
		'mats' : [
			[[*struct[0][0],1], arr(mul(struct[0][0])), 0], #$0 input
			[[*struct[0][0],1], arr(mul(struct[0][0])), 0], #$1=$0+$bias
			[[*struct[0][0],1], arr(mul(struct[0][0]),rnd=rnd), 1]	#$bias
		],
		'inputs' : [0],
		'outputs' : [None]	#Will be set at end
	}
	#Adding bias to input
	model['code'] += cmd(1,0, 0,0, 2,0, '+')
	#
	pre_n = struct[0][0]
	first_lay = True
	for shp,f in struct[1:]:
		c = len(model['mats'])	#old mats lenght
		#==========Tensors==========
		model['mats'] += [
			*[[[*shp, 1], arr(mul(shp)), 0] for _ in range(3 + (0,2)[rnn])],		#($0,$1 dropout),$2,($3,$4 rnn),$5,$6,($7 dropout)
			[[shp[0],pre_n[0],*shp[2:]], arr(mul([*shp[0:1],pre_n[0],*shp[2:]]),rnd=rnd), 1],	#$w0	 weight $w0	prev_lay,this lay
		]
		if rnn:
			model['mats'] += [[[shp[0],shp[0],*shp[2:]], arr(mul([*shp,shp[0]]),rnd=rnd), 1]]	#$w1	 weight $w1 this,this
		model['mats'] += [[[*shp, 1], arr(mul(shp),rnd), 1]]	#$b adding bias
		#===========CODE============
		model['code'] += cmd(
			c,0,
			(1 if first_lay else c-3-(0,1)[rnn]),0,
			c+3+(0,2)[rnn],0,
			'@')
		if rnn:
			model['code'] += [
				*cmd(c+1,0, c+4,1, c+6,0, '@'),	#($3 = $out[-1] @ $w1) if rnn	history
				*cmd(c+2,0, c,0, c+1,0, '+'),	#($4 = $2 + $3) if rnn
			]
		model['code'] += cmd(#bias
			c+1+(0,2)[rnn],0,
			c+0+(0,2)[rnn],0,
			c+4+(0,3)[rnn],0, '+')
		model['code'] += cmd(#activation
			c+2+(0,2)[rnn],0,
			c+1+(0,2)[rnn],0,
			funcs.index(f), ')')

		first_lay = False
		pre_n = shp

	model['outputs'][0] = len(model['mats'])-3-int(rnn)
	
	return model