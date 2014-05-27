#ifndef TYPES_HPP
#define TYPES_HPP

#include <Diff2D/array.hpp>

typedef std::vector< array<real,1> >		coor_type;

typedef std::vector< array<size_t,1> >		cell_count_type;


typedef multivec< 3, array<real,1> >				v_bou_vec_type;

typedef std::map< std::string, v_bou_vec_type >			v_bou_type;


#endif


