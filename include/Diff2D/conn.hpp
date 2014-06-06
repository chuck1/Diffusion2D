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
#include <exception>

#include "face.hpp"
#include "index_lambda.hpp"

struct conns_not_zero: std::exception {
	const char*	what() {
		return "conns not zero";
	}
};

class Conn {
	public:
		// information concerning connection between face and conn
		// from perspective of face
		Conn(Face_s face, void* conns);
		void					refresh();
		void					printinfo();
		void					send(std::string name, array<real,1> v);
		array<real,1>				recv(std::string name);

		IS					pl_;
		IS					ol_;

		int					PG_;

		Index_Lambda				il_;
		

		Face_s					face_;
		Conn_s					twin_;
		void*					conns_;
		bool					parallel_;
		std::map<std::string, array<real,1> >	equs_;
};


void		connect(Face_s f1, int a1, int b1, Face_s f2, int a2, int b2, bool parallel = false);



#endif

