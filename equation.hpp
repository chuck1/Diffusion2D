#ifndef EQUATION_HPP
#define EQUATION_HPP

#include <memory>
#include <string>

#include "config.hpp"
#include "array.hpp"

class Equation_Prob {
	public:
		Equation_Prob(Prob* prob, std::string name, real k, real alpha, real alpha_source);

		Prob*		prob_;
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

		Equation(std::string name, Face* face, Equation_Prob* equ_prob);
		array<real,2>		grad();
		real			grad_mag();
		real			min();
		real			max();
		real			grad_min();
		real			grad_max();
		real			mean();

		std::string						name_;
		Face*							face_;
		Equation_Prob*						equ_prob_;
		array<real,2>						v_;
		unsigned int						flag_;
		std::vector< std::vector< array<real,1> > >		v_bou_;
};

#endif
