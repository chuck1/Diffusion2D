#ifndef PATCH_HPP
#define PATCH_HPP

#include <map>
#include <algorithm>

#include <math-array/array.hpp>

#include <Diff2D/config.hpp>
#include <Diff2D/unit_vec.hpp>
#include <Diff2D/types.hpp>




class Patch: public LocalCoor, public std::enable_shared_from_this<Patch> {
	public:
		Patch(
				Patch_Group_s group,
				std::string name,
				int normal,
				math::multivec<2,size_t> indices,
				std::vector< math::array<real,1> > x,
				std::vector< math::array<size_t,1> > nx,
				patch_v_bou_type v_bou);
		

		//void		create_equ(std::string name, real v0, patch_v_bou_edge_vec_type v_bou, real k, real al);

		void		set_v_bou(std::string equ_name, std::vector< math::array<real,1> > v_bou);

		/** @name value inspection and manipulation
		 * @{
		 */
		real		min(std::string const & equ_name) const;
		real		max(std::string const & equ_name) const;
		/** @} */
		void		create_faces();
		void		grid_nbrs();
		
		void		connection_info() const;
		void		info_geom() const;

		void		write_binary(std::string equ_name);
	public:

		Patch_Group_w				group_;
		std::string				name_;
		
		int					normal_;
		coor_type				coor_;
		cell_count_type				nx_;
		patch_v_bou_type			v_bou_;

		math::multivec<2,size_t>			indices_;
		
		math::array<size_t,1>				npatch_;
		
		math::array<Face_s,2>				faces_;
};

#endif



