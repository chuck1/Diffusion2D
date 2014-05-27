#ifndef STITCH_HPP
#define STITCH_HPP

#include <Diff2D/conn.hpp>
#include <Diff2D/patch.hpp>
#include <Diff2D/util.hpp>


void		stitch(Patch_s patch1, Patch_s patch2);
void		stitch_ortho(Patch_s patch1, Patch_s patch2);

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
	bool ver = false;
	//ver = True

	if(ver); //print "stitch_ortho"

	int ind1[] = {0,0};
	int ind2[] = {0,0};
	
	std::pair< std::vector<int>, std::vector<int> > align_ret;
	
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
		throw;
	}



	try{
		align_ret = align(patch1->indices_[patch1->y_.i], patch2->indices_[patch2->y_.i]);
		//r1, r2 = r11, r12;
	} catch(EdgeError as e) {
		o = 1;
		p = 0;
		rev = e.rev_;
	} catch {
		throw;
	}
	
	
	
	if(rev) {
		ind1[o] = 0;
		ind2[o] = patch2.npatch[o] - 1;

		sol1 = -1;
		sol2 = 1;
	} else {
		ind1[o] = patch1.npatch[o] - 1;
		ind2[o] = 0;

		sol1 = 1;
		sol2 = -1;
	}
	/*if ver:
	  print "o   ",o
	  print "sol1",sol1
	  print "sol2",sol2*/

	for i1, i2 in zip(r1, r2) {
		ind1[p] = i1;
		ind2[p] = i2;

		f1 = patch1.faces[ind1[0],ind1[1]];
		f2 = patch2.faces[ind2[0],ind2[1]];

		if(not f1.conns[o,(sol1+1)/2] is None) {
			//print "face1", ind1
			//print "face2", ind2
			throw 0;//raise ValueError('nbr not none')
		}
		if(not f2.conns[o,(sol2+1)/2] is None) {
			throw 0;//raise ValueError('nbr not none')
		}

		connect(f1, o, (sol1+1)/2, f2, o, (sol2+1)/2);
		//f1.nbrs[o,(sol1+1)/2] = f2
		//f2.nbrs[o,(sol2+1)/2] = f1
	}

}

#endif

