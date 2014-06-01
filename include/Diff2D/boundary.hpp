#ifndef DIFF2D_BOUNDARY_HPP
#define DIFF2D_BOUNDARY_HPP

#include <math-array/array.hpp>

struct boundary {
	virtual void		operator()(std::shared_ptr<Equation> const & equ, std::vector<int> const & ind, std::vector<int> const & indn, unsigned int p) = 0;
};

struct boundary_single: boundary {
	virtual void		operator()(std::shared_ptr<Equation> const & equ, std::vector<int> const & ind, std::vector<int> const & indn, unsigned int p) {
		equ->v_->get(indn[0],indn[1]) = 2.0 * v_ - equ->v_->get(ind);
	}
	real		v_;
};

struct boundary_array: boundary {
	virtual void		operator()(std::shared_ptr<Equation> const & equ, std::vector<int> const & ind, std::vector<int> const & indn, unsigned int p) {
		equ->v_->get(indn[0],indn[1]) = 2.0 * v_->get(ind[p]) - equ->v_->get(ind);
	}
	array<real,1>	v_;
};

struct boundary_insulated: boundary {
	virtual void		operator()(std::shared_ptr<Equation> const & equ, std::vector<int> const & ind, std::vector<int> const & indn, unsigned int p) {
		equ->v_->get(indn[0],indn[1]) = equ->v_->get(ind);
	}
};

#endif

