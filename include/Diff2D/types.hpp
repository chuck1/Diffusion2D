#ifndef TYPES_HPP
#define TYPES_HPP

/** @file types.hpp */

#include <map>

#include <math-array/array.hpp>

struct boundary;

typedef std::vector< math::array<real,1> >					coor_type;

typedef std::vector< math::array<size_t,1> >					cell_count_type;


typedef std::vector< std::shared_ptr< boundary > >			patch_v_bou_edge_vec_type;


typedef math::multivec< 2, std::shared_ptr< boundary > >			equ_v_bou_type;

typedef math::multivec< 3, std::shared_ptr< boundary > >			patch_v_bou_vec_type;

typedef std::map< std::string, patch_v_bou_vec_type >			patch_v_bou_type;


#endif


