#include <iostream>
#include <vector>
#include <map>

#include <Diff2D/array.hpp>


void foo(std::vector<int>) {}


int main() {

	foo({1,2});
	
	std::map<std::string, array< array<int,1>, 2> > a2;

	auto a = make_uninit<double,3>({2,3,4});
	
	a->alloc({2,2,3});
	
	a->get(0,0,0) = 1.0;
	a->get(1,0,0) = 2.0;

	for(size_t i = 0; i < a->n_[0]; ++i) {
		std::cout << a->get(i,0,0) << std::endl;
	}
	
	
}



