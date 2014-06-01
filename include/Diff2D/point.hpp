#ifndef DIFF2D_POINT_HPP
#define DIFF2D_POINT_HPP

#include <Diff2D/config.hpp>

struct point {
	point(real nx, real ny, real nz):
		x(nx), y(ny), z(nz) {}
	point(point const & p):
		x(p.x), y(p.y), z(p.z) {}

	real&		operator[](unsigned int i) {
		assert(i < 3);
		return v[i];
	}

	union {
		struct {
			real x;
			real y;
			real z;
		};
		real v[3];
	};
};


#endif


