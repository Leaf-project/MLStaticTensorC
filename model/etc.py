import struct
import random

funcs = [
	'identity', 'bin_step', 'Logistic', 
	'Tanh', 'ReLU', 'GELU', 
	'SoftPlus', 'ELU', 'SELU', 
	'Leaky_ReLU', 'Softsign', 'SQNL',
	'Bent_Ind', 'Gauss', 
	'SQ_RBF', 'SoftMax', 'Maxout'
]

to_size_t = lambda *l: struct.pack('N'*len(l), l)

linef = lambda op: (lambda y,a,b:to_size_t(op,*y,*a,*b))	#y = a (op) b, y:(id,t), a:(id,t), b:(id,t)

cmds = {
	'+' : linef(0),
	'-' : linef(1),
	'*' : linef(2),
	'/' : linef(3),
	'^' : linef(4),
	'@' : linef(5),
}

def mul(lst):
	a = 1
	for i in lst: a *= i
	return a

def mkts(shape, arr, type):
	return struct.pack(
		'N'*(1 + len(shape))+'d'*len(arr)+'B', 
		len(shape), *shape, *arr, type)

def readmdl(file):
	model = {'code':'', 'mats':[], 'inputs':[], 'outputs':[]}

	with open(file, 'rb') as co: bins = co.read()

	#read info
	nb_vars,inputs,outputs = struct.unpack('NNN', bytes(bins[:8*3]))
	bins = bins[8*3:]
	model['inputs'] = struct.unpack('N'*inputs, bytes(bins[:8*inputs]))
	bins = bins[8*inputs:]
	model['outputs'] = struct.unpack('N'*outputs, bytes(bins[:8*outputs]))
	bins = bins[8*outputs:]

	#read first wave of vars (avoid other times)
	for t in range(times):
		for i in range(nb_vars):
			#read shape
			deep = struct.unpack('N', bytes(bins[:8]))[0]
			bins = bins[8:]
			shape = struct.unpack('N'*deep, bytes(bins[:8*deep]))
			bins = bins[8*deep:]
			#read arr
			len_arr = mul(shape)
			arr = struct.unpack('d'*len_arr, bytes(bins[:8*len_arr]))
			bins = bins[8*len_arr:]
			#read type
			type_ = struct.unpack('B', bytes((bins[0],)))[0]
			bins = bins[1:]

			if t==0: model['mats'] += [[shape, arr, type_]]
	
	#don't read gras (avoid other times)
	for t in range(times):
		for i in range(nb_vars):
			#read shape
			deep = struct.unpack('N', bytes(bins[:8]))[0]
			bins = bins[8:]
			shape = struct.unpack('N'*deep, bytes(bins[:8*deep]))
			bins = bins[8*deep:]
			#read arr
			len_arr = mul(shape)
			bins = bins[8*len_arr:]
			#read type
			bins = bins[1:]

	#read code
	codelen = struct.unpack('N', bytes(bins[:8]))[0]
	bins = bins[8:]
	model['code'] = struct.unpack('B'*codelen, bytes(bins[:codelen]))
	return model

def writemdl(model, filename):
	#print(model)
	with open(filename, "wb") as co:
		#times, nb_vars, inputs, outputs
		co.write(struct.pack('NNN', len(model['mats']), len(model['inputs']), len(model['outputs'])))
		co.write(struct.pack('N'*(len(model['inputs'])+len(model['outputs'])), *model['inputs'], *model['outputs']))
		#=========Vars=========
		for shape, arr, typ in model['mats']:
			#Deep
			co.write(struct.pack('N', len(shape)))
			#Shape
			co.write(struct.pack('N'*len(shape), *shape))
			#Arr
			co.write(struct.pack('d'*len(arr), *arr))
			#Type
			co.write(struct.pack('B', typ))
		#=========Grads=========
		for shape, arr, typ in model['mats']:
			#Deep
			co.write(struct.pack('N', len(shape)))
			#Shape
			co.write(struct.pack('N'*len(shape), *shape))
			#Arr
			co.write(struct.pack('d'*len(arr), *([0.0 for _ in range(len(arr))])))
			#Type
			co.write(struct.pack('B', typ))
		#=========Code==========
		co.write(struct.pack('N', len(model['code'])))
		co.write(struct.pack('B'*len(model['code']), *model['code']))
