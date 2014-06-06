#include <algorithm>
#include <exception>
#include <vector>
#include <utility>
#include <cassert>

#include <iostream>
#include <iomanip>

#include <math-array/range.hpp>

#include <Diff2D/util.hpp>


/*struct NoIntersectError: std::exception {
};
struct EdgeError: std::exception {
	EdgeError(bool rev) {
		rev_ = rev;
	}
	bool rev_;
};
*/
typedef std::vector<size_t>			size_t_vec;
typedef std::pair< size_t_vec, size_t_vec >	align_ret_type;


align_ret_type		align(size_t_vec ind1, size_t_vec ind2) {
#ifdef _DEBUG
	std::cout << "align" << std::endl;
	std::cout << "ind1 ";
	for(auto i : ind1) std::cout << i << " "; std::cout << std::endl;
	std::cout << "ind2 ";
	for(auto i : ind2) std::cout << i << " "; std::cout << std::endl;
#endif
	bool ver = false;
	//#ver = True

	auto ind1_min = std::min_element(ind1.cbegin(), ind1.cend());
	auto ind1_max = std::max_element(ind1.cbegin(), ind1.cend());
	assert(ind1_min != ind1.cend());
	assert(ind1_max != ind1.cend());

	auto ind2_min = std::min_element(ind2.cbegin(), ind2.cend());
	auto ind2_max = std::max_element(ind2.cbegin(), ind2.cend());
	assert(ind2_min != ind2.cend());
	assert(ind2_max != ind2.cend());
	
	size_t s = std::max(*ind1_min,*ind2_min);
	size_t e = std::min(*ind1_max,*ind2_max);

#ifdef _DEBUG
	print_row(8,"ind1","ind2");
	print_row(4,"min","max","min","max","s","e");
	print_row(4,*ind1_min,*ind1_max,*ind2_min,*ind2_max,s,e);
#endif



/*	auto s1 = std::find(ind1.cbegin(), ind1.cend(), s);
	auto e1 = std::find(ind1.cbegin(), ind1.cend(), e);
	auto s2 = std::find(ind2.cbegin(), ind2.cend(), s);
	auto e2 = std::find(ind2.cbegin(), ind2.cend(), e);*/

	size_t s1 = indexof(ind1, s);
	size_t e1 = indexof(ind1, e);
	size_t s2 = indexof(ind2, s);
	size_t e2 = indexof(ind2, e);

/*	if(
			s1 == ind1.cend() || 
			e1 == ind1.cend() || 
			s2 == ind2.cend() || 
			e2 == ind2.cend()) {
		throw NoIntersectError();
	}*/

	//if(s == e) throw EdgeError((s1-ind1.cbegin()) < (s2-ind2.cbegin()));
	if(s == e) throw EdgeError(s1 < s2);

	int d1 = sgn((int)e1-(int)s1);
	int d2 = sgn((int)e2-(int)s2);

#ifdef _DEBUG
	print_row(4,"s1","e1","s2","e2","d1","d2");
	print_row(4,s1,e1,s2,e2,d1,d2);
#endif

	auto i1 = [&] (int i) {
		return s1 + (i - (d1-1)/2) * d1;
	};
	auto i2 = [&] (int i) {
		return s2 + (i - (d2-1)/2) * d2;
	};

	std::vector<size_t> r1;
	std::vector<size_t> r2;

	for(int i : range(abs(e1-s1))) {
		if(ver) {
			//print "1:", i1(i), "'", ind1[i1(i)], ind1[i1(i)+1], "'" //#, ind1[i+s1]
			//print "2:", i2(i), "'", ind2[i2(i)], ind2[i2(i)+1], "'" //#, ind2[i+s2]
		}

		r1.push_back(i1(i));
		r2.push_back(i2(i));
	}

	return make_pair(std::move(r1),std::move(r2));
}




