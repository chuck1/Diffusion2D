#include <map>

#include <Diff2D/face.hpp>
#include <Diff2D/conn.hpp>
#include <Diff2D/util.hpp>

namespace sp = std;

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
		abort();
	}

	v = twin_->equs_[name];
	if(v) return v;
	
	return make_zeros<real,1>({n});
}



bool		are_neighbors(Face_s f1, sp::shared_ptr<Conn> const & conn1, Face_s f2, sp::shared_ptr<Conn> const & conn2) {
	if(!f1) return false;
	if(!conn1) return false;
	if(!f2) return false;
	if(!conn2) return false;
	return((conn1->twin_->face_ == f2) && (conn2->twin_->face_ == f1));
}
void		connect(Face_s f1, int a1, int b1, Face_s f2, int a2, int b2, bool parallel) {
	assert(f1);
	assert(f2);

	#ifdef _DEBUG
	std::cout << "connect" << std::endl;
	print_row(12,"f1","a1","b1","f2","a2","b2");
	print_row(12,f1,a1,b1,f2,a2,b2);
#endif

	void *c1 = 0;
	void *c2 = 0;
	sp::shared_ptr<Conn>& conn1 = f1->conns_[a1][b1];
	sp::shared_ptr<Conn>& conn2 = f2->conns_[a2][b2];
	
	if((conn1 != 0) || (conn2 != 0)) {
		// already connected
		if(are_neighbors(f1,conn1,f2,conn2)) return;
		
		print_row(16,"f1","conn1","f2","conn2");
		print_row(16,f1,conn1,f2,conn2);

		abort();//throw conns_not_zero();
		return;
	}
	
#ifdef _DEBUG
	auto ldebug = [&] () {
		std::cout << "connect" << std::endl;
		std::cout << "((face*)" << f1 << ")->conn[" << a1 << "][" << b1 << "] = ((conn*)" << conn1 << ")" << std::endl;
		std::cout << "((face*)" << f2 << ")->conn[" << a2 << "][" << b2 << "] = ((conn*)" << conn2 << ")" << std::endl;
	};
#endif

	if(parallel) {
		abort();
		//c1, c2 = multiprocessing.Pipe()
	}

	conn1 = std::make_shared<Conn>(f1, c1);
	conn2 = std::make_shared<Conn>(f2, c2);

	conn1->twin_ = conn2;
	conn2->twin_ = conn1;

	//f1.nbrs[a1,b1] = f2
	//f2.nbrs[a2,b2] = f1

	//f1->conns_[a1][b1] = conn1;
	//f2->conns_[a2][b2] = conn2;

#ifdef _DEBUG
	ldebug();
#endif

	conn1->refresh();
	conn2->refresh();
}



