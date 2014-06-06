#include <iostream>

#include <Diff2D/config.hpp>
#include <Diff2D/unit_vec.hpp>

LocalCoor::LocalCoor(int Z):
	Z_(Z),
	X_(0),
	Y_(0),
	x_(0),
	y_(0),
	z_(0)
{
/*	X_(nxt(Z_)),
	Y_(nxt(X_)),
	x_(IS(X_)),
	y_(IS(Y_)),
	z_(IS(Z_))*/
	X_=(nxt(Z_));
	Y_=(nxt(X_));
	x_=(IS(X_));
	y_=(IS(Y_));
	z_=(IS(Z_));

	DEBUG_LINE std::cout << "LocalCoor ctor" << std::endl;
}
int		LocalCoor::glo_to_loc(int G) {
	int sg = sign(G);
	int sz = sign(Z_);

	int d = nxt_dist(abs(Z_), abs(G));

	int L = sg * nxt(3 * sz, d);

	return L;
}
int		LocalCoor::loc_to_glo(int L) {
	int G = sign(L) * nxt(Z_, abs(L) % 3);
	return G;
}
IS		LocalCoor::glo_to_loc2(int G) {
	return IS(glo_to_loc(G));
}
IS		LocalCoor::loc_to_glo2(int L) {
	return IS(loc_to_glo(L));
}
IS		LocalCoor::glo_to_loc2(IS G) {
	return IS(glo_to_loc(G.v()));
}
IS		LocalCoor::loc_to_glo2(IS L) {
	return IS(loc_to_glo(L.v()));
}



