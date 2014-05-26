#ifndef FACE_HPP
#define FACE_HPP

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
#include <vector>

#include "math.hpp"
#include "array.hpp"
#include "config.hpp"
#include "unit_vec.hpp"
#include "equation.hpp"	
#include "conn.hpp"
#include "index_lambda.hpp"
#include "patch.hpp"
#include "patch_group.hpp"

struct Term {
	real	prod() { return a*y; }

	real	a;
	real	y;
};

class Face: public LocalCoor, public std::enable_shared_from_this<Face> {
	public:
	Face(Patch_s patch, int normal, array<real,2> const & ext, real pos_z, array<int,1> n);
	void			create_equ(std::string name, Equation_Prob_s equ_prob);
	int			get_loc_pos_par_index(Face_s nbr);
	real			x(int i);
	real			y(int j);
	real			area();
	int			nbr_to_loc(Face_s nbr);
	Conn_s			loc_to_conn(int V);
	Index_Lambda		index_lambda(Face_s nbr);
	void			send_array(Equation_s equ, Conn_s conn);
	void			recv_array(Equation_s equ, Conn_s conn);
	Term			term(Equation_s equ, std::vector<int> ind, int v, int sv, real To);
	void			step_pre_cell(Equation_s equ, std::vector<int> ind, int V);
	void			step_pre_cell_open_bou(Equation_s equ, std::vector<int> ind, int V);
	void			step_pre(Equation_s equ);
	real			step(std::string equ_name);
	void			send(std::string equ_name);
	void			recv(std::string equ_name);

	Patch_s						patch_;
	array<real,2>					ext_;
	real						pos_z_;
	array<int,1>					n_;
	array<real,3>					d_;
	array<real,1>					l_;
	std::map< std::string, Equation_s >		equs_;
	std::vector< std::vector<Conn_s> >		conns_;

};



#endif
