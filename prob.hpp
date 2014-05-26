#ifndef PROB_HPP
#define PROB_HPP

#include "config.hpp"
#include "array.hpp"

class Prob {
	public:
		Prob(
				std::string name,
				std::vector< array<real,1> > x,
				std::vector< array<int,1> > nx,
				int it_max_1,
				int it_max_2);
		Equation_s			create_equation(std::string name, real k, real alpha, real alpha_source);
		Patch_Group_s			create_patch_group(std::string name, std::map<std::string, real> v_0, std::map<std::string, real> S);
		real				temp_max(std::string equ_name);
		real				temp_min(std::string equ_name);
		real				grad_max(std::string equ_name);
		real				grad_min(std::string equ_name);
		void				value_add(std::string equ_name, array<real,2> v);
		void				value_normalize(std::string equ_name);
		void				copy_value_to_source(std::string equ_name_from, std::string equ_name_to);
		std::vector<Face_s>		faces();
		int				solve(std::string name, real cond, bool ver, real R_outer);
		int				solve_serial(std::string name, real cond, bool ver, real R_outer);
		int				solve2(std::string equ_name, real cond1_final, real cond2, bool ver);
		void				save();
		void				write(std::string equ_name);


		std::vector<Patch_Group_s>			patch_groups_;

		std::string					name_;

		std::vector< array<real,1> >			x_;
		std::vector< array<int,1> >			nx_;

		std::map<std::string, Equation_Prob_s>		equs_;

		int						it_max_1_;
		int						it_max_2_;


};

#endif
