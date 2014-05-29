#include <iostream>
#include <fstream>
#include <iomanip>

#include <Diff2D/config.hpp>
#include <Diff2D/face.hpp>
#include <Diff2D/prob.hpp>

Prob::Prob(
		std::string name,
		std::vector< array<real,1> > x,
		std::vector< array<size_t,1> > nx,
		int it_max_inner,
		int it_max_outer):
	it_max_inner_(it_max_inner),
	it_max_outer_(it_max_outer)
{
	name_ = name;

	x_ = x;
	nx_ = nx;

	equs_ = {};

	//signal.signal(signal.SIGINT, self)
}
Equation_Prob_s		Prob::create_equation(std::string name, real k, real alpha, real alpha_source) {
	auto e = std::make_shared<Equation_Prob>(shared_from_this(), name, k, alpha, alpha_source);
	equs_[name] = e;
	return e;
}
Patch_Group_s		Prob::create_patch_group(std::string name, std::map<std::string, real> v_0, std::map<std::string, real> S) {
	auto g = std::make_shared<Patch_Group>(shared_from_this(), name, v_0, S);

	patch_groups_.push_back(g);

	return g;
}
/*def __call__(signal, frame) {
  print "saving"
//save()
sys.exit(0)
}*/	
real		Prob::temp_max(std::string equ_name) {
	real T = -1E37;
	for(auto f : faces()) {
		auto e = f->equs_[equ_name];
		T = std::max(e->max(), T);
	}
	return T;
}
real		Prob::temp_min(std::string equ_name) {
	real v = 1E37;
	for(auto f : faces()) {
		auto e = f->equs_[equ_name];
		v = std::min(e->min(), v);
	}	
	return v;
}
real		Prob::grad_max(std::string equ_name) {
	real v = -1E37;
	for(auto f : faces()) {
		auto e = f->equs_[equ_name];
		v = std::max(e->grad_max(), v);
	}
	return v;
}
real		Prob::grad_min(std::string equ_name) {
	real v = 1E37;
	for(auto f : faces()) {
		auto e = f->equs_[equ_name];
		v = std::min(e->grad_min(), v);
	}
	return v;
}
// value manipulation
void		Prob::value_add(std::string equ_name, array<real,2> v) {
	for(auto f : faces()) {
		auto equ = f->equs_[equ_name];
		equ->v_->add_self(v);
	}
}
void		Prob::value_add(std::string equ_name, real v) {
	for(auto f : faces()) {
		auto equ = f->equs_[equ_name];
		equ->v_->add_self(v);
	}
}
void		Prob::value_normalize(std::string equ_name) {
	for(auto g : patch_groups_) {
		// max value in patch group
		real v_max = -1E37;
		for(auto p : g->patches_) {
			real p_v_max = p->max(equ_name);
			//print "patch max value",p_v_max
			v_max = std::max(v_max, p_v_max);

			//print "max value",v_max

			// normalize
			for(auto p : g->patches_) {
				for(auto f : *(p->faces_)) {
					auto equ = f->equs_[equ_name];
					equ->v_->divide_self(v_max);
				}
			}
		}
	}
}
void		Prob::copy_value_to_source(std::string equ_name_from, std::string equ_name_to) {
	Equation_s e1;
	Equation_s e2;

	for(auto f : faces()) {
		e1 = f->equs_[equ_name_from];
		e2 = f->equs_[equ_name_to];
	}
	
	std::vector<size_t> s1;
	for(auto a : e1->v_->shape()) {
		s1.push_back(a-2);
	}

	auto s2 = e2->s_->shape();
	
	if(s1 == s2) {
		e2->s_ = e1->v_->sub({0,0},{-2,-2});
	} else {
		//print s1, s2;
		throw 0;//raise ValueError('size mismatch')
	}
}
std::vector<Face_s>		Prob::faces() {
	std::vector<Face_s> ret;
	for(auto g : patch_groups_) {
		for(auto p : g->patches_) {
			for(auto f : *(p->faces_)) {
				ret.push_back(f);
			}
		}
	}
	return ret;
}
// solving
int		Prob::solve(std::string name, real cond, bool ver, real R_outer) {
	return solve_serial(name, cond, ver, R_outer);
}
int		Prob::solve_serial(std::string name, real cond, bool ver, real R_outer) {

	auto R = make_uninit<real,1>({it_max_inner_});
	
	real nR;
	
	size_t it = 0;
	for(; it < it_max_inner_; ++it) {
		nR = 0;
		for(auto face : faces()) {
			nR = std::max(face->step(name), nR);
			face->send(name);
		}
		for(auto face : faces()) {
			face->recv(name);
		}
		
		
		
	
		IF(std::isnan(nR)) {
			throw 0;//raise ValueError('nan')
		}
		
		R->get(it) = nR;
		
		LOG_SEV_CHANNEL(d2d::log::sl::info, d2d::log::core) << std::scientific
			<< std::setw(6) << it
			<< std::setw(16) << R_outer
			<< std::setw(16) << nR
			<< std::setw(16) << R->fda_1_back(it, 1.0) << std::endl;


		if(nR < cond || R->fda_1_back(it-1, 1.0)) break;
		
	}
	return it;
}
int		Prob::solve2(std::string equ_name, real cond1_final, real cond2, bool ver) {
	//cond1 = 1
	//it_cond = 2
	
	real R = 1.0;
	size_t it_outer = 0;
	for(; it_outer < it_max_outer_; ++it_outer) {

		real cond_inner = R / 1000.0; // target residual for inner loop is proportional to current residual for outer loop

		/*int it_1 =*/ solve(equ_name, cond_inner, ver, R);

		R = 0.0;

		for(auto g : patch_groups_) {
			real Rn = g->reset_s(equ_name);

			R = std::max(Rn, R);

			std::cout <<
				std::setw(6) << it_outer <<
				std::setw(16) << R <<
				std::endl;

			if(std::isnan(R)) {
				throw 0;//raise ValueError('nan')
			}
			if(R < cond2) break;
		}
	}
	return it_outer;
}
void		Prob::save() {
	std::ofstream ofs;
	ofs.open("case_" + name_, std::ofstream::out);
	//pickle.dump(f)
}
void		Prob::write(std::string equ_name) {

	std::string directory = name_ + "/";

	/*if(not os.path.exists(directory)) {
	  os.makedirs(directory)
	  }*/

	std::string name = "prof_" + equ_name + ".txt";

	std::ofstream ofs;
	ofs.open(directory + name, std::ofstream::out);

	for(auto g : patch_groups_) {
		//print g
		g->write(equ_name, ofs);
	}

}






