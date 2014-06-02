#define _POSIX_SOURCE
#include <sys/stat.h>

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

	//int Is, Js, Ms, Ns;
	size_t numx, numy;

	for(size_t i = 0; i < NX; ++i) {
		for(size_t j = 0; j < NY; ++j) {
			int I = indices_[x_.i][i];
			int J = indices_[y_.i][j];
			int M = indices_[x_.i][i+1];
			int N = indices_[y_.i][j+1];

			std::cout << "I M J N" << std::endl;
			std::cout << I << " " << M << " " << J << " " << N << std::endl;

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
			auto ext = make_uninit<real,2>({2,2});
			ext->get(0,0) = coor_[x_.i]->get(I);
			ext->get(0,1) = coor_[x_.i]->get(M);
			ext->get(1,0) = coor_[y_.i]->get(J);
			ext->get(1,1) = coor_[y_.i]->get(N);
			/*ext->get(0,0) = coor_[x_.i]->get(Is);
			  ext->get(0,1) = coor_[x_.i]->get(Ms);
			  ext->get(1,0) = coor_[y_.i]->get(Js);
			  ext->get(1,1) = coor_[y_.i]->get(Ns);*/

			std::cout << "extendts" << std::endl;
			std::cout
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

			auto numarr = make_array_1<size_t,1>({numx, numy});

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

	// construct names
	
	auto group = group_.lock();
	assert(group);
	auto prob = group->prob_.lock();
	assert(prob);

	std::string dir1 = "output";
	std::string dir2 = dir1 + "/" + prob->name_;
	std::string filename = dir2 + "/binary_" + equ_name + "_" + name_ + ".txt";
	
	// create folders
	
	struct stat sb;

	if(stat(dir1.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode)) {
	} else {
		if(mkdir(dir1.c_str(),S_IRWXU|S_IRGRP|S_IXGRP) != 0) {
			perror("mkdir error:");
			return;
		}
	}

	if(stat(dir2.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode)) {
	} else {
		if(mkdir(dir2.c_str(),S_IRWXU|S_IRGRP|S_IXGRP) != 0) {
			perror("mkdir error:");
			return;
		}
	}
	
	std::ofstream ofs;
	ofs.open(filename, std::ofstream::trunc);

	if(!ofs.is_open()) {
		BOOST_LOG_CHANNEL_SEV(gal::log::lg, "Diff2D", warning) << "file stream not open" << std::endl;
		return;
	}

	math::basic_binary_oarchive ar(ofs);

	for(auto f : *faces_) {
		f->write_binary(equ_name, ar);
	}
}







