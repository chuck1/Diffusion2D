#include <map>

#include <Diff2D/face.hpp>
#include <Diff2D/conn.hpp>

// information concerning connection between face and conn
// from perspective of face
Conn::Conn(Face_s face, void* conns):
	face_(face),
twin_(),
	conns_(conns),
	parallel_(false)
{
	assert(face_);
}
void		Conn::refresh() {
	int OL_ = face_->nbr_to_loc(twin_->face_);

	ol_ = IS(OL_);
	
	int PL_ = abs(cross(3, OL_));
	pl_ = IS(PL_);
	
	PG_ = face_->loc_to_glo(PL_);
	
	il_ = face_->index_lambda(twin_->face_);

	////printinfo()
}
void			Conn::printinfo() {
	/*
	   print "face",face_.Z
	   print "nbr ",twin_.face.Z
	   print "li lj"
	   print inspect.getsource(li_)
	   print inspect.getsource(lj_)*/
}
void			Conn::send(std::string name, array<real,1> v) {
	if(parallel_) {
		assert(0);
		//conns_[name].send(v);
	} else {
		equs_[name] = v;
	}
}
array<real,1>		Conn::recv(std::string name) {
	size_t n = face_->n_->get(pl_.i);
	
	array<real,1> v;

	if(parallel_) {
		//v = conns_[name].recv();
		assert(0);
	}

	v = twin_->equs_[name];
	if(v) return v;
	
	return make_zeros<real,1>({n});
}


void		connect(Face_s f1, int a1, int b1, Face_s f2, int a2, int b2, bool parallel) {
	void *c1 = 0;
	void *c2 = 0;
	
	if(parallel) {
		assert(0);
		//c1, c2 = multiprocessing.Pipe()
	}

	auto conn1 = std::make_shared<Conn>(f1, c1);
	auto conn2 = std::make_shared<Conn>(f2, c2);
	
	conn1->twin_ = conn2;
	conn2->twin_ = conn1;
	
	//f1.nbrs[a1,b1] = f2
	//f2.nbrs[a2,b2] = f1
	
	f1->conns_[a1][b1] = conn1;
	f2->conns_[a2][b2] = conn2;

	conn1->refresh();
	conn2->refresh();
}



