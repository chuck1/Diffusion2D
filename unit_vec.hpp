

struct IS {
	IS() {}
	IS&	operator=(IS const & is) {
		i = is.i;
		s = is.s;
	}
	IS(int ni, int ns): i(ni), s(ns) {}
	IS(int v) {
		i = abs(v) - 1;
		s = (v < 0) ? -1 : 1;
	}
	int		v() {
		return (i + 1) * s;
	}
	int i;
	int s;
};

int sign(int x) {
	return (x < 0) ? -1 : 1;
}
int fwd(int A, int n) {
	IS a(A);
	
	a.i = (a.i + n) % 3;
	
	return a.v();
}
int fwd_dist(int A, int B) {
	IS a(A);//a,sa = v2is(A);
	IS b(B);//b,sb = v2is(B);
	
	int d = b.i - a.i;
	d = (d > 0) ? d : (d + 3);
	return d;
}
int bwd(int A, int n) {
	int B = fwd(A, 2 * n);
	return B;
	}
int bwd_dist(int A, int B) {
	int d = fwd_dist(B, A);
	}
int nxt(int A, int n = 1) {
	if(A > 0) {
		return fwd(A, n);
	} else {
		return bwd(A, n);
	}
}
int nxt_dist(int A, int B) {
	if(A > 0) {
		return fwd_dist(A, B);
	} else {
		return bwd_dist(A, B);
	}
}

int cross(int A,int B) {
	//#print A,B
	
	IS a(A);//a,sa = v2is(A);
	IS b(B);//b,sb = v2is(B);

	int c;

	if(b.i == (a.i+1) % 3) {
		c = ((a.i+2) % 3) + 1;
	} else {
		c = ((a.i+1) % 3) + 1;
		c = -c;
	}

	c *= a.s * b.s;
	
	//#print a, b, c
	
	return c;
}

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






