real = lambda list: [list[x-1]/list[x] for x in range(1, len(list))]	#get %of evolution not real values

#Derivative and Anti-derivative
up = lambda list: [list[0]]+[.5 * (list[x] + list[x-1]) for x in range(1, len(list))]
down = lambda list: [0]+[list[x]-list[x-1] for x in range(1, len(list))]

#Show multiple plots show([l0,l1,l2...], [name0, name1, name2...])
def show(lists, names):
	figure, axis = plt.subplots(len(lists))
	for x,list in enumerate(lists):
		axis[x].plot(range(len(list)), list)
		axis[x].set_title(names[x])
	plt.subplots_adjust(hspace=0.8)
	plt.show()

#Indicators
rsi = lambda x:x
macd = lambda x:x