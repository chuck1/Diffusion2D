#include <fstream>

#include <Diff2D/config.hpp>
#include <Diff2D/face.hpp>
#include <Diff2D/patch.hpp>
#include <Diff2D/patch_group.hpp>
#include <Diff2D/prob.hpp>
#include <Diff2D/util.hpp>


Patch_Group::Patch_Group(
		Prob_s prob,
		std::string name,
		std::map<std::string, real> v_0,
		std::map<std::string, real> S,
		point v_0_point):
	prob_(prob),
	v_0_point_(v_0_point)
{
	assert(prob);

	name_ = name;
	v_0_ = v_0;
	S_ = S;

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

	patches_.push_back(p);
	return p;
}
real			Patch_Group::reset_s(std::string equ_name) {
	/*def debug() {
	  print "reset_s"
	  print "equ_name",equ_name
	  print "v_m     ",v_m
	  print "v_0     ",v_0
	  print "dS      ",dS
	  print "S       ",S_[equ_name]*/





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
				auto equ = f->equs_[equ_name];

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
		throw 0;//raise ValueError('point is not in patch_group')
	};



	real v_0 = v_0_[equ_name];

	if(v_0 == 0.0) return 0.0;

	// current area-weighted-average value

	auto equ_prob = prob_.lock()->equs_[equ_name];


	real v_m = point();


	real dv = v_0 - v_m;

	real dS = equ_prob->k_ * dv / 10.0;

	S_[equ_name] += dS;
	//Tmean_.append(vm)
	//debug();
	return std::abs(dv / v_0);
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
void		Patch_Group::write_binary(std::string equ_name) {
	for(auto p : patches_) {
		p->write_binary(equ_name);
	}
}






