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

std::pair< std::vector<int>, std::vector<int> >		align(std::vector<int> ind1, std::vector<int> ind2) {
	bool ver = false;
	//#ver = True
	
	int s = std::max(
			*std::min_element(ind1.cbegin(), ind1.cend()),
			*std::min_element(ind2.cbegin(), ind1.cend())
			);

	int e = std::min(
			*std::max_element(ind1.cbegin(), ind1.cend()),
			*std::max_element(ind2.cbegin(), ind2.cend())
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
	
	std::vector<int> r1;
	std::vector<int> r2;
	
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
	
	


