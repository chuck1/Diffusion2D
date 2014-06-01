#ifndef PATCH_GROUP_HPP
#define PATCH_GROUP_HPP

#include <memory>

#include <Diff2D/config.hpp>
#include <Diff2D/patch.hpp>
#include <Diff2D/prob.hpp>
#include <Diff2D/point.hpp>

class Patch_Group: public std::enable_shared_from_this<Patch_Group> {
	public:
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

		real		reset_s(std::string equ_name);

		std::vector< Face_s >		faces();

		void				write(std::string equ_name, std::ofstream& ofs);
		void				write_binary(std::string equ_name);
	public:
		std::vector<Patch_s>		patches_;
		Prob_w				prob_;
		std::string			name_;
		std::map<std::string, real>	v_0_;
		std::map<std::string, real>	S_;
		point				v_0_point_;
};


#endif




