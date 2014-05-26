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
#include <memory>

#include "face.hpp"
#include "index_lambda.hpp"

class Conn {
	public:
		// information concerning connection between face and conn
		// from perspective of face
		Conn(Face* face, void* conns);
		void					refresh();
		void					printinfo();
		void					send(std::string name, array<real,1> v);
		array<real,1>				recv(std::string name);

		IS					pl_;
		IS					ol_;
		Index_Lambda				il_;
		
		Face*					face_;
		Conn*					twin_;
		void*					conns_;
		bool					parallel_;
		std::map<std::string, array<real,2> >	equs_;
};


void		connect(Face* f1, int a1, int b1, Face* f2, int a2, int b2, bool parallel = false);



#endif

