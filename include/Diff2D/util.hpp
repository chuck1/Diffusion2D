#ifndef DIFF2D_UTIL_HPP
#define DIFF2D_UTIL_HPP


#include <algorithm>
#include <exception>
#include <vector>
#include <utility>



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

std::pair< std::vector<size_t>, std::vector<size_t> >		align(std::vector<size_t> ind1, std::vector<size_t> ind2);



#endif



