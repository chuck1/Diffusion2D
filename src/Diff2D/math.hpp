#ifndef MATH_HPP
#define MATH_HPP

std::vector<int>			range(int end) {
	std::vector<int> ret;
	for(int i = 0; i < end; ++i) {
		ret.push_back(i);
	}
	return ret;
}
template<typename T> std::vector<T>	range(T start, T end, T step);

#endif



