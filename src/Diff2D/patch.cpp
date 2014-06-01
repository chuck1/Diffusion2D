#include <map>
#include <algorithm>
#include <vector>

#include <math-array/array.hpp>


#include <Diff2D/config.hpp>
#include <Diff2D/face.hpp>
#include <Diff2D/unit_vec.hpp>



Patch::Patch(
		Patch_Group_s group,
		std::string name,
		int normal,
		multivec<2,size_t> indices,
		coor_type x,
		cell_count_type nx,
		patch_v_bou_type v_bou):
	LocalCoor(normal),
	group_(group),
	name_(name),
	normal_(normal),
	coor_(x),
	nx_(nx),
	v_bou_(v_bou),
	indices_(indices)
{
	//if not np.shape(v_bou_) == (2,2) {
	//	print v_bou_
	//	raise ValueError('')

	//print 'T_0',T_0

	auto NX = indices[x_.i].size() - 1;
	auto NY = indices[y_.i].size() - 1;

	// expand scalar v_bou values
	/*		for k in v_bou.keys() {
			if isinstance(v_bou[k][0][0], float) { v_bou[k][0][0] = np.ones(NY)*v_bou[k][0][0]
			if isinstance(v_bou[k][0][1], float) { v_bou[k][0][1] = np.ones(NY)*v_bou[k][0][1]
			if isinstance(v_bou[k][1][0], float) { v_bou[k][1][0] = np.ones(NX)*v_bou[k][1][0]
			if isinstance(v_bou[k][1][1], float) { v_bou[k][1][1] = np.ones(NX)*v_bou[k][1][1]
			}*/

	// make sure indices are sorted properly

	std::sort(indices[x_.i].begin(), indices[x_.i].end());
	std::sort(indices[y_.i].begin(), indices[y_.i].end());

	if(normal < 0) {
		std::reverse(indices[x_.i].begin(), indices[x_.i].end());
		std::reverse(indices[y_.i].begin(), indices[y_.i].end());
	}

	npatch_ = make_array_1<size_t,1>({NX,NY});

	//faces_ = faces;

}
void		Patch::create_faces() {
	// alloc faces array
	faces_ = make_zeros<Face_s,2>(npatch_->ravel());

	size_t NX = npatch_->get(0);
	size_t NY = npatch_->get(1);

	for(size_t i = 0; i < NX; ++i) {
		for(size_t j = 0; j < NY; ++j) {
			int I = indices_[x_.i][i];
			int J = indices_[y_.i][j];
			int M = indices_[x_.i][i+1];
			int N = indices_[y_.i][j+1];

			int Is = std::min(I,M);
			int Js = std::min(J,N);
			int Ms = std::max(I,M);
			int Ns = std::max(J,N);

			auto ext = make_uninit<real,2>({2,2});
			ext->get(0,0) = coor_[x_.i]->get(Is);
			ext->get(0,1) = coor_[x_.i]->get(Ms);
			ext->get(1,0) = coor_[y_.i]->get(Js);
			ext->get(1,1) = coor_[y_.i]->get(Ns);


			auto numx = nx_[x_.i]->get(std::min(I,M));
			auto numy = nx_[y_.i]->get(std::min(J,N));

			auto numarr = make_array_1<size_t,1>({numx, numy});

			//print "I,J",I,J

			real pos_z = coor_[z_.i]->get(indices_[z_.i]);

			auto nface = std::make_shared<Face>(shared_from_this(), normal_, ext, pos_z, numarr);

			faces_->get(i,j) = nface;

			// unique to current setup
			// create temperature and source spreader equations

			auto prob = group_.lock()->prob_.lock();

			// T
			{
				auto equ = nface->create_equ("T", prob->equs_["T"]);

				auto v_bou_T = v_bou_.find("T");
				if(v_bou_T == v_bou_.cend()) {
					equ->v_bou_ = {{0,0},{0,0}};
				} else {
					if(i == 0)	equ->v_bou_[0][0] = (v_bou_T->second)[0][0][j];
					if(i == NX-1)	equ->v_bou_[0][1] = (v_bou_T->second)[0][1][j];
					if(j == 0)	equ->v_bou_[1][0] = (v_bou_T->second)[1][0][i];
					if(j == NY-1)	equ->v_bou_[1][1] = (v_bou_T->second)[1][1][i];
				}
			}
			// s
			{	
				auto equ = nface->create_equ("s", prob->equs_["s"]);

				auto v_bou_s = v_bou_.find("s");
				if(v_bou_s == v_bou_.cend()) {
					equ->v_bou_ = {{0,0},{0,0}};
				} else {
					if(i == 0)	equ->v_bou_[0][0] = v_bou_s->second[0][0][j];
					if(i == NX-1)	equ->v_bou_[0][1] = v_bou_s->second[0][1][j];
					if(j == 0)	equ->v_bou_[1][0] = v_bou_s->second[1][0][i];
					if(j == NY-1)	equ->v_bou_[1][1] = v_bou_s->second[1][1][i];
				}
				
				equ->flag_ |= ONLY_PARALLEL_FACES;
			}
		}
	}			

	grid_nbrs();

}
/*void		Patch::create_equ(std::string name, real v0, std::vector< array<real,1> > v_bou, real k, real al) {
	auto prob = group_.lock()->prob_.lock();
	for(auto f : *faces_) {
		f->create_equ(name, prob->equs_[name]);
	}
}*/
/*void		set_v_bou(std::string equ_name, v_bou_vec_type v_bou) {
  for(auto f : *faces_) {
  f->equs_[equ_name]->v_bou_ = v_bou;
  }
  }*/


// value statistics
real		Patch::max(std::string equ_name) {
	real v = -1E37;

	for(auto f : *faces_) {
		real a = f->equs_[equ_name]->max();
		real v = std::max(v,a);
		//print "a v",a,v
	}	
	return v;
}
void		Patch::grid_nbrs() {
	int nx = faces_->n_[0];
	int ny = faces_->n_[1];

	for(int i : range(nx)) {
		for(int j : range(ny)) {
			auto f1 = faces_->get(i,j);
			if(i > 0) {
				if(not f1->conns_[0][0])
					connect(f1, 0, 0, faces_->get(i-1,j), 0, 1);
			}
			if(i < (nx-1)) {
				if(not f1->conns_[0][1])
					connect(f1, 0, 1, faces_->get(i+1,j), 0, 0);
			}
			if(j > 0) {
				if(not f1->conns_[1][0])
					connect(f1, 1, 0, faces_->get(i,j-1), 1, 1);
			}
			if(j < (ny-1)) {
				if(not f1->conns_[1][1])
					connect(f1, 1, 1, faces_->get(i,j+1), 1, 0);
			}
		}
	}
}

void		Patch::write_binary(std::string equ_name) {

	std::ofstream ofs;
	ofs.open("binary_" + equ_name + "_" + name_ + ".txt", std::ofstream::trunc);

	if(!ofs.is_open()) {
		BOOST_LOG_CHANNEL_SEV(gal::log::lg, "Diff2D", warning) << "file stream not open" << std::endl;
		return;
	}
	
	math::basic_binary_oarchive ar(ofs);

	for(auto f : *faces_) {
		f->write_binary(equ_name, ar);
	}
}







