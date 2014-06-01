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


Equation::Equation(std::string name, Face_s face, Equation_Prob_s equ_prob):
	name_(name),
	face_(face),
	equ_prob_(equ_prob) {

	//v_0_ = v_0

	v_ = make_ones<real,2>({
			face_->n_->get(0) + 2,
			face_->n_->get(1) + 2
			});
	
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

real			Equation::at_point(point pt) {

	// global coor cooresponding to local coors
	auto Xg = face_->loc_to_glo(1);
	auto Yg = face_->loc_to_glo(2);
	auto Zg = face_->loc_to_glo(3);

	IS xg(Xg);
	IS yg(Yg);
	IS zg(Zg);

	// is point in face extents

	/*	auto debug = [&] () {
	//print 'equ point'
	//print xg,yg,zg
	//print sxg,syg,szg
	//print pt
	//print face_->ext
	};*/
	
	BOOST_LOG_CHANNEL_SEV(gal::log::lg, "Diff2D", debug) << "search for " << pt.x << " " << pt.y << " " << pt.z << GAL_LOG_ENDLINE;
	BOOST_LOG_CHANNEL_SEV(gal::log::lg, "Diff2D", debug) << "extents are:" << GAL_LOG_ENDLINE;
	BOOST_LOG_CHANNEL_SEV(gal::log::lg, "Diff2D", debug) << "    " << face_->ext_->get(0,0) << " " << face_->ext_->get(0,1) << GAL_LOG_ENDLINE;
	BOOST_LOG_CHANNEL_SEV(gal::log::lg, "Diff2D", debug) << "    " << face_->ext_->get(1,0) << " " << face_->ext_->get(1,1) << GAL_LOG_ENDLINE;
	BOOST_LOG_CHANNEL_SEV(gal::log::lg, "Diff2D", debug) << "    " << face_->pos_z_ << GAL_LOG_ENDLINE;

	if(pt[xg.i] < face_->ext_->get(0,0)) {
		BOOST_LOG_CHANNEL_SEV(gal::log::lg, "Diff2D", debug) << "x " << pt[xg.i] << " < " << face_->ext_->get(0,0) << std::endl;
		throw point_not_found();
	}

	if(pt[xg.i] > face_->ext_->get(0,1)) {
		BOOST_LOG_CHANNEL_SEV(gal::log::lg, "Diff2D", debug) << "x " << pt[xg.i] << " > " << face_->ext_->get(0,1) << std::endl;
		throw point_not_found();
	}

	if(pt[yg.i] < face_->ext_->get(1,0)) {
		BOOST_LOG_CHANNEL_SEV(gal::log::lg, "Diff2D", debug) << "y " << pt[yg.i] << " > " << face_->ext_->get(1,0) << std::endl;
		throw point_not_found();
	}

	if(pt[yg.i] > face_->ext_->get(1,1)) {
		BOOST_LOG_CHANNEL_SEV(gal::log::lg, "Diff2D", debug) << "y " << pt[yg.i] << " > " << face_->ext_->get(1,1) << std::endl;
		throw point_not_found();
	}

	if(pt[zg.i] != face_->pos_z_) {
		BOOST_LOG_CHANNEL_SEV(gal::log::lg, "Diff2D", debug) << "z" << std::endl;
		throw point_not_found();
	}

	int i = round((pt[xg.i] - face_->ext_->get(0,0)) / face_->d_->get(0,0,0));
	int j = round((pt[yg.i] - face_->ext_->get(1,0)) / face_->d_->get(0,0,1));

	//print 'equ point i,j',i,j

	return v_->get(i,j);
}





