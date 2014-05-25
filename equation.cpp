#include "equation.hpp"
#include "face.hpp"

Equation_Prob::Equation_Prob(Prob* prob, std::string name, real k, real alpha, real alpha_source) {
	prob_ = prob;
	name_ = name;
	k_ = k;
	alpha_ = alpha;
	alpha_source_ = alpha_source;
}


Equ::Equ(std::string name, Face* face, Equation_Prob* equ_prob) {
	name_ = name;
	face_ = face;

	equ_prob_ = equ_prob;

	//v_0_ = v_0

	array<int,1> n_extended = face_->n + np.array([2, 2]);

	v_ = np.ones(n_extended) * face_->patch.group.v_0[name_];

	s_ = np.ones(face.n);

	flag_ = 0;
}	
array<real,2>		Equ::grad() {
	//return np.gradient(v_[:-2,:-2], face_->d[0,0,0], face_.d[0,0,1])
	return v_.sub(0,0,-2,-2).gradient(face_->d_);
}
real			Equ::grad_mag() {
	return sqrt(grad().square().sum());
}
real		Equ::min() {
	return v_.sub({0,0},{-2,-2}).min();
}
real		Equ::max() {
	return v_.sub({0,0},{-2,-2}).max();
}
real		Equ::grad_min() {
	return np.min(grad_mag()[:-2,:-2]);
}
real		Equ::grad_max() {
	return np.max(grad_mag()[:-2,:-2]);
}
real		Equ::mean() {
	return np.mean(v_[:-2,:-2]);
}

