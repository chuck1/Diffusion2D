#include "array.hpp"
#include "config.hpp"
#include "unit_vec.hpp"
#include "equation.hpp"	


Face::Face(Patch* patch, int normal, array<real,2> const & ext, real pos_z, array<int,1> n): LocalCoor(normal) {

	patch_ = patch;

	ext_ = ext;

	pos_z_ = pos_z;

	n_ = n;

	//// the extra 2 rows/cols are for storing neighbor values
	array<int,1> n_extended = n_ + array<int,1>({2, 2});


	d_.zeros(n_extended.get(0), n_extended.get(1), 2);
	for(int i = 0; i < n_extended.n_[0]; ++i) {
		for(int j = 0; j < n_extended.n_[1]; ++j) {
			for(int k = 0; k < 2; ++k) {
				d_.get(i,j,k) = (ext_.get(k,1) - ext_.get(k,0)) / ((float)n_.get(k));
			}
		}
	}


	/*if np.any(d_ < 0) {
	//print d_
	//raise ValueError('bad') }*/

	//conns_ = np.empty((2,2), dtype=object);

	//// source
	l_.get(0) = (ext_.get(0,1) - ext_.get(0,0)) / 2.0;
	l_.get(1) = (ext_.get(1,1) - ext_.get(1,0)) / 2.0;


	real a = l_.get(0) / 2.0;
	real b = l_.get(1) / 2.0;

	//x = linspace(d_.get(0,0,0) / 2. - a, a - d_.get(0,0,0) / 2., n_.get(0))
	//y = linspace(d_.get(0,0,1) / 2. - b, b - d_.get(0,0,1) / 2., n_.get(1))
	//Y,X = np.meshgrid(y,x)


	//Tmean_ = []
}
void		Face::create_equ(std::string name, Equation_Prob* equ_prob) {
	equs_[name] = new Equation(name, this, equ_prob);
}
int		Face::get_loc_pos_par_index(Face* nbr) {
	int OL = nbr_to_loc(nbr);

	int PL = abs(cross(3, OL));

	int PG = loc_to_glo(PL);
	return PG;
}	
real		Face::x(int i) {
	return (i + 0.5) * d_.get(0);
}
real		Face::y(int j) {
	return (j + 0.5) * d_.get(1);
}
real		Face::area() {
	return (ext_.get(0,1) - ext_.get(0,0)) * (ext_.get(1,1) - ext_.get(1,0));
}
int		Face::nbr_to_loc(Face* nbr) {
	//if(not nbr) raise ValueError('nbr is None')

	for(int i = 0; i < 2; ++i) {
		for(int j = 0; j < 2; ++j) {
			Conn* conn = conns_[i][j];
			if(conn) {
				if(nbr == conn->twin_->face) {
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
Conn*		Face::loc_to_conn(int V) {
	v,sv = v2is(V)
		return conns_[v, (sv+1)/2]
}
int		Face::index_lambda(Face* nbr) {
	// returns lambda:
	// function of positive parallel index of neighbor
	// returns the index of my cell

	// PG global index of neighbor's positive parallel local index
	PG = nbr.get_loc_pos_par_index()

		PL = glo_to_loc(PG)
		OL = nbr_to_loc(nbr)

		pl,spl = v2is(PL)
		ol,sol = v2is(OL)

		d = d_[ol]

		l = [None,None]
		l[pl] = lambda p, pl=pl, spl=spl: p if (spl > 0) else n_[pl] - p - 1
		l[ol] = lambda p, ol=ol, sol=sol: 0 if (sol < 0) else (n_[ol] - 1)

		//print inspect.getsource(i)
		//print inspect.getsource(j)
		//print l

		return l[0],l[1],d
}
void		Face::send_array(Equation* equ, Conn* conn) {

	v = np.ones(n_[conn.pl])

		for a in range(n_[conn.pl]) {
			v[a] = equ.v[conn.li(a), conn.lj(a)]
		}
	conn.send(equ.name, v)
}	
void		Face::recv_array(Equation* equ, Conn* conn) {
	v = conn.recv(equ.name)

		ind = [0,0]
		ind[conn.ol] = -1 if conn.sol < 0 else n_[conn.ol]

		for a in range(n_[conn.pl]) {
			ind[conn.pl] = a

				equ.v[ind[0],ind[1]] = v[a]
		}
}	
void		Face::term(Equation* equ, int ind[2], int v, int sv, real To) {
	// get the value and coefficienct for the cell adjacent to ind in the direction V

	d = d_[ind[0],ind[1],v]

		// interior cells and boundary cells with Face neighbors
		indnbr = list(ind)
		indnbr[v] += sv

		y = equ.v[indnbr[0],indnbr[1]]

		//print ind,indnbr
		//print "T",T,"To",To

		d_nbr = d_[indnbr[0],indnbr[1],v]

		a = 2.0 / (d + d_nbr)

		return a,y
}
void		Face::step_pre_cell(Equation* equ, int ind[2], int V) {
	// set the v-array value for the boundary value at ind+V

	v,sv = v2is(V)

		conn = loc_to_conn(V)
		if conn:
			if equ.flag["only_parallel_faces"] == True:
				if conn.twin.face.Z == Z_:
					recv_array(equ, conn)
				else:
					step_pre_cell_open_bou(equ, ind, V)
			else:
				recv_array(equ, conn)
		else:
			step_pre_cell_open_bou(equ, ind, V)
}
void		Face::step_pre_cell_open_bou(Equation* equ, int ind[2], int V) {
	//logging.debug('step_pre_cell_open_bou')

	v,sv = v2is(V)
		indn = list(ind)
		indn[v] += sv

		p = 0 if v==1 else 1

		logging.debug("V    {0}".format(V))
		logging.debug("ind  {0}".format(ind))
		logging.debug("indn {0}".format(indn))

		// get stored boundary value
		v_bou = v_bou_[equ.name][v][(sv+1)/2]

		if isinstance(v_bou,float) {
		} else {
			v_bou = v_bou[ind[p]];
		}

	if(v_bou == 0.0) { // insulated
		equ.v[tuple(indn)] = equ.v[tuple(ind)];
	} else { //// constant temperature
		equ.v[tuple(indn)] = 2.0 * v_bou - equ.v[tuple(ind)];
	}

}
void		Face::step_pre(Equation* equ) {
	// for boundaries, load boundary temperature cells with proper value
	// west/east
	for j in range(n_[1]) {
		step_pre_cell(equ, [          0, j], -1)
			step_pre_cell(equ, [n_[0]-1, j],  1)
	}
	// north/south
	for i in range(n_[0]) {
		step_pre_cell(equ, [i,           0], -2)
			step_pre_cell(equ, [i, n_[1]-1],  2)
	}
}
real		Face::step(std::string equ_name) {
	equ = equs_[equ_name]
		// solve diffusion equation for equ
		R = 0.0

		ver1 = False
		ver2 = False
		// solve equation
		step_pre(equ)

		g = patch_.group
		S = g.S[equ.name]

		////print "face S",S
		/*
		   void debug_s() {
		   print "face s",s
		   print "s",equ.s[i,j]
		   print "A",A
		   print "k",equ.equ_prob.k
		   }*/
		for i in range(n_[0]) {
			for j in range(n_[1]) {
				A = d_[i,j,0] * d_[i,j,1]

					yo = equ.v[i,j]

					aW, yW = term(equ, [i,j],0,-1, yo)
					aE, yE = term(equ, [i,j],0, 1, yo)
					aS, yS = term(equ, [i,j],1,-1, yo)
					aN, yN = term(equ, [i,j],1, 1, yo)

					//ver = True
					//print "source =",s(To)

					// source term
					s = equ.s[i,j] * S * A / equ.equ_prob.k

					//if s < 0:
					//debug_s()

					num = aW*yW + aE*yE + aS*yS + aN*yN + s
					ys = num / (aW + aE + aS + aN)

					dy = equ.equ_prob.alpha * (ys - yo)

					/*def debug() {
					  print "aW aE aS aN"
					  print aW, aE, aS, aN
					  print "yW yE yS yN yo ys dy"
					  print yW, yE, yS, yN, yo, ys, dy
					  }*/
					////debug()

					if(aW < 0 or aE < 0 or aS < 0 or aN < 0) {
						debug();
						throw 0;
					}

				if (ver1) debug();
				if (math.isnan(yo)) throw 0;

				if math.isnan(ys) or math.isinf(ys) {
					debug();
					throw 0;
				}
				if math.isnan(dy) throw 0;
				equ.v[i,j] += dy

					if (dy == 0.0) {
					} else {
						R = max(math.fabs(dy/yo), R)
					}

				if math.isnan(R) {
					//print 'dy',dy,'yo',yo
					//raise ValueError('nan')
				}
			}
		}
	return R;
}
void		Face::send(std::string equ_name) {
	// send/recv neighbor values
	equ = equs_[equ_name]

		for con in conns_.flatten() {
			if con:
				send_array(equ, con)
		}
}
void		Face::recv(std::string equ_name) {
	equ = equs_[equ_name]

		for con in conns_.flatten() {
			if con:
				recv_array(equ, con)
		}
}









