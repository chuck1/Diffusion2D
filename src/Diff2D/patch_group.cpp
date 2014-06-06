#include <fstream>

#include <Diff2D/config.hpp>
#include <Diff2D/face.hpp>
#include <Diff2D/patch.hpp>
#include <Diff2D/patch_group.hpp>
#include <Diff2D/prob.hpp>
#include <Diff2D/util.hpp>

real	linear_interp(
		real x1,
		real x2,
		real y1,
		real y2,
		real y3
		) {
	real m = (y2-y1)/(x2-x1);
	real b = y2 - m * x2;
	real x3 = (y3 - b)/m;
	return x3;
}

Patch_Group::Patch_Group(
		Prob_s prob,
		std::string name,
		std::map<std::string, real> v_0,
		std::map<std::string, real> S,
		point v_0_point):
	prob_(prob),
	v_0_point_(v_0_point),
	flag_(0)
{
	assert(prob);

	name_ = name;
	v_0_ = v_0;

	v_0_history_["T"] = make_zeros<real,1>({prob->it_max_outer_});
	v_0_history_["s"] = make_zeros<real,1>({prob->it_max_outer_});


	S_["T"] = make_ones<real,1>({prob->it_max_outer_})->multiply_self(S["T"]);
	S_["s"] = make_ones<real,1>({prob->it_max_outer_})->multiply_self(S["s"]);



	//print "patch_group"
	//print v_0_
}
Patch_s			Patch_Group::create_patch(
		std::string name,
		int normal,
		std::vector<size_t> indicesx,
		std::vector<size_t> indicesy,
		std::vector<size_t> indicesz,
		patch_v_bou_type v_bou) {

	//print 'T_0',T_0

	multivec<2,size_t> indices({indicesx,indicesy,indicesz});


	auto prob = prob_.lock();
	assert(prob);

	auto me = shared_from_this();
	assert(me);

	auto p = std::make_shared<Patch>(me, name, normal, indices, prob->x_, prob->nx_, v_bou);

	p->create_faces();

	std::cout
		<< std::setw(16) << ("'" + name_ + "'") << "add"
		<< std::setw(16) << ("'" + name + "'") << std::endl;




	patches_.push_back(p);
	return p;
}
void			Patch_Group::get_value_of_interest(std::string const & ename, real& target, real& value, real& error) {
	/*	auto awa = [&]() {
	// current area-weighted-average value
	real v = 0;
	real A = 0;

	for(auto p : patches_) {
	for(auto f : *p->faces_) {
	auto equ = f->equs_[equ_name];
	real a = f->area();
	v += equ->mean() * a;
	A += a;
	}
	}

	//print "name       ",self.name
	//print "num patches",len(self.patches)

	real v_m = v/A;
	return v_m;
	};*/

	auto point = [&] () {
		real v_m;
		for(auto p : patches_) {
			for(auto f : *p->faces_) {
				auto equ = f->equs_[ename];

				try {
					v_m = equ->at_point(v_0_point_);
					return v_m;
				} catch(point_not_found&) {
					continue;
				} catch(...) {
					BOOST_LOG_CHANNEL_SEV(gal::log::lg, "Diff2D", critical) << "unknwon error occured" << GAL_LOG_ENDLINE;
					abort();
				}
			}
		}
		std::cout << "point not found anywhere" << std::endl;
		std::cout << "group " << ("'"+name_+"'") << std::endl;
		std::cout << "num patches = " << patches_.size() << std::endl;
		throw 0;//raise ValueError('point is not in patch_group')
	};

	target = v_0_[ename];

	value = point();

	error = target - value;
}
real			Patch_Group::get_value_of_interest_residual(std::string const & ename) {
	real target, value, error;
	get_value_of_interest(ename, target, value, error);
	return std::abs(error/target);
}
real			Patch_Group::reset_s(size_t it, std::string ename) {
	real target, value, error;

	auto prob = prob_.lock();

	array<real,1>& v_0_hist = v_0_history_[ename];
	array<real,1>& S = S_[ename];
	assert(v_0_hist);
	assert(S);




	// current area-weighted-average value

	auto equ_prob = prob->equs_[ename];

	get_value_of_interest(ename, target, value, error);

	if(target == 0.0) return 0.0;

	// this value is affected by S[it-1]
	v_0_hist->get(it-1) = value;

	// response
	if(1) {
		//if((it < 5) && (!(flag_ & Patch_Group::flags::SOURCE_SOLVED))) 
		if(it < 3) {
			//real dS = equ_prob->k_ * dv / 20.0;

			//S->get(it) += dS;

			S->get(it) = (real)it * 1000.0;
		} else {
			real nS = linear_interp(
					S->get(it-2),
					S->get(it-1),
					v_0_hist->get(it-2),
					v_0_hist->get(it-1),
					target);
			S->get(it) = nS;
		}
	} else {
		// ramp
		S->get(it) = (real)it * 1.0;
	}

	print_row(24, "S(it-2)", "S(it-1)", "S(it)", "v_0(it-2)", "v_0(it-1)", "v_0(it)");
	print_row(24,
			S->get(it-2),
			S->get(it-1),
			S->get(it),
			v_0_hist->get(it-2),
			v_0_hist->get(it-1),
			value);
	
	return std::abs(error / target);
}
void				Patch_Group::value_add(std::string const & name, real const & a) {
	for(auto f : faces()) {
		auto e = f->equs_[name];
		e->v_->add_self(a);
	}
}
void				Patch_Group::value_divide(std::string const & name, real const & a) {
	for(auto f : faces()) {
		auto e = f->equs_[name];
		e->v_->divide_self(a);
	}
}
void				Patch_Group::value_clamp(std::string const & name, real const & a, real const & b) {
	real vmin = min(name);
	real vmax = max(name);
	real vrng = vmax - vmin;

	DEBUG_LINE print_row(16,"min","max","rng");
	DEBUG_LINE print_row(16,vmin,vmax,vrng);

	value_add(name, -vmin);
	value_divide(name, vrng);

	vmin = min(name);
	vmax = max(name);
	vrng = vmax - vmin;

	DEBUG_LINE print_row(16,"min","max","rng");
	DEBUG_LINE print_row(16,vmin,vmax,vrng);
}
real				Patch_Group::min(std::string const & name) const {
	real v = 1E37;
	for(auto p : patches_) {
		v = std::min(v,p->min(name));
	}
	return v;
}
real				Patch_Group::max(std::string const & name) const {
	real v = -1E37;
	for(auto p : patches_) {
		v = std::max(v,p->max(name));
	}
	return v;
}
std::vector< Face_s >		Patch_Group::faces() {
	std::vector< Face_s > ret;

	for(auto p : patches_) {
		for(auto f : *(p->faces_)) {
			ret.push_back(f);
		}
	}

	return ret;
}
void				Patch_Group::write(std::string equ_name, std::ofstream& ofs) {

	if(!ofs.is_open()) {
		BOOST_LOG_CHANNEL_SEV(gal::log::lg, "Diff2D", warning) << "file stream not open" << std::endl;
		return;
	}

	assert(0);

	std::vector<real> x;// = np.zeros(0);
	std::vector<real> y;// = np.zeros(0);
	std::vector<real> z;// = np.zeros(0);
	std::vector<real> w;// = np.zeros(0);

	for(auto f : faces()) {
		auto grid = f->grid(equ_name);
		//X,Y,Z,W = f.grid(equ_name);

		auto Xr = grid.X[0]->ravel();
		auto Yr = grid.X[1]->ravel();
		auto Zr = grid.X[2]->ravel();
		auto Wr = grid.W->ravel();

		x.insert(x.end(), Xr.begin(), Xr.end());
		y.insert(y.end(), Yr.begin(), Yr.end());
		z.insert(z.end(), Zr.begin(), Zr.end());
		w.insert(w.end(), Wr.begin(), Wr.end());
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
void		Patch_Group::write_binary(std::string ename) {
	for(auto p : patches_) {
		p->write_binary(ename);
	}

	// iteration data
	std::cout << "'"<<name_<<"' write idata" << std::endl;
	//
	auto dir = prob_.lock()->mkdir(ename);

	auto filename = dir + "/idata_" + name_ + ".txt";

	math::binary_oarchive ar(filename);
	ar.open();

	S_[ename]->serialize(ar,0);
	v_0_history_[ename]->serialize(ar,0);
}






