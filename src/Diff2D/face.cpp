#include <map>
#include <memory>
#include <vector>
#include <iostream>
#include <iomanip>

#include <math-array/array.hpp>

#include <Diff2D/config.hpp>
#include <Diff2D/unit_vec.hpp>
#include <Diff2D/equation.hpp>	
#include <Diff2D/conn.hpp>
#include <Diff2D/index_lambda.hpp>
#include <Diff2D/patch.hpp>
#include <Diff2D/patch_group.hpp>
#include <Diff2D/face.hpp>
#include <Diff2D/boundary.hpp>



Face::Face(Patch_s patch, int normal, array<real,2> const & ext, real pos_z, array<size_t,1> n):
	LocalCoor(normal),
	patch_(patch),
	ext_(ext),
	conns_({{0,0},{0,0}}),
	pos_z_(pos_z),
	n_(n),
	d_(make_zeros<real,3>({n_->get(0)+2, n_->get(1)+2, 2}))
{
	LOG_FACE std::cout << "Face ctor" << std::endl;

	assert(patch);
	assert(ext);
	assert(n);

	//// the extra 2 rows/cols are for storing neighbor values
	//array<size_t,1> n_extended = n_->add(make_array_1<size_t,1>({2, 2}));
	
	//d_ = make_zeros<real,3>({n_extended->get(0), n_extended->get(1), 2});

	for(size_t i = 0; i < n_->get(0)+2; ++i) {
		for(size_t j = 0; j < n_->get(1)+2; ++j) {
			for(size_t k = 0; k < 2; ++k) {
				d_->get(i,j,k) = fabs(
						(ext_->get(k,1) - ext_->get(k,0)) / ((real)n_->get(k))
						);
				//LOG_FACE std::cout << "d " << d_->get(i,j,k) << std::endl;
			}
		}
	}

	/*if np.any(d_ < 0) {
	//print d_
	//raise ValueError('bad') }*/


	// source
	l_ = make_uninit<real,1>({2});
	l_->get(0) = (ext_->get(0,1) - ext_->get(0,0)) / 2.0;
	l_->get(1) = (ext_->get(1,1) - ext_->get(1,0)) / 2.0;


	//real a = l_->get(0) / 2.0;
	//real b = l_->get(1) / 2.0;

	//x = linspace(d_.get(0,0,0) / 2. - a, a - d_.get(0,0,0) / 2., n_.get(0))
	//y = linspace(d_.get(0,0,1) / 2. - b, b - d_.get(0,0,1) / 2., n_.get(1))


	//Tmean_ = []
}
Equation_s		Face::create_equ(std::string name, Equation_Prob_s equ_prob) {
	auto equ = std::make_shared<Equation>(name, shared_from_this(), equ_prob);
	equs_[name] = equ;
	return equ;
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

	for(size_t a = 0; a < n_->get(conn->pl_.i); ++a) {
		v->get(a) = equ->v_->get(conn->il_(0,a), conn->il_(1,a));
	}
	conn->send(equ->name_, v);
}	
void		Face::recv_array(Equation_s equ, Conn_s conn) {
	assert(equ);
	assert(equ->v_);

	auto v = conn->recv(equ->name_);
	assert(v);

	int ind[] = {0,0};
	ind[conn->ol_.i] = (conn->ol_.s < 0) ? -1 : n_->get(conn->ol_.i);

	for(size_t a = 0; a < n_->get(conn->pl_.i); ++a) {
		ind[conn->pl_.i] = a;
		equ->v_->get(ind[0],ind[1]) = v->get(a);
	}
}	
Term		Face::term(Equation_s equ, std::vector<int> ind, int v, int sv, real To) {
	// get the value and coefficienct for the cell adjacent to ind in the direction V

	real d = d_->get(ind[0],ind[1],v);
	IF(d <= 0) {
		LOG_FACE std::cout << "d get "
			<< std::setw(8) << ind[0] << std::setw(8) << ind[1] << std::setw(8) << v
			<< " = " << d << std::endl;
		throw 0;
	}

	// interior cells and boundary cells with Face neighbors
	int indnbr[] = {ind[0], ind[1]};

	indnbr[v] += sv;

	Term t;

	t.y = equ->v_->get(indnbr[0],indnbr[1]);

	//print ind,indnbr
	//print "T",T,"To",To

	real d_nbr = d_->get(indnbr[0],indnbr[1],v);
	
	t.a = 2.0 / (d + d_nbr);
	
	if(std::isnan(t.a) || std::isinf(t.a)) {
		LOG_FACE std::cout << std::setw(16) << "d" << std::setw(16) << "d_nbr" << std::endl;
		LOG_FACE std::cout << std::setw(16) << d << std::setw(16) << d_nbr << std::endl;
		throw 0;
	}
	return t;
}
void		Face::step_pre_cell(Equation_s equ, std::vector<int> ind, int V) {
	assert(equ);
	
	// set the v-array value for the boundary value at ind+V
	IS v(V);

	Conn_s conn = loc_to_conn(V);

	auto ldebug = [&] () {
		LOG_FACE std::cout << "face::step_pre_cell" << std::endl;
		LOG_FACE std::cout << "this" << "Z" << "V" << std::endl;
		LOG_FACE std::cout << this << " " << Z_ << " " << V << std::endl;
		LOG_FACE std::cout << "conn = " << conn << std::endl;
		LOG_FACE std::cout << "ind  = " << ind[0] << " " << ind[1] << std::endl;
		//LOG_FACE std::cout << "Z = " << Z_ << std::endl;

	};

	if(Z_ == 3)// && conn == 0)
		ldebug();

	if(conn) {
		assert(conn->twin_);
		assert(conn->twin_->face_);
		if(equ->flag_ & ONLY_PARALLEL_FACES) {
			auto g1 = patch_->group_.lock();
			auto g2 = conn->twin_->face_->patch_->group_.lock();

			if(g1 == g2) {
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
	std::vector<int> indn({ind[0], ind[1]});
	indn[v.i] += v.s;

	int p = (v.i == 1) ? 0 : 1;

	/*logging.debug("V    {0}".format(V))
	  logging.debug("ind  {0}".format(ind))
	  logging.debug("indn {0}".format(indn))*/

	// get stored boundary value
	assert(equ);
	assert(equ->v_bou_.size() == 2);


	std::shared_ptr<boundary>& v_bou_obj = equ->v_bou_[v.i][(v.s+1)/2];
	if(!v_bou_obj) {
		v_bou_obj = std::make_shared<boundary_insulated>();
	}

	v_bou_obj->eval(equ, ind, indn, p);

}
void		Face::step_pre(Equation_s equ) {
	// for boundaries, load boundary temperature cells with proper value
	// west/east
	for(size_t j = 0; j < n_->get(1); ++j) {
		step_pre_cell(equ, {                  0, (int)j}, -1);
		step_pre_cell(equ, {(int)n_->get(0) - 1, (int)j},  1);
	}
	// north/south
	for(size_t i = 0; i < n_->get(0); ++i) {
		step_pre_cell(equ, {(int)i,           0}, -2);
		step_pre_cell(equ, {(int)i, (int)n_->get(1)-1},  2);
	}
}
real		Face::step(std::string equ_name, size_t it_outer) {
	Equation_s equ = equs_[equ_name];
	// solve diffusion equation for equ
	real R = 0.0;

	bool ver1 = false;
	//bool ver2 = false;

	// solve equation
	step_pre(equ);

	auto g = patch_->group_.lock();
	auto S = g->S_[equ->name_]->get(it_outer);

	BOOST_LOG_CHANNEL_SEV(gal::log::lg, "Diff2D", debug) << "face step S = " << S << GAL_LOG_ENDLINE;

	////print "face S",S
	/*
	   void debug_s() {
	   print "face s",s
	   print "s",equ.s[i,j]
	   print "A",A
	   print "k",equ.equ_prob.k
	   }*/
	for(int i : range(n_->get(0))) {
		for(int j : range(n_->get(1))) {
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

			auto ldebug = [&] () {
				LOG_FACE std::cout << std::scientific;

				LOG_FACE std::cout << std::setw(16) << "termW.a" << std::setw(16) << "termE.a" << std::setw(16) << "termS.a" << std::setw(16) << "termN.a" << std::endl;
				LOG_FACE std::cout << std::setw(16) << termW.a << std::setw(16) << termE.a << std::setw(16) << termS.a << std::setw(16) << termN.a << std::endl;

				LOG_FACE std::cout
					<< std::setw(16) << "termW.y" << std::setw(16) << "termE.y" << std::setw(16) << "termS.y" << std::setw(16) << "termN.y"
					<< std::setw(16) << "yo" << std::setw(16) << "ys" << std::setw(16) << "dy" << std::endl;

				LOG_FACE std::cout
					<< std::setw(16) << termW.y << std::setw(16) << termE.y << std::setw(16) << termS.y << std::setw(16) << termN.y
					<< std::setw(16) << yo << std::setw(16) << ys << std::setw(16) << dy << std::endl;

			};

			if(termW.a < 0 or termE.a < 0 or termS.a < 0 or termN.a < 0) {
				ldebug();
				throw 0;
			}

			if (ver1) {
				//debug();
			}

			if(std::isnan(yo)) { ldebug(); throw 0; }
			if(std::isinf(yo)) { ldebug(); throw 0; }

			if(std::isnan(ys) || std::isinf(ys)) {
				ldebug();
				throw 0;
			}
			if(std::isnan(dy)) throw 0;

			equ->v_->get(i,j) += dy;

			real nR;
			if(yo == 0.0) {
				BOOST_LOG_CHANNEL_SEV(gal::log::lg, "Diff2D", debug) << "yo is zero" << GAL_LOG_ENDLINE;
				//debug();
			} else {
				nR = fabs(dy/yo);

				if (dy == 0.0) {
				} else {
					R = std::max(nR, R);
				}
			}



			IF(std::isnan(R)) {
				BOOST_LOG_CHANNEL_SEV(gal::log::lg, "Diff2D", critical) << "R is nan" << GAL_LOG_ENDLINE;
				ldebug();
				throw 0;
			}
			IF(std::isinf(R)) {
				BOOST_LOG_CHANNEL_SEV(gal::log::lg, "Diff2D", critical) << "R is inf" << GAL_LOG_ENDLINE;
				ldebug();
				throw 0;
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
grid_return_type	Face::grid(std::string equ_name) {

	auto x = linspace(ext_->get(0,0), ext_->get(0,1), n_->get(0));
	auto y = linspace(ext_->get(1,0), ext_->get(1,1), n_->get(1));

	auto grid = meshgrid(x, y);

	auto X = grid.first;
	auto Y = grid.second;

	auto Z = make_ones<real,2>(X->shape());
	Z->multiply_self(pos_z_);

	auto equ = equs_[equ_name];

	auto W = equ->v_->sub({0,0},{-2,-2});


	/*	if(z_.s > 0) {
		W->transpose_self();
		} else {
		W->rot90_self(1);
		W->fliplr_self();
		}*/


	return grid_return_type(X,Y,Z,W);
}

void		Face::write_binary(std::string equ_name, math::basic_binary_oarchive& ar) {

	auto g = grid(equ_name);

	ar << x_.i;
	ar << y_.i;
	ar << z_.i;

	g.X[0]->serialize(ar, 0);
	g.X[1]->serialize(ar, 0);
	g.X[2]->serialize(ar, 0);

	g.W->serialize(ar, 0);

}



