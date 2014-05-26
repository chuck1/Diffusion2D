
#include "config.hpp"
#include "patch.hpp"
#include "prob.hpp"


Patch_Group::Patch_Group(
		Prob_s prob,
		std::string name,
		std::map<std::string, real> v_0,
		std::map<std::string, real> S)
{

	prob_ = prob;
	name_ = name;
	v_0_ = v_0;

	S_ = S;

	//print "patch_group"
	//print v_0_
}
Patch_s			Patch_Group::create_patch(
		std::string name,
		int normal,
		std::vector< std::vector<int> > indices,
		std::map< std::string, array<array<real,1>, 2 > > v_bou) {

	//print 'T_0',T_0

	auto prob = prob_.lock();

	auto p = std::make_shared<Patch>(name, normal, indices, prob->x_, prob->nx_, v_bou);

	patches_.push_back(p);
	return p;
}
void		Patch_Group::reset_s(std::string equ_name) {
	/*def debug() {
	  print "reset_s"
	  print "equ_name",equ_name
	  print "v_m     ",v_m
	  print "v_0     ",v_0
	  print "dS      ",dS
	  print "S       ",S_[equ_name]*/

	real v_0 = v_0_[equ_name];

	if(v_0 == 0.0) return;

	// current area-weighted-average value
	real v = 0;
	real A = 0;

	for(auto p : patches_) {
		for(auto f : *p->faces_) {//->flatten()) 
			auto equ = f->equs[equ_name];

			a = f.area();
			v += equ.mean() * a;
			A += a;
		}
	}
	//print "name       ",name_
	//print "num patches",len(patches_)

	real v_m = v/A;

	real dv = v_0 - v_m;

	real dS = equ.equ_prob.k * dv / 10.0;

	S_[equ_name] += dS;
	//Tmean_.append(vm)
	//debug();
	return math.fabs(dv / v_0);
}	
std::vector< Face_s >		Patch_Group::faces() {
	std::vector< Face_s > ret;

	for(p : patches_) {
		for(f : p->faces_->flatten()) {
			ret.push_back(f);
		}
	}
}
void				Patch_Group::write(std::string equ_name, std::ofstream file) {

	x = np.zeros(0);
	y = np.zeros(0);
	z = np.zeros(0);
	w = np.zeros(0);

	for(auto f : faces()) {
		X,Y,Z,W = f.grid(equ_name);

		x = np.append(x, X.ravel());
		y = np.append(y, Y.ravel());
		z = np.append(z, Z.ravel());
		w = np.append(w, W.ravel());
	}	
	name = "prof_" + name_ + "_" + equ_name;

	n = np.size(x,0);

	file.write("(({0} point {1})\n".format(name,n));

	file.write("(x\n");
	file.write("".join(np_join(x)) + ")\n");

	file.write("(y\n");
	file.write("".join(np_join(y)) + ")\n");

	file.write("(z\n");
	file.write("".join(np_join(z)) + ")\n");

	file.write("(w\n");
	file.write("".join(np_join(w)) + ")\n");
	//file.write(" ".join("{0:e}".format(a) for a in w) + ")")

	file.write(")\n");
}





