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

#include <math-array/array.hpp>


#include <Diff2D/config.hpp>
#include <Diff2D/unit_vec.hpp>
#include <Diff2D/equation.hpp>	
#include <Diff2D/conn.hpp>
#include <Diff2D/index_lambda.hpp>
#include <Diff2D/patch.hpp>
#include <Diff2D/patch_group.hpp>

struct Term {
	real	prod() { return a*y; }

	real	a;
	real	y;
};

struct grid_return_type {
	grid_return_type(
			math::array<real,2> x,
			math::array<real,2> y,
			math::array<real,2> z,
			math::array<real,2> w):
		X({x,y,z}),
		W(w)
	{}
	math::array<real,2>	X[3];
	math::array<real,2>	W;
};

//typedef std::tuple< std::vector<real>, std::vector<real>, std::vector<real>, std::vector<real> >		grid_tup;
//typedef std::tuple< array<real,2>, array<real,2>, array<real,2>, array<real,2> >		grid_tup;

/** @ingroup group_core
 * @brief %Face
 */
class Face: public LocalCoor, public std::enable_shared_from_this<Face> {
	public:
		Face(Patch_s patch, int normal, math::array<real,2> const & ext, real pos_z, math::array<size_t,1> n);

		Equation_s		create_equ(std::string name, Equation_Prob_s equ_prob);
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
		real			step(std::string equ_name, size_t it);

		void			send(std::string equ_name);
		void			recv(std::string equ_name);

		grid_return_type	grid(std::string equ_name);

		void			write_binary(std::string equ_name, math::basic_binary_oarchive& ar);
	public:

		Patch_s						patch_;
		math::array<real,2>					ext_;
		std::vector< std::vector<Conn_s> >		conns_;
		real						pos_z_;
		math::array<size_t,1>					n_;
		math::array<real,3>					d_;
		math::array<real,1>					l_;
		std::map< std::string, Equation_s >		equs_;

		//std::map<std::string, array<real,1>[2][2]>	v_bou_;
};



#endif
