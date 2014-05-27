#ifndef DIFF2D_UTIL_HPP
#define DIFF2D_UTIL_HPP


#include <algorithm>
#include <exception>
#include <vector>
#include <utility>

#include <Diff2D/math.hpp>


template <typename T> int sgn(T val) {
	return (T(0) < val) - (val < T(0));
}

struct NoIntersectError: std::exception {
};
struct EdgeError: std::exception {
	EdgeError(bool rev) {
		rev_ = rev;
	}
	bool rev_;
};

std::pair< std::vector<int>, std::vector<int> >		align(std::vector<int> ind1, std::vector<int> ind2);



#endif



