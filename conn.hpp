#ifndef CONN_HPP
#define CONN_HPP

/*import math
import inspect
import numpy as np
import multiprocessing
from matplotlib import cm
import pylab as pl
import logging

from unit_vec import *

import equation
*/

#include <map>

#include "face.hpp"

class Conn {
	// information concerning connection between face and conn
	// from perspective of face
	Conn(Face* face, void* conns) {
		face_ = face;
		conns_ = conns;
		
		parallel_ = false;
	}
	void		refresh() {
		int OL_ = face_->nbr_to_loc(twin_.face)
		
		ol_, sol_ = v2is(OL_)
		
		PL_ = abs(cross(3, OL_))
		pl_,_ = v2is(PL_)

		PG_ = face_.loc_to_glo(PL_)
		
		li_, lj_, d_ = face_.index_lambda(twin_.face)
		
		////printinfo()
	}
	void		printinfo() {
		/*
		print "face",face_.Z
		print "nbr ",twin_.face.Z
		print "li lj"
		print inspect.getsource(li_)
		print inspect.getsource(lj_)*/
	}
	void		send(std::string name, array<real,2>* v) {
		if MP_:
			conns_[name].send(v)
		else:
			equs_[name] = v
	}
	void		recv(std::string name) {
		n = face_.n[pl_]
		
		if(parallel_) {
			v = conns_[name].recv()
		} else {
			try:
				v = twin_.equs[name]
			except:
				//print "warning: array not available"
				v = np.zeros(n)
		}
		return v
	}

	Face*					face_;
	void*					conns_;
	bool					parallel_;
	std::map<std::string, array<real,2> >	equs_;
};
	
	


void		connect(Face* f1, int a1, int b1, Face* f2, int a2, int b2, bool parallel = false) {
	/*if multi:
		c1, c2 = multiprocessing.Pipe()
	else:
		c1, c2 = None, None
	*/
	
	Conn* conn1 = new Conn(f1, c1)
	Conn* conn2 = new Conn(f2, c2)
	
	conn1->twin_ = conn2
	conn2->twin_ = conn1
	
	//f1.nbrs[a1,b1] = f2
	//f2.nbrs[a2,b2] = f1
	
	f1.conns[a1,b1] = conn1
	f2.conns[a2,b2] = conn2
	
	conn1.refresh()
	conn2.refresh()
}


#endif

