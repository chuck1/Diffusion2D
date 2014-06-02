#ifndef DIFF2D_UTIL_HPP
#define DIFF2D_UTIL_HPP


#include <algorithm>
#include <exception>
#include <vector>
#include <utility>

template<typename T> inline void write_vec(std::ofstream& ofs, std::vector<T> x) {
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



