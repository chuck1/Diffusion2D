#include <Diff2D/conn.hpp>
#include <Diff2D/patch.hpp>
#include <Diff2D/util.hpp>
#include <Diff2D/stitch.hpp>

void		stitch(Patch_s patch1, Patch_s patch2) {
	if(!patch1) return;
	if(!patch2) return;
	
	bool ver = false;
	//ver = True

	if(ver) {
	//	print "stitch"	
	//	print "patch1.Z", patch1.Z
	//	print "patch2.Z", patch2.Z
	}

	if(patch1->Z_ == patch2->Z_) {
		stitch_ortho(patch1, patch2);
		return;
	}
	
	// global direction parallel to common edge
	int P = cross(patch1->Z_, patch2->Z_);

	IS pg(P);
	
	int PL1 = patch1->glo_to_loc(P);
	int PL2 = patch2->glo_to_loc(P);

	IS og1 = patch2->z_;
	IS og2 = patch1->z_;
	
	IS ol1(patch1->glo_to_loc(patch2->Z_));
	IS ol2(patch2->glo_to_loc(patch1->Z_));
	
	if(ver) {
		//print "ol1", ol1, "ol2", ol2
	}

	{	
		auto vec1 = patch1->indices_[og1.i];

		auto it1 = std::find(vec1.begin(), vec1.end(), patch2->indices_[patch2->z_.i][0]);

		if(it1 == vec1.end()) {
			ol1.s = -1;
		} else {
			ol1.s = 1;
		}
	}

	{
		auto vec2 = patch2->indices_[og2.i];

		auto it2 = std::find(vec2.begin(), vec2.end(), patch1->indices_[patch1->z_.i][0]);

		if(it2 == vec2.end()) {
			ol2.s = -1;
		} else {
			ol2.s = 1;
		}
	}

	IS pl1(PL1);
	IS pl2(PL2);

	//int n1 = patch1->npatch_->get(pl1.i);
	//int n2 = patch2->npatch_->get(pl2.i);

	int ind1[] = {0,0};
	int ind2[] = {0,0};

	ind1[ol1.i] = (ol1.s < 0) ? 0 : (patch1->npatch_->get(ol1.i) - 1);
	ind2[ol2.i] = (ol2.s < 0) ? 0 : (patch2->npatch_->get(ol2.i) - 1);

	auto align_ret = align(patch1->indices_[pg.i], patch2->indices_[pg.i]);
	auto r1 = align_ret.first;
	auto r2 = align_ret.second;

	auto it1 = r1.begin();
	auto it2 = r2.begin();

	for(; it1 != r1.end(); ++it1, ++it2) {

		ind1[pl1.i] = *it1;
		ind2[pl2.i] = *it2;

		auto f1 = patch1->faces_->get(ind1[0], ind1[1]);
		auto f2 = patch2->faces_->get(ind2[0], ind2[1]);

		//f1.nbrs[ol1,(sol1+1)/2] = f2
		//f2.nbrs[ol2,(sol2+1)/2] = f1

		connect(f1, ol1.i, (ol1.s+1)/2, f2, ol2.i, (ol2.s+1)/2);
	}
}

void		stitch_ortho(Patch_s patch1, Patch_s patch2) {
	std::cout << "stitch ortho '" << patch1->name_ << "' '" << patch2->name_ << "'" << std::endl;

	bool ver = false;
	//ver = True

	if(ver); //print "stitch_ortho"

	int ind1[] = {0,0};
	int ind2[] = {0,0};
	
	std::pair< std::vector<size_t>, std::vector<size_t> > align_ret;
	
	int o, p;
	bool rev;

	try {
		align_ret = align(patch1->indices_[patch1->x_.i], patch2->indices_[patch2->x_.i]);
		//r1, r2 = r01, r02;
	} catch(EdgeError e) {
		o = 0;
		p = 1;
		rev = e.rev_;
	} catch(...) {
		std::cout << "unknwon error" << std::endl;
		abort();
	}



	try{
		align_ret = align(patch1->indices_[patch1->y_.i], patch2->indices_[patch2->y_.i]);
		//r1, r2 = r11, r12;
	} catch(EdgeError e) {
		o = 1;
		p = 0;
		rev = e.rev_;
	} catch(...) {
		std::cout << "unknwon error" << std::endl;
		abort();
	}
	
	IS ol1, ol2;	
	
	if(rev) {
		ind1[o] = 0;
		ind2[o] = patch2->npatch_->get(o) - 1;

		ol1.s = -1;
		ol2.s = 1;
	} else {
		ind1[o] = patch1->npatch_->get(o) - 1;
		ind2[o] = 0;

		ol1.s = 1;
		ol2.s = -1;
	}
	/*if ver:
	  print "o   ",o
	  print "sol1",sol1
	  print "sol2",sol2*/
	
	auto r1 = align_ret.first;
	auto r2 = align_ret.second;
	
	auto it1 = r1.begin();
	auto it2 = r2.begin();
	
	for(; it1 != r1.end(); ++it1, ++it2) {
		ind1[p] = *it1;
		ind2[p] = *it2;
	
		std::cout
				<< "ind1 " << ind1[0] << " " << ind1[1] << " "
				<< "ind2 " << ind2[0] << " " << ind2[1] << std::endl;

		auto f1 = patch1->faces_->get(ind1[0],ind1[1]);
		auto f2 = patch2->faces_->get(ind2[0],ind2[1]);

		if(f1->conns_[o][(ol1.s+1)/2]) {

			std::cout << "f1 nbr not none" << std::endl;

			std::cout << f1->conns_[o][(ol1.s+1)/2] << std::endl;

			std::cout
				<< "ind1 " << ind1[0] << " " << ind1[1] << " "
				<< "ind2 " << ind2[0] << " " << ind2[1] << std::endl;

			std::cout << "o = " << o << " ol1.s = " << ol1.s << " ol2.s = " << ol2.s << std::endl;

			continue;
			abort();
		}
		if(f2->conns_[o][(ol2.s+1)/2]) {
			std::cout << "f2 nbr not none" << std::endl;
			std::cout
				<< "ind1 " << ind1[0] << " " << ind1[1] << " "
				<< "ind2 " << ind2[0] << " " << ind2[1] << std::endl;

			continue;
			abort();
		}

		connect(f1, o, (ol1.s + 1)/2, f2, o, (ol2.s + 1)/2);
		//f1.nbrs[o,(sol1+1)/2] = f2
		//f2.nbrs[o,(sol2+1)/2] = f1
	}

}


