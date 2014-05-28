
#include <Diff2D/prob.hpp>
#include <Diff2D/patch_group.hpp>
#include <Diff2D/stitch.hpp>

int main() {

	auto arr = make_uninit<int,3>({2,3,4});

	for(size_t i : range(2)) {
		for(size_t j : range(3)) {
			for(size_t k : range(4)) {

				std::cout << arr->get(i,j,k) << std::endl;
			}
		}
	}	
}


