#ifndef DIFF2D_UTIL_HPP
#define DIFF2D_UTIL_HPP

#include <iostream>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <exception>
#include <vector>
#include <utility>

template<typename LastArg> void					print_row(unsigned int w, const LastArg& last) {
	std::cout << std::setw(w) << last << std::endl;
}
template<typename FirstArg, typename... Args> void		print_row(unsigned int w, const FirstArg& first, const Args&... args) {
	std::cout << std::setw(w) << first;
	print_row(w,args...);
}

template<typename T, typename STRM> inline STRM&		operator<<(STRM& strm, std::vector<T> const & vec) {
	size_t w = strm.width();
	for(auto t : vec) {
		strm << std::setw(w) << t;
	}
}

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

template<typename T> size_t		indexof(std::vector<T> const & vec, T const & t) {
	auto it = std::find(vec.begin(), vec.end(), t);
	if(it == vec.cend()) throw 0;
	return (it - vec.begin());
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



