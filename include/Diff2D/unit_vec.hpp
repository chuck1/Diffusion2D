#ifndef UNIT_VEC_HPP
#define UNIT_VEC_HPP

#include <algorithm>

struct IS {
	IS() {}
	IS&	operator=(IS const & is) {
		i = is.i;
		s = is.s;
		return *this;
	}
	IS(int ni, int ns): i(ni), s(ns) {}
	IS(int v) {
		i = std::abs(v) - 1;
		s = (v < 0) ? -1 : 1;
	}
	int		v() {
		return (i + 1) * s;
	}
	int i;
	int s;
};

int sign(int x);
int fwd(int A, int n);
int fwd_dist(int A, int B);
int bwd(int A, int n);
int bwd_dist(int A, int B);
int nxt(int A, int n = 1);
int nxt_dist(int A, int B);
int cross(int A,int B);

class LocalCoor {
	public:
		//# store as [[-1, 1],[-2, 2],[-3, 3]]

		LocalCoor(int Z);
		int		glo_to_loc(int G);
		int		loc_to_glo(int L);

		int	Z_;
		int	X_;
		int	Y_;
		
		IS	x_;
		IS	y_;
		IS	z_;

};



#endif



