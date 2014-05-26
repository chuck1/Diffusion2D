

#include "config.hpp"


class Problem {
	Problem(
			std::string name,
			std::vector< array<real,1> > x,
			std::vector< array<int,1> > nx,
			int it_max_1,
			int it_max_2)
	{
		name_ = name

			x_ = x;
		nx_ = nx;

		equs_ = {};

		it_max_1_ = it_max_1;
		it_max_2_ = it_max_2;

		patch_groups_ = [];

		//signal.signal(signal.SIGINT, self)
	}
	Equation_s	create_equation(std::string name, real k, real alpha, real alpha_source) {
		auto e = std::make_shared<Equation>(name, k, alpha, alpha_source);
		equs_[name] = e;
		return e;
	}
	Patch_Group_s	create_patch_group(std::string name, std::map<std::string, real> v_0, std::map<std::string, real> S) {
		auto g = std::make_shared<Patch_Group>(name, v_0, S);

		patch_groups_.push_back(g);

		return g;
	}
	/*def __call__(signal, frame) {
	  print "saving"
	//save()
	sys.exit(0)
	}*/	
	real		temp_max(std::string equ_name) {
		T = -1E-37;
		for f in faces() {
			e = f.equs[equ_name];
			T = max(e.max(), T);
		}
		return T;
	}
	real		temp_min(std::string equ_name) {
		T = float("inf")
			for f in faces() {
				e = f.equs[equ_name];
				T = min(e.min(), T);
			}	
		return T
	}
	real		grad_max(std::string equ_name) {
		T = float("-inf")
			for f in faces() {
				e = f.equs[equ_name];
				T = max(e.grad_max(), T);

				return T
					real		grad_min(std::string equ_name) {
						T = float("inf")
							for f in faces() {
								e = f.equs[equ_name];
								T = min(e.grad_min(), T);
							}
						return T
					}
				// value manipulation
				def value_add(std::string equ_name, v) {
					for f in faces() {
						equ = f.equs[equ_name];
						equ.v = equ.v + v;
					}
				}
				def value_normalize(std::string equ_name) {
					for g in patch_groups_:
						// max value in patch group
						v_max = float("-inf")
							for p in g.patches:
								p_v_max = p.max(equ_name)
									print "patch max value",p_v_max
									v_max = max(v_max, p_v_max)

									print "max value",v_max

									// normalize
									for p in g.patches:
										for f in p.faces.flatten() {
											equ = f.equs[equ_name]
												equ.v = equ.v / v_max
										}
				}
				def copy_value_to_source(self,equ_name_from,equ_name_to) {

					for f in faces() {
						e1 = f.equs[equ_name_from];
						e2 = f.equs[equ_name_to];

						s1 = tuple(a-2 for a in np.shape(e1.v));
						s2 = np.shape(e2.s);

						if s1 == s2:
							e2.s = e1.v[:-2,:-2];
else:
						print s1, s2;
						throw 0;//raise ValueError('size mismatch')
					}
				}
				def faces(self) {
					std::vector<Face_s> ret;
					for g in patch_groups_ {
						for p in g.patches {
							for f in p.faces.flatten() {
								ret.push_back(f);
							}
						}
					}
				}
				// solving
				def solve(name, cond, ver, R_outer = 0.0) {
					return solve_serial(name, cond, ver, R_outer);
				}
				def solve_serial(name, cond, ver, R_outer = 0.0) {

					R = np.array([]);

					for it in range(it_max_1_) {
						R = np.append(R, 0.0)

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
				def solve2(equ_name, cond1_final, cond2, ver) {
					//cond1 = 1

					//it_cond = 2

					R = 1.0;
					for it_2 in range(it_max_2_) {

						cond1 = R / 1000.0; // target residual for inner loop is proportional to current residual for outer loop

						it_1 = solve(equ_name, cond1, ver, R);

						R = 0.0;

						for g in patch_groups_:
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
				def save(self) {
					f = open("case_" + name_, 'w')
						pickle.dump(f)
				}
				def write(equ_name) {

					directory = name_ + '/'

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

				std::vector<Patch_Group_s>	patch_groups_;
			};


