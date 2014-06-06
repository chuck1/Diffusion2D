#define _POSIX_SOURCE
#include <sys/stat.h>

#include <iostream>
#include <fstream>
#include <iomanip>

#include <Diff2D/config.hpp>
#include <Diff2D/face.hpp>
#include <Diff2D/prob.hpp>
#include <Diff2D/util.hpp>

#define REPLACE_MIN(x,y) if(y < x) x = y;
#define REPLACE_MAX(x,y) if(y > x) x = y;

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
real		Prob::max(std::string const & equ_name) const {
	real T = -1E37;
	for(auto f : faces()) {
		auto e = f->equs_[equ_name];
		T = std::max(e->max(), T);
	}
	return T;
}
real		Prob::min(std::string const & equ_name) const {
	real v = 1E37;
	for(auto f : faces()) {
		auto e = f->equs_[equ_name];
		v = std::min(e->min(), v);
	}	
	return v;
}
real		Prob::grad_max(std::string const & equ_name) const {
	real v = -1E37;
	for(auto f : faces()) {
		auto e = f->equs_[equ_name];
		v = std::max(e->grad_max(), v);
	}
	return v;
}
real		Prob::grad_min(std::string const & equ_name) const {
	real v = 1E37;
	for(auto f : faces()) {
		auto e = f->equs_[equ_name];
		v = std::min(e->grad_min(), v);
	}
	return v;
}
// value manipulation
void		Prob::value_add(std::string const & equ_name, array<real,2> const & v) {
	for(auto f : faces()) {
		auto equ = f->equs_[equ_name];
		equ->v_->add_self(v);
	}
}
void		Prob::value_add(std::string const & equ_name, real const & v) {
	for(auto f : faces()) {
		auto equ = f->equs_[equ_name];
		equ->v_->add_self(v);
	}
}
void		Prob::value_clamp_per_group(std::string const & name, real a, real const & b) {
	for(auto g : patch_groups_) {
		g->value_clamp(name,a,b);
	}
}
void		Prob::value_normalize(std::string const & equ_name) {
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
std::vector<Face_s>		Prob::faces() const {
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

	real cond_grad_inner = -1E-9;

	/** @todo move this to class variable to avoid excess alloc!*/
	auto R = make_zeros<real,1>({it_max_inner_});

	real nR = 0;

	size_t it = 0;
	for(; it < it_max_inner_; ++it) {
		nR = 0;
		for(auto face : faces()) {
			nR = std::max(face->step(name, it_outer), nR);
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
		real grad2 = R->fda_2_back(it, 1.0);
		
		BOOST_LOG_CHANNEL_SEV(gal::log::lg, "Diff2D", info) << std::scientific
			<< std::setw(4) << name
			<< std::setw(6) << it_outer
			<< std::setw(16) << R_outer
			<< std::setw(6) << it
			<< std::setw(16) << nR
			<< std::setw(16) << grad
			<< std::setw(16) << grad2
			<< std::endl;
		
		
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
int		Prob::solve_outer_group(
		std::string const & ename,
		real cond_outer,
		real cond_inner,
		std::shared_ptr<Patch_Group> group) {

	real cond_upper_outer = 1E4;

	real R_outer = 1.0;

	// iterate
	size_t it_outer = 0;
	solve(ename, cond_inner, it_outer, R_outer);
	it_outer++;
	for(; it_outer < it_max_outer_; ++it_outer) {
		// reset source =======================
		R_outer = group->reset_s(it_outer, ename);
		
		// solve inner ========================
		solve(ename, cond_inner, it_outer, R_outer);

		BOOST_LOG_CHANNEL_SEV(gal::log::lg, "Diff2D", info)
			<< std::setw(6) << it_outer
			<< std::setw(16) << R_outer
			<< std::endl;

		if(std::isnan(R_outer)) {
			throw 0;//raise ValueError('nan')
		}

		if(R_outer < cond_outer) {
			BOOST_LOG_CHANNEL_SEV(gal::log::lg, "Diff2D", info) << "break outer because residual < " << std::scientific << cond_outer << std::endl;
			
			// callback good break
			// fill S with final value
			real S = group->S_[ename]->get(it_outer-1);
			group->S_[ename]->ones();
			group->S_[ename]->multiply_self(S);
		
			// fill v_0_history with final value
			real v_0 = group->v_0_history_[ename]->get(it_outer-1);
			group->v_0_history_[ename]->ones();
			group->v_0_history_[ename]->multiply_self(v_0);

			/// @todo put in Patch_Group equation data class
			group->flag_ |= Patch_Group::flags::SOURCE_SOLVED;

			break;
		}
		if(R_outer > cond_upper_outer) {
			BOOST_LOG_CHANNEL_SEV(gal::log::lg, "Diff2D", info) << "kill because outer residual > " << std::scientific << cond_upper_outer << std::endl;
			throw 0;
		}
	}
	return it_outer;
}
void		Prob::solve2(std::string ename, real cond_outer, real cond_inner) {
	
	//real R_outer = 1.0;

	// run at least twice
	
	size_t it3 = 0;
	size_t it3_stop = 5;
	while(1) {
		for(auto g : patch_groups_) {
			solve_outer_group(ename, cond_outer, cond_inner, g);
		}
		
		real R3_max = -1E35;
		real R3 = 0;
		for(auto g : patch_groups_) {
			R3 = g->get_value_of_interest_residual(ename);
			R3_max = std::max(R3_max, R3);
			std::cout << "group '" << g->name_ << "' residual = " << R3 << std::endl;
		}
		
		if(R3 < cond_outer) {
			BOOST_LOG_CHANNEL_SEV(gal::log::lg, "Diff2D", info) << "break loop 3 because residual < " << std::scientific << cond_outer << std::endl;
			break;
		}
		if(it3 == (it3_stop-1)) {
			BOOST_LOG_CHANNEL_SEV(gal::log::lg, "Diff2D", info) << "break loop 3 because it3 == " << std::scientific << it3_stop << std::endl;
			break;
		}
		it3++;
	}
}
void		Prob::save() {
	std::ofstream ofs;
	ofs.open("case_" + name_, std::ofstream::out);
	//pickle.dump(f)
}
void		Prob::connection_info() const {
	for(auto g : patch_groups_) {
		for(auto p : g->patches_) {
			p->connection_info();
		}
	}
}
void		Prob::value_stats(std::string const & name) const {
	size_t col = 16;
	std::cout
		<< std::setw(col*4) << std::setfill(' ') << " "
		<< std::setw(col) << "min"
		<< std::setw(col) << "max"
		<< std::endl;

	for(auto g : patch_groups_) {
		print_row(col, "group", ("'" + g->name_ + "'"), " ", " ", g->min(name), g->max(name));

		for(auto p : g->patches_) {
			std::cout
				<< std::setw(col*2) << " "
				<< std::setw(col) << "patch"
				<< std::setw(col) << ("'" + p->name_ + "'")
				<< std::setw(col) << p->min(name)
				<< std::setw(col) << p->max(name)<< std::endl;
		}
	}

	// value of interest
	print_row(16, "group", "target", "value", "error");
	for(auto g : patch_groups_) {
		real target, value, error;
		g->get_value_of_interest(name, target, value, error);
		
		print_row(16, ("'" + g->name_ + "'"), target, value, error);
	}
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
void		Prob::write_binary(std::string ename) {

	for(auto g : patch_groups_) {
		//print g
		g->write_binary(ename);
	}


}
std::string		Prob::mkdir(std::string const & ename) const {
	std::string dir1 = "output";
	std::string dir2 = dir1 + "/" + name_;
	std::string dir3 = dir2 + "/" + ename;

	// create folders

	auto lmkdir = [] (std::string const &  dir) {
		struct stat sb;
		if(stat(dir.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode)) {
		} else {
			if(::mkdir(dir.c_str(), S_IRWXU|S_IRGRP|S_IXGRP) != 0) {
				perror("mkdir error:");
				return;
			}
		}
	};

	lmkdir(dir1);
	lmkdir(dir2);
	lmkdir(dir3);

	return dir3;
}




