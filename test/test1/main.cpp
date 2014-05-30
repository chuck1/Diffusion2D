
#include <Diff2D/init.hpp>
#include <Diff2D/prob.hpp>
#include <Diff2D/patch_group.hpp>
#include <Diff2D/stitch.hpp>
#include <Diff2D/log.hpp>


int main(int ac, char** av) {
	// program
	d2d::log::init();

	d2d::log::core = d2d::log::sl::debug;
	
	// solve

	size_t n = 10;
	
	coor_type x;
	cell_count_type N;

	x.push_back(make_array_1<real,1>({0.0, 1.0, 2.0, 3.0}));
	x.push_back(make_array_1<real,1>({0.0, 1.0, 2.0, 3.0}));
	x.push_back(make_array_1<real,1>({0.0, 1.0, 2.0, 3.0}));
	
	N.push_back(make_array_1<size_t,1>({n+0,n+0}));
	N.push_back(make_array_1<size_t,1>({n+2,n+2}));
	N.push_back(make_array_1<size_t,1>({n+4,n+4}));
	
	
	auto prob = std::make_shared<Prob>("test4", x, N, 1000, 1000);

	prob->create_equation("T", 10.0, 1.5, 1.5);
	prob->create_equation("s", 10.0, 1.5, 1.5);

	//prob.get_3d_axes()
	//sys.exit(0)
	
	Patch_s	p0, p1, p2, p3, p4, p5;
	
	auto g2 = prob->create_patch_group("2", {{"T",10.0},{"s",2.0}}, {{"T",0.0},{"s",10.0}});
	auto g3 = prob->create_patch_group("3", {{"T", 0.0},{"s",2.0}}, {{"T",0.0},{"s",10.0}});
	auto g4 = prob->create_patch_group("4", {{"T", 0.0},{"s",2.0}}, {{"T",0.0},{"s",10.0}});
	auto g5 = prob->create_patch_group("5", {{"T", 0.0},{"s",2.0}}, {{"T",0.0},{"s",10.0}});
	
	patch_v_bou_type v_bou_def;

/*	v_bou_def["T"] = {
		{30.0,30.0},
		{30.0,30.0}
	};

	v_bou_def["s"] = {{1.0,1.0},{1.0,1.0}};*/
	
	//p0 = prob.createPatch(1,	[1,	[0,1],	[0,1]])
	//p1 = prob.createPatch(2,	[[0,1],	1,	[0,1]])
	
	p2 = g2->create_patch("2",3,	{0,1,2},	{0,1,2},	{2},		v_bou_def);

	p3 = g3->create_patch("3",-1,	{0},		{2,1,0},	{2,1,0},	v_bou_def);
	p4 = g4->create_patch("4",-2,	{2,1,0},	{0},		{2,1,0},	v_bou_def);
	p5 = g5->create_patch("5",-3,	{2,1,0},	{2,1,0},	{0},		v_bou_def);


	stitch(p0,p1);
	stitch(p0,p2);
	stitch(p0,p4);
	stitch(p0,p5);

	stitch(p1,p2);
	stitch(p1,p3);
	stitch(p1,p5);

	stitch(p2,p3);
	stitch(p2,p4);

	stitch(p3,p4);
	stitch(p3,p5);

	stitch(p4,p5);

	Face_s f0, f1, f2, f3, f4, f5;

	//f0 = p0.faces[0,0]
	//f1 = p1.faces[0,0]
/*	f2 = p2->faces_->get(0,0];
	f3 = p3->faces_[0,0];
	f4 = p4->faces_[0,0];
	f5 = p5->faces_[0,0];
*/
	
	//f0.create_equ('T', 0., [[30.,0.],[0.,0.]], k, al)
	
	//f1.create_equ('T', 0., [[30.,0.],[0.,0.]], k, al)

/*	f2.equs['T'].v_bou = [[30.,30.],[30.,30.]];
	f3.equs['T'].v_bou = [[30.,30.],[30.,30.]];
	f4.equs['T'].v_bou = [[30.,30.],[30.,30.]];
	f5.equs['T'].v_bou = [[10.,10.],[10.,10.]];
*/
	
	prob->write("T");

	return 0;

	//prob.solve2(1e-2, 1e-4, True)
	
	//profile.run("prob.solve('s', 1e-1)")
	prob->solve("s", 1e-3, true, 0, 0.0);
	
	
	prob->value_add("s", -1.0);

	prob->value_normalize("s");

	//prob.copy_value_to_source('s','T')

	//prob.solve('T', 1e-1, True)
	prob->solve2("T", 1e-2, 1e-2, true);

	//prob.plot3()

	//prob.plot('s')
	//prob->.plot('T');

	//pl.show()

	prob->write("T");


}


