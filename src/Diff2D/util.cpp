import math

from unit_vec import *


class NoIntersectError(Exception):
	pass

class EdgeError(Exception):
	def __init__(self, rev):
		self.rev = rev

def align(ind1, ind2):
	ver = False
	#ver = True

	s = max(min(ind1), min(ind2))
	e = min(max(ind1), max(ind2))
	
	if ver:
		print "ind1", ind1
		print "ind2", ind2
		print "s", s, "e", e
	
	try:
		s1 = ind1.index(s)
		e1 = ind1.index(e)
		s2 = ind2.index(s)
		e2 = ind2.index(e)
	except ValueError:
		raise NoIntersectError
	
	if ver:
		print "s1", s1, "e1", e1
		print "s2", s2, "e2", e2

	if s == e:
		raise EdgeError(False if s1 > s2 else True)
	
	d1 = sign(e1-s1)
	d2 = sign(e2-s2)
	
	if ver:
		print ind1
		print ind2

		print "min",min(ind1), min(ind2), "s", s
		print "max",max(ind1), max(ind2), "e", e
	
		print s1, e1, d1
		print s2, e2, d2
	
	i1 = lambda i: s1 + (i - (d1-1)/2) * d1
	i2 = lambda i: s2 + (i - (d2-1)/2) * d2
	
	r1 = []
	r2 = []
	
	for i in range(abs(e1-s1)):
		if ver:
			print "1:", i1(i), "'", ind1[i1(i)], ind1[i1(i)+1], "'" #, ind1[i+s1]
			print "2:", i2(i), "'", ind2[i2(i)], ind2[i2(i)+1], "'" #, ind2[i+s2]
		
		r1.append(i1(i))
		r2.append(i2(i))

	return r1,r2
	
	
	
def nice_axes(a, b):
	p = math.floor(math.log10(b - a))
	a = nice_lower(a, p)
	b = nice_upper(b, p)
	return a, b

def nice_upper(a, p):
	a = a / 10**p
	a = math.ceil(a)
	a = a * 10**p
	return a
	
def nice_lower(a, p):
	a = a / 10**p
	a = math.floor(a)
	a = a * 10**p
	return a


