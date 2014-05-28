#include <algorithm>
#include <exception>
#include <vector>
#include <utility>
#include <cassert>

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

typedef std::vector<size_t>			size_t_vec;
typedef std::pair< size_t_vec, size_t_vec >	align_ret_type;

align_ret_type		align(size_t_vec ind1, size_t_vec ind2) {
	bool ver = false;
	//#ver = True
	
	auto ind1_min = std::min_element(ind1.cbegin(), ind1.cend());
	assert(ind1_min != ind1.cend());

	auto ind2_min = std::min_element(ind2.cbegin(), ind2.cend());
	assert(ind2_min != ind2.cend());

	auto ind1_max = std::max_element(ind1.cbegin(), ind1.cend());
	assert(ind1_max != ind1.cend());

	auto ind2_max = std::max_element(ind2.cbegin(), ind2.cend());
	assert(ind2_max != ind2.cend());


	int s = std::max(*ind1_min,*ind2_min);

	int e = std::min(
			*ind1_max,
			*ind2_max
		   );
	
	if(ver) {
		//print "ind1", ind1
		//print "ind2", ind2
		//print "s", s, "e", e
	}
	
	
	auto s1 = std::find(ind1.cbegin(), ind1.cend(), s);
	auto e1 = std::find(ind1.cbegin(), ind1.cend(), e);
	auto s2 = std::find(ind2.cbegin(), ind2.cend(), s);
	auto e2 = std::find(ind2.cbegin(), ind2.cend(), e);
	
	if(
			s1 == ind1.cend() || 
			e1 == ind1.cend() || 
			s2 == ind2.cend() || 
			e2 == ind2.cend()) {
		throw NoIntersectError();
	}
	
	if(ver) {
		//print "s1", s1, "e1", e1
		//print "s2", s2, "e2", e2
	}
	
	if(s == e) throw EdgeError(s1 < s2);
	
	int d1 = sgn(*e1-*s1);
	int d2 = sgn(*e2-*s2);
	
	if(ver) {
	/*	print ind1
		print ind2

		print "min",min(ind1), min(ind2), "s", s
		print "max",max(ind1), max(ind2), "e", e
	
		print s1, e1, d1
		print s2, e2, d2*/
	}
	
	auto i1 = [&] (int i) {
		return *s1 + (i - (d1-1)/2) * d1;
	};
	auto i2 = [&] (int i) {
		return *s2 + (i - (d2-1)/2) * d2;
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
	
	return make_pair(r1,r2);
}
	
	


