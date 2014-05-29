#include <Diff2D/init.hpp>
#include <Diff2D/prob.hpp>
#include <Diff2D/patch_group.hpp>
#include <Diff2D/stitch.hpp>

int main(int ac, char** av) {
	
	d2d::log::init();

	// turn on array debug
	d2d::log::array = d2d::log::sl::debug;

	auto arr1 = make_uninit<int,2>({3,4});
	//auto arr2 = make_zeros<int,3>({1,2,3});
	
	for(size_t i : range(2)) {
		for(size_t j : range(3)) {
			//for(size_t k : range(4)) {
				arr1->get(i,j) = i + j;
			//}
		}
	}
	
	auto arr2 = arr1->sub({0,0},{2,3});
}


