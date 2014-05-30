#include <math-array/array.hpp>

#include <Diff2D/equation.hpp>
#include <Diff2D/face.hpp>



Equation_Prob::Equation_Prob(Prob_s prob, std::string name, real k, real alpha, real alpha_source) {
	prob_ = prob;
	name_ = name;
	k_ = k;
	alpha_ = alpha;
	alpha_source_ = alpha_source;
}


Equation::Equation(std::string name, Face_s face, Equation_Prob_s equ_prob) {
	name_ = name;
	face_ = face;

	equ_prob_ = equ_prob;

	//v_0_ = v_0
	
	auto n_extended = face_->n_->add(make_array_1<size_t,1>({2, 2}));
	
	v_ = make_ones_arr<real,2>(n_extended);
	
	auto group = face_->patch_->group_.lock();

	v_->multiply_self(group->v_0_[name_]);
	
	s_ = make_ones_arr<real,2>(face->n_);

	flag_ = 0;
}
array<real,3>		Equation::grad() {
	//return np.gradient(v_[:-2,:-2], face_->d[0,0,0], face_.d[0,0,1])
	return v_->sub({0,0},{-2,-2})->gradient(face_->d_);
}
real			Equation::grad_mag() {
	return sqrt(grad()->square()->sum());
}
real			Equation::min() {
	return v_->sub({0,0},{-2,-2})->min();
}
real			Equation::max() {
	return v_->sub({0,0},{-2,-2})->max();
}
real			Equation::grad_min() {
	return v_->sub({0,0},{-2,-2})->gradient(face_->d_)->min();
}
real			Equation::grad_max() {
	return v_->sub({0,0},{-2,-2})->gradient(face_->d_)->max();
}
real			Equation::mean() {
	return v_->sub({0,0},{-2,-2})->max();
}

void			Equation::point(real pt[2]) {

	// global coor cooresponding to local coors
	Xg = self.face.loc_to_glo(1);
	Yg = self.face.loc_to_glo(2);
	Zg = self.face.loc_to_glo(3);

	xg,sxg = v2is(Xg);
	yg,syg = v2is(Yg);
	zg,szg = v2is(Zg);

	// is point in face extents

	auto debug = [&] () {
		//print 'equ point'
		//print xg,yg,zg
		//print sxg,syg,szg
		//print pt
		//print self.face.ext
	}

	if(pt[xg] < face_->ext[0,0]) {
		LOG_SEV_CHANNEL(d2d::log::sl::debug, LOG_CORE) << "x " << pt[xg] << " < " << face_->ext[0,0] << std::endl;
		throw point_not_found();
	}

	if(pt[xg] > face_->ext[0,1]) {
		LOG_SEV_CHANNEL(d2d::log::sl::debug, LOG_CORE) << "x " << pt[xg] << " > " << face_->ext[0,1] << std::endl;
			throw point_not_found();
	}

	if(pt[yg] < face_->ext[1,0]) {
		LOG_SEV_CHANNEL(d2d::log::sl::debug, LOG_CORE) << "y " << pt[yg] << " > " << face_->ext[1,0] << std::endl;
			throw point_not_found();
	}

	if(pt[yg] > face_->ext[1,1]) {
		LOG_SEV_CHANNEL(d2d::log::sl::debug, LOG_CORE) << "y " << pt[yg] << " > " << face_->ext[1,1] << std::endl;
			throw point_not_found();
	}

	if(pt[zg] != face_->pos_z) {
		LOG_SEV_CHANNEL(d2d::log::sl::debug, LOG_CORE) << "z"
			throw point_not_found();
	}

	i = (pt[xg] - face_->ext[0,0]) / self.face.d[0,0,0];
	j = (pt[yg] - face_->ext[1,0]) / self.face.d[0,0,1];

	i = round(i);
	j = round(j);

	//print 'equ point i,j',i,j

	return self.v[i,j];
}





