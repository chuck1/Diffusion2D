#include <Diff2D/unit_vec.hpp>

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
	return d;
}
int nxt(int A, int n) {
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





