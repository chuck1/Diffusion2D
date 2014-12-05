
#include <map>
#include <algorithm>
#include <vector>

#include <math-array/array.hpp>


#include <Diff2D/config.hpp>
#include <Diff2D/face.hpp>
#include <Diff2D/unit_vec.hpp>
#include <Diff2D/util.hpp>
#include <Diff2D/boundary.hpp>


Patch::Patch(
		Patch_Group_s group,
		std::string name,
		int normal,
		math::multivec<2,size_t> indices,
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

	auto NX = indices_[x_.i].size() - 1;
	auto NY = indices_[y_.i].size() - 1;

	// expand scalar v_bou values
	/*		for k in v_bou.keys() {
			if isinstance(v_bou[k][0][0], float) { v_bou[k][0][0] = np.ones(NY)*v_bou[k][0][0]
			if isinstance(v_bou[k][0][1], float) { v_bou[k][0][1] = np.ones(NY)*v_bou[k][0][1]
			if isinstance(v_bou[k][1][0], float) { v_bou[k][1][0] = np.ones(NX)*v_bou[k][1][0]
			if isinstance(v_bou[k][1][1], float) { v_bou[k][1][1] = np.ones(NX)*v_bou[k][1][1]
			}*/

	// make sure indices are sorted properly

	std::sort(indices_[x_.i].begin(), indices_[x_.i].end());
	std::sort(indices_[y_.i].begin(), indices_[y_.i].end());

	if(normal < 0) {
		std::reverse(indices_[x_.i].begin(), indices_[x_.i].end());
		std::reverse(indices_[y_.i].begin(), indices_[y_.i].end());
	}

	npatch_ = math::make_array_1<size_t,1>({NX,NY});

	//faces_ = faces;

}
void		Patch::create_faces() {
	// alloc faces array
	faces_ = math::make_zeros<Face_s,2>(npatch_->ravel());

	size_t NX = npatch_->get(0);
	size_t NY = npatch_->get(1);

	//int Is, Js, Ms, Ns;
	size_t numx, numy;

	for(size_t i = 0; i < NX; ++i) {
		for(size_t j = 0; j < NY; ++j) {
			int I = indices_[x_.i][i];
			int J = indices_[y_.i][j];
			int M = indices_[x_.i][i+1];
			int N = indices_[y_.i][j+1];


			/*	if(Z_ < 0) {
				assert(I > M);
				assert(J > N);

				Is = M;
				Js = N;
				Ms = I;
				Ns = J;
				} else {
				assert(I < M);
				assert(J < N);

				Is = I;
				Js = J;
				Ms = M;
				Ns = N;
				}*/

			// extends are global coordinate associated with local begin and end
			auto ext = math::make_uninit<real,2>({2,2});
			ext->get(0,0) = coor_[x_.i]->get(I);
			ext->get(0,1) = coor_[x_.i]->get(M);
			ext->get(1,0) = coor_[y_.i]->get(J);
			ext->get(1,1) = coor_[y_.i]->get(N);
			/*ext->get(0,0) = coor_[x_.i]->get(Is);
			  ext->get(0,1) = coor_[x_.i]->get(Ms);
			  ext->get(1,0) = coor_[y_.i]->get(Js);
			  ext->get(1,1) = coor_[y_.i]->get(Ns);*/

			LOG_PATCH std::cout << "I M J N" << std::endl;
			LOG_PATCH std::cout << I << " " << M << " " << J << " " << N << std::endl;
			LOG_PATCH std::cout << "extents" << std::endl;
			LOG_PATCH std::cout
				<< std::setw(16) << "0,0"
				<< std::setw(16) << "0,1"
				<< std::setw(16) << "1,0"
				<< std::setw(16) << "1,1"
				<< std::endl;
			std::cout
				<< std::setw(16) << ext->get(0,0)
				<< std::setw(16) << ext->get(0,1)
				<< std::setw(16) << ext->get(1,0)
				<< std::setw(16) << ext->get(1,1)
				<< std::endl;

			numx = nx_[x_.i]->get(std::min(I,M));
			numy = nx_[y_.i]->get(std::min(J,N));

			auto numarr = math::make_array_1<size_t,1>({numx, numy});

			//print "I,J",I,J

			real pos_z = coor_[z_.i]->get(indices_[z_.i]);

			auto nface = std::make_shared<Face>(shared_from_this(), normal_, ext, pos_z, numarr);

			faces_->get(i,j) = nface;

			// unique to current setup
			// create temperature and source spreader equations

			auto prob = group_.lock()->prob_.lock();



			auto lsetup_bou = [&] (std::string name, unsigned int flag) {

				auto equ = nface->create_equ(name, prob->equs_[name]);
				assert(equ);

				auto v_bou_s = v_bou_.find(name);
				if(v_bou_s == v_bou_.cend()) {
					equ->v_bou_ = {{0,0},{0,0}};
				} else {
					assert((v_bou_s->second).size() == 2);
					assert((v_bou_s->second)[0].size() == 2);
					assert((v_bou_s->second)[1].size() == 2);

					assert(equ->v_bou_.size() == 2);
					assert(equ->v_bou_[0].size() == 2);
					assert(equ->v_bou_[1].size() == 2);


					auto lset_equ_bou = [&](
							size_t itmp,
							size_t jtmp,
							size_t vec_ind,
							size_t cmp_ind,
							size_t cmp_val,
							size_t vec_size_cmp)
					{

						std::vector< std::shared_ptr<boundary> >&	vec = (v_bou_s->second)[itmp][jtmp];
						std::shared_ptr<boundary>&			equ_bou = equ->v_bou_[itmp][jtmp];

						if(cmp_ind == cmp_val) {
							if(vec.size() == vec_size_cmp) {
								equ_bou = vec[vec_ind];
							} else if(vec.size() == 1) {
								equ_bou = vec[0];
							} else if(vec.empty()) {
								vec.push_back(std::make_shared<boundary_insulated>());
								equ_bou = vec[0];
							} else {
								std::cout << "boundary vector[][] must be of size 1 or equal to number of faces along direction in patch" << std::endl;
								abort();
							}
						}
					};

					lset_equ_bou(0, 0, j, i, 0,	NY);
					lset_equ_bou(0, 1, j, i, NX-1,	NY);
					lset_equ_bou(1, 0, i, j, 0,	NX);		
					lset_equ_bou(1, 1, i, j, NY-1,	NX);
				}

				equ->flag_ |= flag;
			};

			lsetup_bou("T",0);
			lsetup_bou("s",ONLY_PARALLEL_FACES);
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
real				Patch::min(std::string const & name) const {
	real v = 1E37;
	for(auto f : *faces_) {
		auto e = f->equs_[name];
		assert(e);
		v = std::min(v,e->min());
	}
	return v;
}
real				Patch::max(std::string const & name) const {
	real v = -1E37;
	for(auto f : *faces_) {
		auto e = f->equs_[name];
		assert(e);
		v = std::max(v,e->max());
	}
	return v;
}
void		Patch::grid_nbrs() {
#ifdef _DEBUG
	std::cout << "grid_nbrs" << std::endl;
	print_row(8,"name","Z");
	print_row(8,name_,Z_);
#endif

	size_t nx = faces_->n_[0];
	size_t ny = faces_->n_[1];

	for(size_t i : range(nx)) {
		for(size_t j : range(ny)) {
			int is = i;
			int js = j;
#ifdef _DEBUG
			print_row(4,"i","j","is","js");
			print_row(4,i,j,is,js);
#endif
			auto f1 = faces_->get(i,j);
			
			auto lconnect = [&] (bool criteria, size_t i, size_t s, int i_off, int j_off) {
				int i2 = is + i_off;
				int j2 = js + j_off;
				if(criteria) {
					print_row(4,"i2","j2",i2,j2);

					auto f2 = faces_->get(i2, j2);
					try {
						connect(f1, i, s, f2, i, ((s==0)?1:0));
					} catch(conns_not_zero e) {
					} catch(...) {
						std::cout << "unknwon error" << std::endl;
						abort();
					}
				}
			};
			
			assert(i >= 0);
			assert(j >= 0);

			lconnect(((size_t)i) > 0, 	0, 0, -1,  0);
			lconnect(((size_t)i) < (nx-1), 	0, 1,  1,  0);
			lconnect(((size_t)j) > 0, 	1, 0,  0, -1);
			lconnect(((size_t)j) < (ny-1),	1, 1,  0,  1);

		}
	}
}
void		Patch::connection_info() const {
	std::cout << "patch " << name_ << std::endl;
	for(size_t i : range(faces_->n_[0])) {
		for(size_t j : range(faces_->n_[1])) {
			std::cout << "\tface[" << i << "][" << j << "]" << std::endl;

			auto f = faces_->get(i,j);
			for(auto ic : range(2)) {
				for(auto jc : range(2)) {
					auto c = f->conns_[ic][jc];
					if(c) {
						std::cout << "\t\tconn["<< ic <<"]["<< jc <<"] = face in patch '" << c->twin_->face_->patch_->name_ << "'" << std::endl;
					}
				}
			}
		}
	}
}
void		Patch::info_geom() const {
	
	int I = *std::min_element(indices_[x_.i].cbegin(), indices_[x_.i].cend());
	int M = *std::max_element(indices_[x_.i].cbegin(), indices_[x_.i].cend());
	int J = *std::min_element(indices_[y_.i].cbegin(), indices_[y_.i].cend());
	int N = *std::max_element(indices_[y_.i].cbegin(), indices_[y_.i].cend());
	

	
	real xm = coor_[x_.i]->get(I);
	real xp = coor_[x_.i]->get(M);
	real ym = coor_[y_.i]->get(J);
	real yp = coor_[y_.i]->get(N);

	print_row(16,"xm","xp","ym","yp");
	print_row(16, I, M, J, N);

	print_row(16,"xm","xp","ym","yp");
	print_row(16,xm,xp,ym,yp);

	real z = coor_[z_.i]->get(indices_[z_.i][0]);

	print_row(16, "z");
	print_row(16, z);

}
void		Patch::write_binary(std::string ename) {

	// construct names

	auto group = group_.lock();
	assert(group);
	auto prob = group->prob_.lock();
	assert(prob);

	auto dir = prob->mkdir(ename);
	auto filename = dir + "/binary_" + name_ + ".txt";
	
	std::ofstream ofs;
	ofs.open(filename, std::ofstream::trunc);
	if(!ofs.is_open()) {
		LOG(lg, info, warning) << "file stream not open";
		return;
	}

	math::basic_binary_oarchive ar(ofs);

	for(auto f : *faces_) {
		f->write_binary(ename, ar);
	}
}







