

#include "config.hpp"


Problem::Problem(
			std::string name,
			std::vector< array<real,1> > x,
			std::vector< array<int,1> > nx,
			int it_max_1,
			int it_max_2)
	{
		name_ = name;

		x_ = x;
		nx_ = nx;

		equs_ = {};

		it_max_1_ = it_max_1;
		it_max_2_ = it_max_2;

		//signal.signal(signal.SIGINT, self)
	}
	Equation_s	Problem::create_equation(std::string name, real k, real alpha, real alpha_source) {
		auto e = std::make_shared<Equation>(name, k, alpha, alpha_source);
		equs_[name] = e;
		return e;
	}
	Patch_Group_s	Problem::create_patch_group(std::string name, std::map<std::string, real> v_0, std::map<std::string, real> S) {
		auto g = std::make_shared<Patch_Group>(name, v_0, S);

		patch_groups_.push_back(g);

		return g;
	}
	/*def __call__(signal, frame) {
	  print "saving"
	//save()
	sys.exit(0)
	}*/	
	real		Problem::temp_max(std::string equ_name) {
		real T = -1E37;
		for(auto f : faces()) {
			auto e = f->equs[equ_name];
			T = max(e.max(), T);
		}
		return T;
	}
	real		Problem::temp_min(std::string equ_name) {
		T = 1E37;
		for f in faces() {
			e = f.equs[equ_name];
			T = min(e.min(), T);
		}	
		return T;
	}
	real		Problem::grad_max(std::string equ_name) {
		real T = -1E37;
		for f in faces() {
			e = f.equs[equ_name];
			T = max(e.grad_max(), T);
		}
		return T;
	}
	real		Problem::grad_min(std::string equ_name) {
		T = float("inf")
			for f in faces() {
				e = f.equs[equ_name];
				T = min(e.grad_min(), T);
			}
		return T
	}
	// value manipulation
	void		Problem::value_add(std::string equ_name, array<real,2> v) {
		for(f : faces()) {
			equ = f.equs[equ_name];
			equ.v = equ.v + v;
		}
	}
	void		Problem::value_normalize(std::string equ_name) {
		for(g in patch_groups_) {
			// max value in patch group
			real v_max = -1E37;
			for(p : g.patches) {
				p_v_max = p.max(equ_name;)
					//print "patch max value",p_v_max
					v_max = max(v_max, p_v_max);

				//print "max value",v_max

				// normalize
				for(p : g.patches_) {
					for f in p.faces.flatten() {
						equ = f.equs[equ_name]
							equ.v = equ.v / v_max
					}
				}
			}
		}
	}
	void		Problem::copy_value_to_source(std::string equ_name_from, std::string equ_name_to) {

		for f in faces() {
			e1 = f.equs[equ_name_from];
			e2 = f.equs[equ_name_to];
		}
		s1 = tuple(a-2 for a in np.shape(e1.v));
		s2 = np.shape(e2.s);

		if(s1 == s2) {
			e2.s = e1.v[:-2,:-2];
		} else {
			print s1, s2;
			throw 0;//raise ValueError('size mismatch')
		}
	}
	std::vector<Face_s>		Problem::faces() {
		std::vector<Face_s> ret;
		for g in patch_groups_ {
			for p in g.patches {
				for f in p.faces.flatten() {
					ret.push_back(f);
				}
			}
		}
		return ret;
	}
	// solving
	int		Problem::solve(std::string name, real cond, bool ver, real R_outer) {
		return solve_serial(name, cond, ver, R_outer);
	}
	int		Problem::solve_serial(std::string name, real cond, bool ver, real R_outer) {
		std::vector<real> R;

		for it in range(it_max_1_) {
			R = np.append(R, 0.0);

			for face in faces() {
				R[-1] = max(face.step(name), R[-1]);
				face.send(name);
			}
			for face in faces() {
				face.recv(name);
			}

			if(ver) {
				std::cout.flags(std::ios_base::scientific);
				std::cout << setw(4) << it << setw(8) << R_outer << setw(8) << R[-1] << std::endl;
			}
			if math.isnan(R[-1]) {
				throw 0;//raise ValueError('nan')
			}
			if R[-1] < cond: break;
		}
		return it;
	}
	int		Problem::solve2(std::string equ_name, real cond1_final, real cond2, bool ver) {
		//cond1 = 1

		//it_cond = 2

		R = 1.0;
		for it_2 in range(it_max_2_) {

			cond1 = R / 1000.0; // target residual for inner loop is proportional to current residual for outer loop

			it_1 = solve(equ_name, cond1, ver, R);

			R = 0.0;

			for(g : patch_groups_) {
				Rn = g.reset_s(equ_name);

				R = max(Rn, R);

				print "{0:3d} {1:8e}".format(it_2,R);

				if math.isnan(R) {
					throw 0;//raise ValueError('nan')
				}
				if(R < cond2) break;
			}
			return it_2;
		}
	}
	void		Problem::save() {
		f = open("case_" + name_, 'w')
			pickle.dump(f)
	}
	void	write(std::string equ_name) {

		directory = name_ + "/";

			if not os.path.exists(directory) {
				os.makedirs(directory)
			}
		name = "prof_" + equ_name + ".txt";

		with open(directory + name,'w') as f:
			for(g in patch_groups_){
				print g
					g.write(equ_name,f)
			}

	}




