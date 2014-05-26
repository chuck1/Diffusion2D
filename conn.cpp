#include <map>

#include "face.hpp"
#include "conn.hpp"

// information concerning connection between face and conn
// from perspective of face
Conn::Conn(Face* face, void* conns) {
	face_ = face;
	conns_ = conns;

	parallel_ = false;
}
void		Conn::refresh() {
	int OL_ = face_->nbr_to_loc(twin_.face);

	ol_, sol_ = v2is(OL_);

	PL_ = abs(cross(3, OL_));
	pl_ = IS(PL_);
	
	PG_ = face_.loc_to_glo(PL_);
	
	il_ = face_.index_lambda(twin_.face);

	////printinfo()
}
void		Conn::printinfo() {
	/*
	   print "face",face_.Z
	   print "nbr ",twin_.face.Z
	   print "li lj"
	   print inspect.getsource(li_)
	   print inspect.getsource(lj_)*/
}
void		Conn::send(std::string name, array<real,2>* v) {
	if(parallel_) {
		conns_[name].send(v);
	} else {
		equs_[name] = v;
	}
}
void		Conn::recv(std::string name) {
	n = face_.n[pl_];

	if(parallel_) {
		v = conns_[name].recv();
	} else {
		try {
			v = twin_.equs[name]
		} catch(...) {
			//print "warning: array not available"
			v = np.zeros(n);
		}
		return v;
	}
}




void		Conn::connect(Face* f1, int a1, int b1, Face* f2, int a2, int b2, bool parallel = false) {
	/*if multi:
	  c1, c2 = multiprocessing.Pipe()
else:
c1, c2 = None, None
*/

	Conn* conn1 = new Conn(f1, c1);
	Conn* conn2 = new Conn(f2, c2);

	conn1->twin_ = conn2;
	conn2->twin_ = conn1;

	//f1.nbrs[a1,b1] = f2
	//f2.nbrs[a2,b2] = f1

	f1.conns[a1,b1] = conn1;
	f2.conns[a2,b2] = conn2;

	conn1.refresh();
	conn2.refresh();
}



