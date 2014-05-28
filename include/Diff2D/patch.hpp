#ifndef PATCH_HPP
#define PATCH_HPP

#include <map>
#include <algorithm>

#include <Diff2D/config.hpp>
#include <Diff2D/unit_vec.hpp>
#include <Diff2D/array.hpp>
#include <Diff2D/types.hpp>




class Patch: public LocalCoor, public std::enable_shared_from_this<Patch> {
	public:
		Patch(
				Patch_Group_s group,
				std::string name,
				int normal,
				multivec<2,size_t> indices,
				std::vector< array<real,1> > x,
				std::vector< array<size_t,1> > nx,
				v_bou_type v_bou);
		

		void		create_equ(std::string name, real v0, std::vector< array<real,1> > v_bou, real k, real al);

		void		set_v_bou(std::string equ_name, std::vector< array<real,1> > v_bou);

		real		max(std::string equ_name);

		void		create_faces();
		void		grid_nbrs();
	public:

		Patch_Group_w				group_;
		std::string				name_;
		
		int					normal_;
		coor_type				coor_;
		cell_count_type				nx_;
		v_bou_type				v_bou_;

		multivec<2,size_t>			indices_;
		
		array<size_t,1>				npatch_;
		
		array<Face_s,2>				faces_;
};

#endif



