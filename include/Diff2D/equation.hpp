#ifndef EQUATION_HPP
#define EQUATION_HPP

#include <memory>
#include <string>

#include <math-array/array.hpp>

#include <Diff2D/point.hpp>
#include <Diff2D/config.hpp>
#include <Diff2D/types.hpp>

class point_not_found: public std::exception {
};

class Equation_Prob {
	public:
		Equation_Prob(Prob_s prob, std::string name, real k, real alpha, real alpha_source);

		Prob_s		prob_;
		std::string	name_;
		real		k_;
		real		alpha_;
		real		alpha_source_;

};
class Equation {
	public:
		// diffusion equation variable set

		enum {
			ONLY_PARALLEL_FACES = 1 << 0
		};

		Equation(std::string name, Face_s face, Equation_Prob_s equ_prob);

		array<real,3>						grad();
		real							grad_mag();
		real							min();
		real							max();
		real							grad_min();
		real							grad_max();
		real							mean();
		real							at_point(point pt);

		std::string						name_;
		Face_s							face_;
		Equation_Prob_s						equ_prob_;
		
		array<real,2>						s_;
		array<real,2>						v_;

		unsigned int						flag_;
		equ_v_bou_type						v_bou_;
};

#endif
