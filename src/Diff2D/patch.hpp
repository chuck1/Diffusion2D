#ifndef PATCH_HPP
#define PATCH_HPP

#include <map>
#include <algorithm>

#include <Diff2D/config.hh>
#include <Diff2D/unit_vec.hpp>
#include <Diff2D/array.hpp>


typedef multivec< 3, array<real,1> >				v_bou_vec_type;

typedef std::map< std::string, v_bou_vec_type >			v_bou_type;


class Patch: public LocalCoor, public std::enable_shared_from_this<Patch> {
	public:
		Patch(
				Patch_Group_s group,
				std::string name,
				int normal,
				std::vector< std::vector<int> > indices,
				std::vector< array<real,1> > x,
				std::vector< array<int,1> > nx,
				v_bou_type v_bou);
		
		void		create_equ(std::string name, real v0, std::vector< array<real,1> > v_bou, real k, real al);

		void		set_v_bou(std::string equ_name, std::vector< array<real,1> > v_bou);

		real		max(std::string equ_name);

		void		grid_nbrs();
	public:

		Patch_Group_w				group_;
		std::string				name_;

		std::vector< std::vector<int> >		indices_;
		
		array<int,1>				npatch_;
		
		array<Face_s,2>				faces_;
};

#endif



