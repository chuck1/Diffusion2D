import time
import numpy as np
import itertools
import pylab as pl
#from mpl_toolkits.mplot3d import Axes3D
#from matplotlib import cm
import math
import inspect
#import pickle
#import signal
import sys
import os

from face import *
from patch import *

import patch_group

class Fig:
	def __init__(self):
		self.fig = pl.figure()
		
		self.ax1 = self.fig.add_subplot(121)
		self.ax2 = self.fig.add_subplot(122)
		
		self.cb1 = None
		self.cb2 = None
		
		self.text = pl.figtext(0,0,'')

class Problem:
	def __init__(self, name, x, nx, it_max_1, it_max_2):
		self.name = name

		self.x = x
		self.nx = nx

		self.equs = {}
		
		self.it_max_1 = it_max_1
		self.it_max_2 = it_max_2

		self.patch_groups = []

		#signal.signal(signal.SIGINT, self)
	def create_equation(self, name, k, alpha, alpha_source):
		e = equation.equation_prob(self, name, k, alpha, alpha_source)
		self.equs[name] = e
		return e 

	def create_patch_group(self, name, v_0, S):
		g = patch_group.patch_group(self, name, v_0, S)
		
		self.patch_groups.append(g)

		return g

	def __call__(self, signal, frame):
		print "saving"
		#self.save()
		sys.exit(0)
		
	def temp_max(self, equ_name):
		T = float("-inf")
		for f in self.faces():
			e = f.equs[equ_name]
			T = max(e.max(), T)
		
		return T
	def temp_min(self, equ_name):
		T = float("inf")
		for f in self.faces():
			e = f.equs[equ_name]
			T = min(e.min(), T)
		
		return T
	def grad_max(self, equ_name):
		T = float("-inf")
		for f in self.faces():
			e = f.equs[equ_name]
			T = max(e.grad_max(), T)
		
		return T
	def grad_min(self, equ_name):
		T = float("inf")
		for f in self.faces():
			e = f.equs[equ_name]
			T = min(e.grad_min(), T)
		
		return T
	
	# value manipulation
	def value_add(self, equ_name, v):
		for f in self.faces():
			equ = f.equs[equ_name]
			equ.v = equ.v + v
	
	def value_normalize(self, equ_name):
		for g in self.patch_groups:
			# max value in patch group
			v_max = float("-inf")
			for p in g.patches:
				p_v_max = p.max(equ_name)
				print "patch max value",p_v_max
				v_max = max(v_max, p_v_max)
			
			print "max value",v_max
			
			# normalize
			for p in g.patches:
				for f in p.faces.flatten():
					equ = f.equs[equ_name]
					equ.v = equ.v / v_max
	
	def copy_value_to_source(self,equ_name_from,equ_name_to):

		for f in self.faces():
			e1 = f.equs[equ_name_from]
			e2 = f.equs[equ_name_to]
			
			s1 = tuple(a-2 for a in np.shape(e1.v))
			s2 = np.shape(e2.s)
				
			if s1 == s2:
				e2.s = e1.v[:-2,:-2]
			else:
				print s1, s2
				raise ValueError('size mismatch')

	
	# plotting
	def plot(self, equ_name):
		a, b = nice_axes(self.temp_min(equ_name), self.temp_max(equ_name))
		V = np.linspace(a, b, 21)
		
		a, b = nice_axes(self.grad_min(equ_name), self.grad_max(equ_name))
		Vg = np.linspace(a, b, 21)
		
		figs = {}
		
		for g in self.patch_groups:
			for p in g.patches:
				key = (p.Z, p.indices[p.z])
				try:
					f = figs[key]
				except:
					f = Fig()
					figs[key] = f
			
				con1, con2 = p.plot(equ_name, f, V, Vg)
		
				f.ax1.axis('equal')
				f.ax2.axis('equal')
			
				if f.cb1 is None:
					f.cb1 = pl.colorbar(con1, ax = f.ax1)
					f.cb2 = pl.colorbar(con2, ax = f.ax2)
		

		pl.show()
	def get_3d_axes(self):
		x = self.x

		x_mean = np.zeros(3)
		x_min = np.zeros(3)
		x_max = np.zeros(3)
		
		for a in range(3):
			x_mean[a] = np.mean(x[a])
			x_min[a] = min(x[a])
			x_max[a] = max(x[a])
			
		
		x_rng = x_max - x_min
		
		x_rng_max = max(x_rng) * 0.5
		
		#print x_mean
		#print x_rng
		#print x_rng_max
		
		l = x_mean - x_rng_max
		u = x_mean + x_rng_max
		
		return l,u

	def plot3(self, equ_name):
		T_max = self.temp_max()
		
		fig = pl.figure()
		ax = Axes3D(fig)
		


		for f in self.faces:
			f.plot3(ax, T_max)
		
		l,u = self.get_3d_axes()
		
		ax.set_xlim3d(l[0], u[0])
		ax.set_ylim3d(l[1], u[1])
		ax.set_zlim3d(l[2], u[2])

		return ax
	
	def faces(self):
		for g in self.patch_groups:
			for p in g.patches:
				for f in p.faces.flatten():
					yield f

	# solving
	def solve(self, name, cond, ver, R_outer = 0.0):
		return self.solve_serial(name, cond, ver, R_outer)
	
	def solve_serial(self, name, cond, ver, R_outer = 0.0):
		
		R = np.array([])
		
		for it in range(self.it_max_1):
			R = np.append(R, 0.0)
			
			for face in self.faces():
				R[-1] = max(face.step(name), R[-1])
				face.send(name)
			
			for face in self.faces():
				face.recv(name)
			
			
			if ver:
				print "{0:3d} {1:8e} {2:8e}".format(it, R_outer, R[-1])
			
			if math.isnan(R[-1]):
				raise ValueError('nan')
			
			if R[-1] < cond:
				break
		
		return it
		

	def solve2(self, equ_name, cond1_final, cond2, ver):
		#cond1 = 1
		
		#it_cond = 2
		
		R = 1.0
		for it_2 in range(self.it_max_2):
			
			cond1 = R / 1000.0 # target residual for inner loop is proportional to current residual for outer loop
			
			it_1 = self.solve(equ_name, cond1, ver, R)
			
			R = 0.0
			
			for g in self.patch_groups:
				Rn = g.reset_s(equ_name)
				
				R = max(Rn, R)
			
			print "{0:3d} {1:8e}".format(it_2,R)
			
			if math.isnan(R):
				raise ValueError('nan')
			
			if R < cond2:
				break
		
		return it_2

	def save(self):
		f = open('case_' + self.name, 'w')
		pickle.dump(self, f)

	def write(self, equ_name):
		
		directory = self.name + '/'
	
		if not os.path.exists(directory):
			os.makedirs(directory)

		name = 'prof_' + equ_name + '.txt'
		
		with open(directory + name,'w') as f:
			for g in self.patch_groups:
				print g
				g.write(equ_name,f)




