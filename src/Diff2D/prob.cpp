#include <iostream>
#include <fstream>
#include <iomanip>

#include <Diff2D/config.hpp>
#include <Diff2D/face.hpp>
#include <Diff2D/prob.hpp>
#include <Diff2D/util.hpp>

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
Patch_Group_s		Prob::create_patch_group(std::string name, std::map<std::string, real> v_0, std::map<std::string, real> S, point v_0_point) {
	auto g = std::make_shared<Patch_Group>(shared_from_this(), name, v_0, S, v_0_point);

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
int		Prob::solve(std::string name, real cond, size_t it_outer, real R_outer) {
	return solve_serial(name, cond, it_outer, R_outer);
}
int		Prob::solve_serial(std::string name, real cond_inner, size_t it_outer, real R_outer) {

	real cond_grad_inner = -1E-7;

	/** @todo move this to class variable to avoid excess alloc!*/
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
			BOOST_LOG_CHANNEL_SEV(gal::log::lg, "Diff2D", critical) << "nan" << GAL_LOG_ENDLINE;
			throw 0;
		}
		IF(std::isinf(nR)) {
			BOOST_LOG_CHANNEL_SEV(gal::log::lg, "Diff2D", critical) << "nan" << GAL_LOG_ENDLINE;
			throw 0;
		}
		
		R->get(it) = nR;
		
		real grad = R->fda_1_back(it, 1.0);
		
		BOOST_LOG_CHANNEL_SEV(gal::log::lg, "Diff2D", info) << std::scientific
			<< std::setw(6) << it_outer
			<< std::setw(16) << R_outer
			<< std::setw(6) << it
			<< std::setw(16) << nR
			<< std::setw(16) << grad << std::endl;
		
		
		if((grad <= 0) && (grad > cond_grad_inner)) {
			BOOST_LOG_CHANNEL_SEV(gal::log::lg, "Diff2D", info) << "break inner because residual gradient < " << std::scientific << cond_grad_inner << std::endl;
			break;
		}
		if(nR < cond_inner) {
			BOOST_LOG_CHANNEL_SEV(gal::log::lg, "Diff2D", info) << "break inner because residual < " << std::scientific << cond_inner << std::endl;
			break;
		}
	}
	return it;
}
int		Prob::solve2(std::string equ_name, real cond_outer, real inner_outer_ratio) {
	//cond1 = 1
	//it_cond = 2

	real R_outer = 1.0;

	// target residual for inner loop is proportional to current residual for outer loop
	real cond_inner = R_outer * inner_outer_ratio;
		
	size_t it_outer = 0;
	for(; it_outer < it_max_outer_; ++it_outer) {
		
		cond_inner = std::min(R_outer * inner_outer_ratio, cond_inner); 
		
		/*int it_1 =*/ solve(equ_name, cond_inner, it_outer, R_outer);
		
		R_outer = 0.0;
		
		for(auto g : patch_groups_) {
			real Rn = g->reset_s(equ_name);

			R_outer = std::max(Rn, R_outer);
		}

		BOOST_LOG_CHANNEL_SEV(gal::log::lg, "Diff2D", info)
			<< std::setw(6) << it_outer
			<< std::setw(16) << R_outer
			<< std::endl;

		if(std::isnan(R_outer)) {
			throw 0;//raise ValueError('nan')
		}

		if(R_outer < cond_outer) {
			BOOST_LOG_CHANNEL_SEV(gal::log::lg, "Diff2D", debug) << "break outer because residual < " << std::scientific << cond_outer << std::endl;
			break;
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

	/*if(not os.path.exists(directory)) {
	  os.makedirs(directory)
	  }*/

	std::ofstream ofs;
	ofs.open("prof_" + name_ + "_" + equ_name + ".prof", std::ofstream::trunc);

	if(!ofs.is_open()) {
		BOOST_LOG_CHANNEL_SEV(gal::log::lg, "Diff2D", warning) << "file stream not open" << GAL_LOG_ENDLINE;
		return;
	}

	std::vector<real> x;// = np.zeros(0);
	std::vector<real> y;// = np.zeros(0);
	std::vector<real> z;// = np.zeros(0);
	std::vector<real> w;// = np.zeros(0);


	for(auto g : patch_groups_) {
		//print g
		//g->write(equ_name, ofs);
		
		for(auto f : g->faces()) {
			auto grid = f->grid(equ_name);
			//X,Y,Z,W = f.grid(equ_name);

			auto Xr = grid.X[f->glo_to_loc2(1).i]->ravel();
			auto Yr = grid.X[f->glo_to_loc2(2).i]->ravel();
			auto Zr = grid.X[f->glo_to_loc2(3).i]->ravel();
			auto Wr = grid.W->ravel();

			x.insert(x.end(), Xr.begin(), Xr.end());
			y.insert(y.end(), Yr.begin(), Yr.end());
			z.insert(z.end(), Zr.begin(), Zr.end());
			w.insert(w.end(), Wr.begin(), Wr.end());
		}	
	}



	std::string name = "prof_" + name_ + "_" + equ_name;

	int n = x.size();


	BOOST_LOG_CHANNEL_SEV(gal::log::lg, "Diff2D", info)
		<< "writing " << n << " points" << std::endl;


	ofs << "((" << name << " point " << n << ")\n";

	ofs << "(x\n";
	write_vec(ofs, x);
	ofs << ")\n";

	ofs << "(y\n";
	write_vec(ofs, y);
	ofs << ")\n";

	ofs << "(z\n";
	write_vec(ofs, z);
	ofs << ")\n";

	ofs << "(w\n";
	write_vec(ofs, w);
	ofs << ")\n";

	ofs << ")\n";


}
void		Prob::write_binary(std::string equ_name) {
	
	
	
	for(auto g : patch_groups_) {
		//print g
		g->write_binary(equ_name);
	}

}





