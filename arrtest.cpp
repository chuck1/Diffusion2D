#include <iostream>
#include <vector>
#include "array.hpp"


void foo(std::vector<int>) {}


int main() {

	foo({1,2});


	array<double,3> a;
	
	a.alloc<2,2,3>();
	
	a.get(0,0,0) = 1.0;
	a.get(1,0,0) = 2.0;

	for(int i = 0; i < a.n_[0]; ++i) {
		std::cout << a.get(i,0,0) << std::endl;
	}
	
	
}



