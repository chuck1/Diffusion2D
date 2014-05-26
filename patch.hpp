#ifndef PATCH_HPP
#define PATCH_HPP

/*from face import *
from util import *
*/
#include <algorithm>

#include "config.hpp"
#include "unit_vec.hpp"
#include "array.hpp"

class Patch: public LocalCoor {
	public:
		Patch(
				Patch_Group* group,
				std::string name,
				int normal,
				std::vector< std::vector<int> > indices,
				std::vector< array<real,1> > x,
				std::vector< array<int,1> > nx,
				std::map<std::string, array< array<real,1>, 2 > > v_bou);

		void		create_equ(std::string name, real v0, std::vector< array<real,1> > v_bou, real k, real al);
		void		set_v_bou(std::string equ_name, std::vector< array<real,1> > v_bou);
		real		max(std::string equ_name);
		real		grid_nbrs();

		Patch_Group*				group_;
		std::string				name_;
		std::vector< std::vector<int> >		indices_;
		array<Face*,2>				faces_;
};

#endif

