
#include <map>
#include <memory>
#include <vector>

#include "math.hpp"
#include "array.hpp"
#include "config.hpp"
#include "unit_vec.hpp"
#include "equation.hpp"	
#include "conn.hpp"
#include "index_lambda.hpp"
#include "patch.hpp"
#include "patch_group.hpp"
#include "face.hpp"

Face::Face(Patch_s patch, int normal, array<real,2> const & ext, real pos_z, array<int,1> n): LocalCoor(normal) {

	patch_ = patch;

	ext_ = ext;

	pos_z_ = pos_z;

	n_ = n;

	//// the extra 2 rows/cols are for storing neighbor values
	array<int,1> n_extended = n_->add(make_array<int,1>({2, 2}));
	
	d_->alloc({n_extended->get(0), n_extended->get(1), 2});
	d_->zeros();

	for(int i = 0; i < n_extended->n_[0]; ++i) {
		for(int j = 0; j < n_extended->n_[1]; ++j) {
			for(int k = 0; k < 2; ++k) {
				d_->get(i,j,k) = (ext_->get(k,1) - ext_->get(k,0)) / ((real)n_->get(k));
			}
		}
	}

	/*if np.any(d_ < 0) {
	//print d_
	//raise ValueError('bad') }*/

	//conns_ = np.empty((2,2), dtype=object);

	//// source
	l_->get(0) = (ext_->get(0,1) - ext_->get(0,0)) / 2.0;
	l_->get(1) = (ext_->get(1,1) - ext_->get(1,0)) / 2.0;


	real a = l_->get(0) / 2.0;
	real b = l_->get(1) / 2.0;

	//x = linspace(d_.get(0,0,0) / 2. - a, a - d_.get(0,0,0) / 2., n_.get(0))
	//y = linspace(d_.get(0,0,1) / 2. - b, b - d_.get(0,0,1) / 2., n_.get(1))
	//Y,X = np.meshgrid(y,x)


	//Tmean_ = []
}
void		Face::create_equ(std::string name, Equation_Prob_s equ_prob) {
	equs_[name].reset(new Equation(name, shared_from_this(), equ_prob));
}
int		Face::get_loc_pos_par_index(Face_s nbr) {
	int OL = nbr_to_loc(nbr);

	int PL = abs(cross(3, OL));

	int PG = loc_to_glo(PL);
	return PG;
}	
real		Face::x(int i) {
	return (i + 0.5) * d_->get(0);
}
real		Face::y(int j) {
	return (j + 0.5) * d_->get(1);
}
real		Face::area() {
	return (ext_->get(0,1) - ext_->get(0,0)) * (ext_->get(1,1) - ext_->get(1,0));
}
int		Face::nbr_to_loc(Face_s nbr) {
	//if(not nbr) raise ValueError('nbr is None')

	for(int i = 0; i < 2; ++i) {
		for(int j = 0; j < 2; ++j) {
			Conn_s conn = conns_[i][j];
			if(conn) {
				if(nbr == conn->twin_->face_) {
					return IS(i,2*j-1).v();
				}
			}
		}
	}
	//print [conn.nbr if conn else conn for conn in conns_.flatten()]
	//print nbr
	//raise ValueError('nbr not found')
	throw 0;
}
Conn_s			Face::loc_to_conn(int V) {
	IS v(V);
	return conns_[v.i][(v.s+1)/2];
}
Index_Lambda		Face::index_lambda(Face_s nbr) {
	// returns lambda:
	// function of positive parallel index of neighbor
	// returns the index of my cell

	// PG global index of neighbor's positive parallel local index
	int PG = nbr->get_loc_pos_par_index(shared_from_this());

	int PL = glo_to_loc(PG);
	int OL = nbr_to_loc(nbr);

	IS pl(PL);//pl,spl = v2is(PL);
	IS ol(OL);//ol,sol = v2is(OL);

	real d = d_->get(ol.i);

	Index_Lambda il;
	il.a[pl.i] = (pl.s > 0) ? 1 : -1;
	il.b[pl.i] = (pl.s > 0) ? 0 : (n_->get(pl.i) - 1);

	il.a[ol.i] = 0;
	il.b[ol.i] = (ol.s < 0) ? 0 : (n_->get(ol.i) - 1);

	il.d = d;

	return il;
}
void		Face::send_array(Equation_s equ, Conn_s conn) {
	auto v = make_ones<real,1>({n_->get(conn->pl_.i)});

	for(int a = 0; a < n_->get(conn->pl_.i); ++a) {
		v->get(a) = equ->v_->get(conn->il_(0,a), conn->il_(1,a));
	}
	conn->send(equ->name_, v);
}	
void		Face::recv_array(Equation_s equ, Conn_s conn) {
	auto v = conn->recv(equ->name_);

	int ind[] = {0,0};
	ind[conn->ol_.i] = (conn->ol_.s < 0) ? -1 : n_->get(conn->ol_.i);

	for(int a = 0; a < n_->get(conn->pl_.i); ++a) {
		ind[conn->pl_.i] = a;
		equ->v_->get(ind[0],ind[1]) = v->get(a);
	}
}	
Term		Face::term(Equation_s equ, std::vector<int> ind, int v, int sv, real To) {
	// get the value and coefficienct for the cell adjacent to ind in the direction V

	real d = d_->get(ind[0],ind[1],v);

	// interior cells and boundary cells with Face neighbors
	int indnbr[] = {ind[0], ind[1]};

	indnbr[v] += sv;

	Term t;

	t.y = equ->v_->get(indnbr[0],indnbr[1]);

	//print ind,indnbr
	//print "T",T,"To",To

	real d_nbr = d_->get(indnbr[0],indnbr[1],v);

	t.a = 2.0 / (d + d_nbr);
	return t;
}
void		Face::step_pre_cell(Equation_s equ, std::vector<int> ind, int V) {
	// set the v-array value for the boundary value at ind+V
	IS v(V);

	Conn_s conn = loc_to_conn(V);
	if(conn) {
		if(equ->flag_ & ONLY_PARALLEL_FACES) {
			if(conn->twin_->face_->Z_ == Z_) {
				recv_array(equ, conn);
			} else {
				step_pre_cell_open_bou(equ, ind, V);
			}
		} else {
			recv_array(equ, conn);
		}
	} else {
		step_pre_cell_open_bou(equ, ind, V);
	}
}
void		Face::step_pre_cell_open_bou(Equation_s equ, std::vector<int> ind, int V) {
	//logging.debug('step_pre_cell_open_bou')

	IS v(V);
	int indn[] = {ind[0], ind[1]};
	indn[v.i] += v.s;

	int p = (v.i == 1) ? 0 : 1;

	/*logging.debug("V    {0}".format(V))
	  logging.debug("ind  {0}".format(ind))
	  logging.debug("indn {0}".format(indn))*/

	// get stored boundary value
	auto tmp = equ->v_bou_[v.i];
	auto tmp2 = tmp[(v.s+1)/2];
	auto v_bou_ar = equ->v_bou_[v.i][(v.s+1)/2];

	real v_bou = v_bou_ar->get(ind[p]);

	if(v_bou == 0.0) { // insulated
		equ->v_->get(indn[0],indn[1]) = equ->v_->get(ind);
	} else { //// constant temperature
		equ->v_->get(indn[0],indn[1]) = 2.0 * v_bou - equ->v_->get(ind);
	}

}
void		Face::step_pre(Equation_s equ) {
	// for boundaries, load boundary temperature cells with proper value
	// west/east
	for(int j = 0; j < n_->get(1); ++j) {
		step_pre_cell(equ, {             0, j}, -1);
		step_pre_cell(equ, {n_->get(0) - 1, j},  1);
	}
	// north/south
	for(int i = 0; i < n_->get(0); ++i) {
		step_pre_cell(equ, {i,           0}, -2);
		step_pre_cell(equ, {i, n_->get(1)-1},  2);
	}
}
real		Face::step(std::string equ_name) {
	Equation_s equ = equs_[equ_name];
	// solve diffusion equation for equ
	real R = 0.0;

	bool ver1 = false;
	bool ver2 = false;
	// solve equation
	step_pre(equ);

	auto g = patch_->group_;
	auto S = g->S_[equ->name_];

	////print "face S",S
	/*
	   void debug_s() {
	   print "face s",s
	   print "s",equ.s[i,j]
	   print "A",A
	   print "k",equ.equ_prob.k
	   }*/
	for(auto i : range(n_->get(0))) {
		for(auto j : range(n_->get(1))) {
			real A = d_->get(i,j,0) * d_->get(i,j,1);

			real yo = equ->v_->get(i,j);

			auto termW = term(equ, {i,j}, 0, -1, yo);
			auto termE = term(equ, {i,j},0, 1, yo);
			auto termS = term(equ, {i,j},1,-1, yo);
			auto termN = term(equ, {i,j},1, 1, yo);

			//ver = True
			//print "source =",s(To)

			// source term
			real s = equ->s_->get(i,j) * S * A / equ->equ_prob_->k_;

			//if s < 0:
			//debug_s()

			real num = termW.prod() + termE.prod() + termS.prod() + termN.prod() + s;
			real den = termW.a + termE.a + termS.a + termN.a;

			real ys = num / den;

			real dy = equ->equ_prob_->alpha_ * (ys - yo);

			/*def debug() {
			  print "aW aE aS aN"
			  print aW, aE, aS, aN
			  print "yW yE yS yN yo ys dy"
			  print yW, yE, yS, yN, yo, ys, dy
			  }*/
			////debug()

			if(termW.a < 0 or termE.a < 0 or termS.a < 0 or termN.a < 0) {
				//debug();
				throw 0;
			}

			if (ver1) {
				//debug();
			}
			if(std::isnan(yo)) throw 0;

			if(std::isnan(ys) or std::isinf(ys)) {
				//debug();
				throw 0;
			}
			if(std::isnan(dy)) throw 0;

			equ->v_->get(i,j) += dy;

			if (dy == 0.0) {
			} else {
				R = std::max(fabs(dy/yo), R);
			}

			if(std::isnan(R)) {
				//print 'dy',dy,'yo',yo
				throw 0;//raise ValueError('nan')
			}
		}
	}
	return R;
}
void		Face::send(std::string equ_name) {
	// send/recv neighbor values
	auto equ = equs_[equ_name];

	for(auto c1 : conns_) {
		for(auto con : c1) {
			if(con)	send_array(equ, con);
		}
	}
}
void		Face::recv(std::string equ_name) {
	auto equ = equs_[equ_name];
	
	for(auto c1 : conns_) {
		for(auto con : c1) {
			if(con) recv_array(equ, con);
		}
	}
}







