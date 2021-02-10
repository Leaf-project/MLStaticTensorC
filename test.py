from lib.neurones import InpNeu, Neu, OutNeu	#Neu is 1st order neurone : f(a*x**1 + b)
from lib.network import Network, Link
from lib.data import loaddata
from lib.simple import SimpleTrain, SimpleShow

inputs = [InpNeu(), InpNeu()]
layer = [Neu('tanh'), Neu('logistic'), Neu('logistic')]
outputs = [OutNeu('logistic')]

xor_net = Network(
	inputs=inputs,
	outputs=outputs,
	links=[
		Link(inputs[0],layer[0]),	#defaul link is : out = 1*inp + 0
		Link(inputs[0],layer[1]),
		Link(inputs[0],layer[2]),
		#
		Link(inputs[1],layer[0]),
		Link(inputs[1],layer[1]),
		Link(inputs[1],layer[2]),
		#
		Link(layer[0],out[0]),
		Link(layer[1],out[0]),
		Link(layer[2],out[0]),
	]
)

xor_data = loaddata('path/to/csv-file')

SimpleTrain(xor_net, xor_data, time='20s', target_error=1e4)
SimpleShow(xor_net, xor_data)

############### Train Btc ###############

from lib.neurones import InpNeu, OutNeu, Neu
from lib.network import Network, Patern, Link
from lib.server import Server
from lib.data import loaddata

inputs = [InpNeu(name='BrutBtc'), InpNeu(name='Rsi'), InpNeu(name='VolumBtc'), InpNeu(name='VolumUsdt')]
outputs = [OutNeu('bin', name='Buy or cell')]

btc_net = Network(
	inputs=inputs,
	outputs=outputs,
	links=[
		Link(Patern(
			inputs[0],
			,
			'pyramid', (2,5,2,10)
		), ),
	]
)