#include <map>
#include <algorithm>
#include <vector>

#include <Diff2D/config.hh>
#include <Diff2D/face.hpp>
#include <Diff2D/unit_vec.hpp>
#include <Diff2D/array.hpp>




Patch::Patch(
		Patch_Group_s group,
		std::string name,
		int normal,
		std::vector< std::vector<int> > indices,
		std::vector< array<real,1> > x,
		std::vector< array<int,1> > nx,
		v_bou_type v_bou):
	LocalCoor(normal)
{

	group_ = group;
	name_ = name;

	//v_bou_ = v_bou;

	//if not np.shape(v_bou_) == (2,2) {
	//	print v_bou_
	//	raise ValueError('')

	//print 'T_0',T_0

	indices_ = indices;

	int NX = indices[x_.i].size() - 1;
	int NY = indices[y_.i].size() - 1;

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

	// alloc faces array
	faces_->alloc({NX,NY});
	faces_->zeros();

	for(int i = 0; i < NX; ++i) {
		for(int j = 0; j < NY; ++j) {
			int I = indices[x_.i][i];
			int J = indices[y_.i][j];
			int M = indices[x_.i][i+1];
			int N = indices[y_.i][j+1];

			int Is = std::min(I,M);
			int Js = std::min(J,N);
			int Ms = std::max(I,M);
			int Ns = std::max(J,N);

			auto ext = make_uninit<real,2>({2,2});
			ext->get(0,0) = x[x_.i]->get(Is);
			ext->get(0,1) = x[x_.i]->get(Ms);
			ext->get(1,0) = x[y_.i]->get(Js);
			ext->get(1,1) = x[y_.i]->get(Ns);


			int numx = nx[x_.i]->get(std::min(I,M));
			int numy = nx[y_.i]->get(std::min(J,N));

			auto numarr = make_array<int,1>({numx, numy});

			//print "I,J",I,J

			real pos_z = x[z_.i]->get(indices[z_.i]);

			auto nface = std::make_shared<Face>(shared_from_this(), normal, ext, pos_z, numarr);

			faces_->get(i,j) = nface;

			// unique to current setup
			// create temperature and source spreader equations

			auto prob = group_.lock()->prob_.lock();

			nface->create_equ("T", prob->equs_["T"]);

			nface->create_equ("s", prob->equs_["s"]);

			nface->equs_["s"]->flag_ |= ONLY_PARALLEL_FACES;

			// alloc v_bou dict in face
			//nface->v_bou = {};

			for(auto it : v_bou) {
				// alloc face v_bou list
				//nface->v_bou_[it.first] = [[0,0],[0,0]];

				// set face v_bou
				//logging.debug("".format(np.shape(v_bou[k][)))

				std::string k = it.first;

				if(i == 0)	nface->v_bou_[k][0][0] = v_bou[k][0][0][j];
				if(i == NX-1)	nface->v_bou_[k][0][1] = v_bou[k][0][1][j];
				if(j == 0)	nface->v_bou_[k][1][0] = v_bou[k][1][0][i];
				if(j == NY-1)	nface->v_bou_[k][1][1] = v_bou[k][1][1][i];

			}
		}
	}			

	npatch_ = make_uninit<int,1>({2});
	npatch_->get(0) = NX;
	npatch_->get(0) = NY;

	//faces_ = faces;

	grid_nbrs();
}
void		Patch::create_equ(std::string name, real v0, std::vector< array<real,1> > v_bou, real k, real al) {
	auto prob = group_.lock()->prob_.lock();
	for(auto f : *faces_) {
		f->create_equ(name, prob->equs_[name]);
	}
}
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




