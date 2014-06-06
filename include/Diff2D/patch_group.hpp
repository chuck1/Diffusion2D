#ifndef PATCH_GROUP_HPP
#define PATCH_GROUP_HPP

#include <memory>

#include <Diff2D/config.hpp>
#include <Diff2D/patch.hpp>
#include <Diff2D/prob.hpp>
#include <Diff2D/point.hpp>

class Patch_Group: public std::enable_shared_from_this<Patch_Group> {
	public:
		enum flags {
			SOURCE_SOLVED = 1 << 0
		};
		Patch_Group(
				Prob_s prob,
				std::string name,
				std::map<std::string, real> v_0,
				std::map<std::string, real> S,
				point v_0_point);

		Patch_s				create_patch(
				std::string name,
				int normal,
				std::vector<size_t> indicesx,
				std::vector<size_t> indicesy,
				std::vector<size_t> indicesz,
				patch_v_bou_type v_bou);

		real				reset_s(size_t it, std::string equ_name);
		
		std::vector< Face_s >		faces();
		
		void				write(std::string equ_name, std::ofstream& ofs);
		void				write_binary(std::string equ_name);
		/** @name value inspection and manipulation
		 * @{
		 */
		void				value_add(std::string const & name, real const & a);
		void				value_divide(std::string const & name, real const & a);
		void				value_clamp(std::string const & name, real const & a, real const & b);
		real				min(std::string const & name) const;
		real				max(std::string const & name) const;
		/** @} */
		void				get_value_of_interest(std::string const & ename, real& target, real& value, real& error);
		real				get_value_of_interest_residual(std::string const & ename);

	public:
		std::vector<Patch_s>			patches_;
		Prob_w					prob_;
		std::string				name_;
		std::map<std::string, real>		v_0_;
		point					v_0_point_;
		unsigned int				flag_;
		std::map<std::string, array<real,1> >	v_0_history_;
		std::map<std::string, array<real,1> >	S_;
};


#endif




