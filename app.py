#! usr/bin/python

from tkinter import ttk
import tkinter as tk
import tkinter.filedialog as filedialog
import os
from model.etc import *
from model.fullyconn import *

#Toplevel for other tkinter windows

initialdir = os.getcwd()

def _resize_cvs(old,new,frm,to):
	with open(old, 'r') as co:
		lines = co.read().split('\n')
		if lines[-1] == '':
			del lines[-1]
	if to-frm > len(lines):
		print('_resize_cvs (line 15): to-frm > len(lines).')
		return
	with open(new, 'w') as co:
		co.write('\n'.join(lines[frm:to]))

def resize_cvs():
	root = tk.Toplevel()
	root.title('Resizing Cvs file')
	#Frames
	frame_from = tk.Frame(root)
	frame_to = tk.Frame(root)
	frame_spinbx = tk.Frame(root)
	frame_apply = tk.Frame(root)
	#frm
	frm = tk.StringVar() 
	frm.set("path/to/input file")
	frmentry = tk.Entry(frame_from, textvariable=frm, width=20)
	frmentry.pack(side=tk.LEFT)

	tk.Button(frame_from, text='brose input', command=lambda :frm.set(filedialog.askopenfilename(
		title="Open input cvs file",
		filetypes=[('cvs files','.cvs'),('all files','.*')]))).pack(side=tk.LEFT)
	#to
	to = tk.StringVar() 
	to.set("path/to/output file")
	toentry = tk.Entry(frame_to, textvariable=to, width=20)
	toentry.pack(side=tk.LEFT)

	tk.Button(frame_to, text='brose output', command=lambda :to.set(filedialog.asksaveasfilename(
		title="Open output cvs file",
		filetypes=[('cvs files','.cvs'),('all files','.*')]))).pack(side=tk.LEFT)
	#New size
	tk.Label(frame_spinbx, text='From line:').pack(side=tk.LEFT)
	sfrm = tk.Spinbox(frame_spinbx, width=10)
	sfrm.pack(side=tk.LEFT)

	tk.Label(frame_spinbx, text='To line:').pack(side=tk.LEFT)
	sto = tk.Spinbox(frame_spinbx, width=10)
	sto.pack(side=tk.LEFT)
	#Btn
	tk.Button(frame_apply, text='Resize',
		command=lambda :(_resize_cvs(frm.get(), to.get(), int(sfrm.get()), int(sto.get())),root.destroy())
	).pack()
	#
	frame_from.pack()
	frame_to.pack()
	frame_spinbx.pack()
	frame_apply.pack()
	root.mainloop()

def show_cvs():
	global initialdir
	with open(filedialog.askopenfilename(initialdir=initialdir,title="Open output cvs file",filetypes=[('cvs files','.cvs'),('all files','.*')]),'r') as co:
		content = co.read().replace('.', '\033[96m.\033[0m').replace(',', '\033[31m,\033[0m').replace(';', '\033[41m;\033[0m')
		print(content)

def cvrtcvstodata():
	def set_inp(btn):
		btn.configure(
			text=filedialog.askopenfilename(initialdir=initialdir,title="Open input cvs file",filetypes=[('cvs files','.cvs'),('all files','.*')]))

	def set_out(btn):
		btn.configure(
			text=filedialog.asksaveasfilename(initialdir=initialdir,title="Open output cvs file",filetypes=[('cvs files','.cvs'),('all files','.*')]))

	def convert(finp, fout, inputs, outputs):
		with open(finp, 'r') as co:
			content = [[float(j) for i in x.split(';') for j in i.split(',')] for x in co.read().split('\n')]

		shapes = inputs+outputs

		with open(fout, 'wb') as co:
			for line in content:
				frm = 0
				for shape in shapes:
					arr = line[frm:frm+mul(shape)]
					co.write(mkts(shape, arr, 0))
					frm += mul(shape)

	root = tk.Toplevel()
	#
	finp = tk.Frame(root)
	tk.Label(finp, text='Input:').pack(side=tk.LEFT)
	btninp = tk.Button(finp, text='Input', command=lambda :set_inp(btninp))
	btninp.pack(side=tk.LEFT)
	finp.pack()
	#
	fout = tk.Frame(root)
	tk.Label(fout, text='Output:').pack(side=tk.LEFT)
	btnout = tk.Button(fout, text='Output', command=lambda :set_out(btnout))
	btnout.pack(side=tk.LEFT)
	fout.pack()
	#
	f0 = tk.Frame(root)
	tk.Label(f0, text='Inputs(sh0,sh1;sh0...):').pack(side=tk.LEFT)
	inputs = tk.StringVar()
	tk.Entry(f0, textvariable=inputs, width=20).pack(side=tk.LEFT)
	f0.pack()
	#
	f1 = tk.Frame(root)
	tk.Label(f1, text='Outputs(sh0,sh1;sh0...):').pack(side=tk.LEFT)
	outputs = tk.StringVar()
	tk.Entry(f1, textvariable=outputs, width=20).pack(side=tk.LEFT)
	f1.pack()
	#
	tk.Button(root, text='Convert', 
		command=(lambda :(
			convert(
				btninp.config('text')[-1],
				btnout.config('text')[-1], 
				list(map(lambda x:[int(i) for i in x.split(',')], inputs.get().strip(';').split(';'))),
				list(map(lambda x:[int(i) for i in x.split(',')], outputs.get().strip(';').split(';'))),
			),
			root.destroy()))
	).pack()

	root.mainloop()

def del_sm_cvs():
	global initialdir

	def new_(content, outfile, form):
		iform,oform = form.split(';')
		iform = iform.split(',')
		oform = oform.split(',')
		with open(outfile, 'w') as co:
			s = ''
			for lp in range(len(content)):
				new_line = ''
				relatif = 0
				for k,form in enumerate((iform, oform)):
					for i in form:
						if '+' in i:
							i,add = i.split('+')
							relatif += int(add)
						if '-' in i:
							i,sub = i.split('-')
							relatif -= int(sub)
						if 0 <= lp+relatif < len(content):
							if 'o' in i:
								new_line += content[lp+relatif][1][int(i[1:])] + ','
							elif 'i' in i:
								new_line += content[lp+relatif][0][int(i[1:])] + ','
							else:
								print(f'Not recognized {i}')
								exit()
						else:
							if 'o' in i:
								new_line += '0,'
							elif 'i' in i:
								new_line += '0,'
							else:
								print(f'Not recognized {i}')
								exit()
					new_line = new_line.strip(',') + ';'
				s += new_line.strip(';') + '\n'
			co.write(s.strip('\n'))

	def help_():
		root = tk.Toplevel()
		text = t.StringVar('''Each line will be rewrite in the form you have indicate.
Say you have cvs file:
0,1,2,3;4,5,6		form is: 'i0,i1,i2,i3;o0,o1,o2'
9,-1,4,4;0,0,1
1,1,1,1;1,1,1
Each pos is \{type\}\{id\} and you can add '+' or '-' : i0+1 (i0 of next line)
If you choos form: 'o0,i1+1,i0;i0-1,o2', you will have (not existing line will be set to 0)
4,-1,0;0,6
0,1,9;0,1
1,1,1;9,1''')
		tk.Message(root, textvariable=text).pack(side=tk.LEFT)
		root.mainloop()

	def set_file(btn):
		btn.configure(
			text=filedialog.asksaveasfilename(initialdir=initialdir,title="Open output cvs file",filetypes=[('cvs files','.cvs'),('all files','.*')]))

	with open(filedialog.askopenfilename(initialdir=initialdir,title="Open Input cvs file",filetypes=[('cvs files','.cvs'),('all files','.*')]),'r') as co:
		content = [list(map(lambda x:x.split(','), x.split(';'))) for x in co.read().split('\n')]
		leninp,lenout = len(content[0][0]), len(content[0][1])

	root = tk.Toplevel()
	#
	tk.Label(root, text='Line #0:'+str(content[0])).pack()
	#
	f = tk.Frame(root)
	tk.Label(f, text='New line form:').pack(side=tk.LEFT)
	form = tk.StringVar()
	tk.Entry(f, textvariable=form, width=20).pack(side=tk.LEFT)
	tk.Button(f, text='?', command=help_).pack(side=tk.LEFT)
	f.pack()
	#
	btn = tk.Button(root, text='Add output file', command=lambda : set_file(btn))
	btn.pack()
	#
	tk.Button(root, text='Make new', command=lambda :(new_(content, btn.config('text')[-1], form.get()),root.destroy())).pack()
	root.mainloop()

def show_data():
	pass

def show_mdl():
	global initialdir
	model = readmdl(filedialog.askopenfilename(initialdir=initialdir,title="Open input bin file",filetypes=[('cvs files','.bin'),('all files','.*')]))
	print(model)

def genfllycnd():
	def gen(rnn, rnd, layers):
		struct = [(list(map(int, shape.get().split(','))), f.get()) for _,f,shape,_,_ in layers]
		writemdl(fullcnd(*struct, rnn=rnn, rnd=rnd), filedialog.asksaveasfilename(initialdir=initialdir,title="Open output mdl bin file",filetypes=[('bin files','.bin'),('all files','.*')]))

	root = tk.Toplevel()
	#
	global layer_, layers
	layer_ = tk.LabelFrame(root, text='Layers (afunc, shape)')
	layers = []
	#
	def del_layer():
		layers[-1][0].destroy()
		del layers[-1]
	#
	def add_layer():
		global layer_, layers
		layers += [[tk.Frame(layer_), tk.StringVar(), tk.StringVar()]]
		layers[-1] += [tk.Entry(layers[-1][0], textvariable=layers[-1][2])]
		layers[-1] += [ttk.Combobox(layers[-1][0], width=10, textvariable=layers[-1][1])]
		#droped list
		layers[-1][-1]['values'] = funcs
		layers[-1][-1].pack()
		#
		layers[-1][-1].current(0)
		#spin box
		layers[-1][3].pack(side=tk.LEFT)
		#drop list
		layers[-1][-1].pack(side=tk.LEFT)
		layers[-1][0].pack()
	#
	add_layer()
	layer_.pack()
	#
	actions = tk.LabelFrame(root, text='Actions')
	tk.Button(actions, text='Add layer', command=add_layer).pack(side=tk.LEFT)
	tk.Button(actions, text='Del last', command=del_layer).pack(side=tk.LEFT)
	actions.pack(pady=3)
	#
	isrnn = tk.BooleanVar()
	isrnd = tk.BooleanVar()
	isrnnf = tk.LabelFrame(root, text='Options')
	tk.Checkbutton(isrnnf, text='rnn', variable=isrnn).pack()
	tk.Checkbutton(isrnnf, text='randomize', variable=isrnd).pack()
	isrnnf.pack()
	#
	tk.Button(root, text='Save to', command=(lambda:gen(isrnn.get(), isrnd.get(), layers))).pack()
	#
	root.mainloop()

def genfllycndneu():
	pass

class Struct:
	pass

class App:
	data = Struct()
	model = Struct()
	train = Struct()
	cyther = Struct()

	def __init__(self):
		self.root = tk.Tk()
		self.root.title('App')
		self.tabcontrole = ttk.Notebook(self.root)
		self.tabs = {
			'Data' : ttk.Frame(self.tabcontrole),
			'Models' : ttk.Frame(self.tabcontrole),
			'Train' : ttk.Frame(self.tabcontrole),
			'Cyther' : ttk.Frame(self.tabcontrole)
		}
		#
		self._creat_data()
		self._creat_model()
		self._creat_train()
		self._creat_cryptography()
		#
		self.tabcontrole.pack(expand=1, fill='both')
		self.root.mainloop()

	def _creat_data(self):
		self.tabcontrole.add(self.tabs['Data'], text='Data')
		#Cvs
		self.data.cvs = tk.LabelFrame(self.tabs['Data'], 
			text="Cvs Files Controle", padx=5, pady=10)
		self.data.cvs.pack(fill="both", expand="yes")
		
		tk.Button(self.data.cvs, text='Resize Cvs file', command=resize_cvs).pack()
		tk.Button(self.data.cvs, text='Show cvs', command=show_cvs).pack()
		tk.Button(self.data.cvs, text='Del values from cvs', command=del_sm_cvs).pack()
		#Tensor Data
		self.data.tsdata = tk.LabelFrame(self.tabs['Data'], 
			text="Tensor Data files controle", padx=5, pady=10)
		self.data.tsdata.pack(fill="both", expand="yes")

		tk.Button(self.data.tsdata, text='Convert cvs to data', command=cvrtcvstodata).pack()
		tk.Button(self.data.tsdata, text='Show data', command=show_data).pack()

	def _creat_model(self):
		self.tabcontrole.add(self.tabs['Models'], text='Models')
		#Ptr
		self.model.prt = tk.LabelFrame(self.tabs['Models'], 
			text="Show model", padx=5, pady=2)
		self.model.prt.pack(fill="both", expand="yes")
		
		tk.Button(self.model.prt, text='Print', command=show_mdl).pack()

		#Generate
		self.model.prt = tk.LabelFrame(self.tabs['Models'], 
			text="Generate Model", padx=5, pady=3)
		self.model.prt.pack(fill="both", expand="yes")
		
		tk.Button(self.model.prt, text='Fully connected    (Tensors)', command=genfllycnd).pack()
		tk.Button(self.model.prt, text='Fully connected (Neurones)', command=genfllycndneu).pack()

	def _creat_train(self):
		self.tabcontrole.add(self.tabs['Train'], text='Train')

	def _creat_cryptography(self):
		self.tabcontrole.add(self.tabs['Cyther'], text='Cyther')
		#Data
		self.cyther.data = tk.LabelFrame(self.tabs['Cyther'], 
			text="Cyther data", padx=5, pady=10)
		self.cyther.data.pack(fill="both", expand="yes")
		#Model
		self.cyther.mdl = tk.LabelFrame(self.tabs['Cyther'], 
			text="Cyther model", padx=5, pady=10)
		self.cyther.mdl.pack(fill="both", expand="yes")

if __name__ == "__main__":
	App()