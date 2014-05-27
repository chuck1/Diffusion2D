#ifndef PATCH_GROUP_HPP
#define PATCH_GROUP_HPP

#include <memory>

#include <Diff2D/config.hh>
#include <Diff2D/patch.hpp>
#include <Diff2D/prob.hpp>


class Patch_Group: public std::enable_shared_from_this<Patch_Group> {
	public:
		Patch_Group(
				Prob_s prob,
				std::string name,
				std::map<std::string, real> v_0,
				std::map<std::string, real> S);

		Patch_s				create_patch(
				std::string name,
				int normal,
				std::vector< std::vector<int> > indices,
				v_bou_type v_bou);

		real		reset_s(std::string equ_name);

		std::vector< Face_s >		faces();

		void				write(std::string equ_name, std::ofstream& ofs);





		std::vector<Patch_s>		patches_;
		Prob_w				prob_;
		std::string			name_;
		std::map<std::string, real>	v_0_;
		std::map<std::string, real>	S_;
};


#endif




