import math
import numpy as np

import patch

class patch_group:
	def __init__(self, prob, name, v_0, S):
		
		self.patches = []

		self.prob = prob
		self.name = name
		self.v_0 = v_0

		self.S = S

		print "patch_group"
		print self.v_0

	def create_patch(self, name, normal, indices, v_bou):
		
		#print 'T_0',T_0
		
		p = patch.Patch(self, name, normal, indices, self.prob.x, self.prob.nx, v_bou)
		
		self.patches.append(p)
		
		return p

	def reset_s(self, equ_name):
		def debug():
			print "reset_s"
			print "equ_name",equ_name
			print "v_m     ",v_m
			print "v_0     ",v_0
			print "dS      ",dS
			print "S       ",self.S[equ_name]
		
		v_0 = self.v_0[equ_name]
		
		if v_0 == 0.0:
			return
		
		# current area-weighted-average value
		v = float(0)
		A = float(0)
		
		for p in self.patches:
			for f in p.faces.flatten():
				equ = f.equs[equ_name]
		
				a = f.area()
				v += equ.mean() * a
				A += a
		
		print "name       ",self.name
		print "num patches",len(self.patches)

		v_m = v/A
		
		dv = v_0 - v_m
		
		dS = equ.equ_prob.k * dv / 10.
		
		self.S[equ_name] += dS
		
		#self.Tmean.append(vm)
		
		debug()
		
		return math.fabs(dv/v_0)

	def faces(self):
		for p in self.patches:
			for f in p.faces.flatten():
				yield f

	def write(self, equ_name, file):
		
		x = np.zeros(0)
		y = np.zeros(0)
		z = np.zeros(0)
		w = np.zeros(0)

		for f in self.faces():
			X,Y,Z,W = f.grid(equ_name)
			
			x = np.append(x, X.ravel())
			y = np.append(y, Y.ravel())
			z = np.append(z, Z.ravel())
			w = np.append(w, W.ravel())
		
		name = 'prof_' + self.name + '_' + equ_name
		
		n = np.size(x,0)
		
		
		file.write("(({0} point {1})\n".format(name,n))
		

		file.write("(x\n")
		file.write("".join(np_join(x)) + ")\n")
		
		file.write("(y\n")
		file.write("".join(np_join(y)) + ")\n")

		file.write("(z\n")
		file.write("".join(np_join(z)) + ")\n")
		
		file.write("(w\n")
		file.write("".join(np_join(w)) + ")\n")
		#file.write(" ".join("{0:e}".format(a) for a in w) + ")")
		
		file.write(")\n")
		
		
		
def np_join(x):
	a = 0

	for b in x:
		yield "{0:e} ".format(b)
		a = a + 1
		if a == 10:
			yield "\n"
			a = 0





