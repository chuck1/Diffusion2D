#include <fstream>

#include <Diff2D/config.hpp>
#include <Diff2D/face.hpp>
#include <Diff2D/patch.hpp>
#include <Diff2D/patch_group.hpp>
#include <Diff2D/prob.hpp>

template<typename T> void write_vec(std::ofstream& ofs, std::vector<T> x) {
	int a = 0;

	ofs << std::scientific;

	for(auto b : x) {
		ofs << b << " ";
		a = a + 1;
		if(a == 10) {
			ofs << "\n";
			a = 0;
		}
	}
}

Patch_Group::Patch_Group(
		Prob_s prob,
		std::string name,
		std::map<std::string, real> v_0,
		std::map<std::string, real> S):
	prob_(prob)
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

	real v_0 = v_0_[equ_name];

	if(v_0 == 0.0) return 0.0;

	// current area-weighted-average value
	real v = 0;
	real A = 0;
	
	auto equ_prob = prob_.lock()->equs_[equ_name];
	
	for(auto p : patches_) {
		for(auto f : *p->faces_) {//->flatten()) 
			auto equ = f->equs_[equ_name];
		
			real a = f->area();
			v += equ->mean() * a;
			A += a;
		}
	}
	//print "name       ",name_
	//print "num patches",len(patches_)
	
	real v_m = v/A;
	
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

	std::vector<real> x;// = np.zeros(0);
	std::vector<real> y;// = np.zeros(0);
	std::vector<real> z;// = np.zeros(0);
	std::vector<real> w;// = np.zeros(0);
	
	for(auto f : faces()) {
		auto grid = f->grid(equ_name);
		//X,Y,Z,W = f.grid(equ_name);
		
		auto Xr = std::get<0>(grid)->ravel();
		auto Yr = std::get<0>(grid)->ravel();
		auto Zr = std::get<0>(grid)->ravel();
		auto Wr = std::get<0>(grid)->ravel();
		
		x.insert(x.begin(), Xr.begin(), Xr.end());
		y.insert(y.begin(), Yr.begin(), Yr.end());
		z.insert(z.begin(), Zr.begin(), Zr.end());
		w.insert(w.begin(), Wr.begin(), Wr.end());
	}

	std::string name = "prof_" + name_ + "_" + equ_name;

	int n = x.size();
	
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





