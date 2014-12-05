#ifndef PROB_HPP
#define PROB_HPP

#include <map>

#include <math-array/array.hpp>

#include <Diff2D/config.hpp>
#include <Diff2D/prob.hpp>
#include <Diff2D/types.hpp>
#include <Diff2D/point.hpp>

/** @ingroup group_core
 * @brief %Problem
 */
class Prob: public std::enable_shared_from_this<Prob> {
	public:
		Prob(
				std::string name,
				coor_type x,
				cell_count_type nx,
				int it_max_1,
				int it_max_2);
		Equation_Prob_s			create_equation(std::string name, real k, real alpha, real alpha_source);
		Patch_Group_s			create_patch_group(std::string name, std::map<std::string, real> v_0, std::map<std::string, real> S, point v_0_point);
		/** @name value inspection and manipulation
		 * @{
		 */
		real				max(std::string const & equ_name) const;
		real				min(std::string const & equ_name) const;
		real				grad_max(std::string const & equ_name) const;
		real				grad_min(std::string const & equ_name) const;
		void				value_add(std::string const & equ_name, real const & v);
		void				value_add(std::string const & equ_name, math::array<real,2> const & v);
		void				value_normalize(std::string const & equ_name);
		void				value_clamp_per_group(std::string const & name, real a, real const & b);
		void				copy_value_to_source(std::string equ_name_from, std::string equ_name_to);
		/** @} */
		std::vector<Face_s>		faces() const;
		/** @brief solve
		 *
		 * @param name name of variable to solve
		 * @param cond residual stop condition
		 * @param it_outer current iteration of outer loop for purpose of displaying
		 * @param R_outer current residual of outer loop for purpose of displaying
		 */
		int				solve(std::string name, real cond, size_t it_outer, real R_outer);
		int				solve_serial(std::string name, real cond, size_t it_outer, real R_outer);

		/** @brief solve_outer_group
		 *
		 * @param ename name
		 * @param cond_outer residual stop condition of outer loop
		 * @param cond_inner residual stop condition of inner loop
		 * @param group Patch_Group for which enforce target value
		 */
		int				solve_outer_group(std::string const & equ_name, real cond_outer, real inner_out_ratio, std::shared_ptr<Patch_Group> group);
		/** @brief solve2
		 *
		 * @param ename name
		 * @param cond_outer residual stop condition of outer loop
		 * @param cond_inner residual stop condition of inner loop
		 */
		void				solve2(std::string equ_name, real cond_outer, real inner_out_ratio);

		void				save();

		void				write(std::string equ_name, std::string filename_post = std::string());
		void				write_binary(std::string equ_name);

		void				connection_info() const;
		void				value_stats(std::string const & name) const;

		std::string			mkdir(std::string const & name) const;

		std::vector<Patch_Group_s>			patch_groups_;

		std::string					name_;

		std::vector< math::array<real,1> >			x_;
		std::vector< math::array<size_t,1> >			nx_;

		std::map<std::string, Equation_Prob_s>		equs_;

		size_t						it_max_inner_;
		size_t						it_max_outer_;


};

#endif


