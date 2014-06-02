#ifndef DIFF2D_BOUNDARY_HPP
#define DIFF2D_BOUNDARY_HPP

#include <math-array/array.hpp>

#include <Diff2D/config.hpp>
#include <Diff2D/decl.hpp>
#include <Diff2D/equation.hpp>

struct boundary {
	virtual void		eval(std::shared_ptr<Equation> const & equ, std::vector<int> const & ind, std::vector<int> const & indn, unsigned int p) = 0;
};

struct boundary_single: boundary {
	boundary_single(real v): v_(v) {}

	virtual void		eval(std::shared_ptr<Equation> const & equ, std::vector<int> const & ind, std::vector<int> const & indn, unsigned int p) {
		equ->v_->get(indn[0],indn[1]) = 2.0 * v_ - equ->v_->get(ind);
	}
	real		v_;
};

struct boundary_array: boundary {
	boundary_array(array<real,1> v): v_(v) {}

	virtual void		eval(std::shared_ptr<Equation> const & equ, std::vector<int> const & ind, std::vector<int> const & indn, unsigned int p) {
		equ->v_->get(indn[0],indn[1]) = 2.0 * v_->get(ind[p]) - equ->v_->get(ind);
	}

	array<real,1>	v_;
};

struct boundary_insulated: boundary {
	virtual void		eval(std::shared_ptr<Equation> const & equ, std::vector<int> const & ind, std::vector<int> const & indn, unsigned int p) {
		equ->v_->get(indn[0],indn[1]) = equ->v_->get(ind);
	}
};

#endif

