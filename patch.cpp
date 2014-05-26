/*from face import *
from util import *
*/
#include <algorithm>

#include "config.hpp"
#include "unit_vec.hpp"
#include "array.hpp"

class Patch: public LocalCoor {
	Patch(
			Patch_Group* group,
			std::string name, int normal, std::vector< std::vector<int> > indices, std::vector< array<real,1> > x, std::vector< array<int,1> > nx,
			std::map<std::string, array<real,1>[2][2]>v_bou):
		LocalCoor(normal)
	{
		
		group_ = group;
		name_ = name;

				
		
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
		faces_->zeros(NX,NY);

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
				
				auto ext = make_array<real,2>({
					{x[x_.i]->get(Is), x[x_.i]->get(Ms)},
					{x[y_.i]->get(Js), x[y_.i]->get(Ns)}
				});
				
				int numx = nx[x_.i]->get(std::min(I,M));
				int numy = nx[y_.i]->get(std::min(J,N));
				
				//print "I,J",I,J
				
				real pos_z = x[z_.i]->get(indices[z_.i]);
				
				
				faces_->get(i,j) = Face(normal, ext, pos_z, [numx, numy]);
			
				// unique to current setup
				// create temperature and source spreader equations
				
				
				faces[i,j].create_equ('T', group_.prob.equs['T']);
				
				faces[i,j].create_equ('s', group_.prob.equs['s']);
				faces[i,j].equs['s'].flag_ |= ONLY_PARALLEL_FACES;
				
				// alloc v_bou dict in face
				faces[i,j].v_bou = {};
				
				for k in v_bou.keys() {
					// alloc face v_bou list
					faces[i,j].v_bou[k] = [[0,0],[0,0]];
					
					// set face v_bou
					//logging.debug("".format(np.shape(v_bou[k][)))

					if i == 0:	faces[i,j].v_bou[k][0][0] = v_bou[k][0][0][j];
					if i == NX-1:	faces[i,j].v_bou[k][0][1] = v_bou[k][0][1][j];
					if j == 0:	faces[i,j].v_bou[k][1][0] = v_bou[k][1][0][i];
					if j == NY-1:	faces[i,j].v_bou[k][1][1] = v_bou[k][1][1][i];

				}
			}
		}			
				
		npatch_ = np.array([NX,NY])

		faces_ = faces;

		grid_nbrs()
	}
	void		create_equ(std::string name, real v0, std::vector< array<real,1> > v_bou, real k, real al) {
		for f in faces_.flatten() {
			f.create_equ(name, v0, v_bou, k, al)
	
		}
	}
	void		set_v_bou(std::string equ_name, std::vector< array<real,1> > v_bou) {
		for f in faces_.flatten() {
			f.equs[equ_name].v_bou = np.array(v_bou)
		}
	}
	

	// value statistics
	real		max(std::string equ_name) {
		v = float("-inf")

		for f in faces_.flatten() {
			a = f.equs[equ_name].max()
			v = max(v,a)
			print "a v",a,v
		}	
		return v
	}
	real		grid_nbrs() {
		nx,ny = np.shape(faces_)
	
		for i in range(nx) {
			for j in range(ny) {
				f1 = faces_[i,j]
				if(i > 0) {
					if not f1.conns[0,0]:
						connect(f1, 0, 0, faces_[i-1,j], 0, 1)
				}
				if i < (nx-1) {
					if not f1.conns[0,1]:
						connect(f1, 0, 1, faces_[i+1,j], 0, 0)
				}
				if(j > 0) {
					if not f1.conns[1,0]:
						connect(f1, 1, 0, faces_[i,j-1], 1, 1)
				}
				if j < (ny-1) {
					if not f1.conns[1,1]:
						connect(f1, 1, 1, faces_[i,j+1], 1, 0)
				}
			}
		}
	}

	Patch_Group*				group_;
	std::string				name_;
	std::vector< std::vector<int> >		indices_;
	array<Face*,2>				faces_;
};

