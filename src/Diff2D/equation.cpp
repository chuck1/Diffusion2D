#include "array.hpp"
#include "equation.hpp"
#include "face.hpp"


Equation_Prob::Equation_Prob(Prob* prob, std::string name, real k, real alpha, real alpha_source) {
	prob_ = prob;
	name_ = name;
	k_ = k;
	alpha_ = alpha;
	alpha_source_ = alpha_source;
}


Equation::Equation(std::string name, Face* face, Equation_Prob* equ_prob) {
	name_ = name;
	face_ = face;

	equ_prob_ = equ_prob;

	//v_0_ = v_0

	array<int,1> n_extended = face_->n_->add(make_array<int,1>({2, 2}));
	
	v_->ones(n_extended);
	
	v_->multiply_self(face_->patch_->group_->v_0_[name_]);
	
	s_->ones(face->n_);

	flag_ = 0;
}
array<real,2>		Equation::grad() {
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







