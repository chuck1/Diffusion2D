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

		LocalCoor(int Z) {
			Z_ = Z;
			X_ = nxt(Z_);
			Y_ = nxt(X_);

			x_ = IS(X_);
			y_ = IS(Y_);
			z_ = IS(Z_);
		}
		int		glo_to_loc(int G) {
			int sg = sign(G);
			int sz = sign(Z_);

			int d = nxt_dist(abs(Z_), abs(G));

			int L = sg * nxt(3 * sz, d);

			return L;
		}
		int		loc_to_glo(int L) {
			int G = sign(L) * nxt(Z_, abs(L) % 3);
			return G;
		}

		int	X_;
		int	Y_;
		int	Z_;
		IS	x_;
		IS	y_;
		IS	z_;

};





#endif



