
#include <Diff2D/init.hpp>
#include <Diff2D/prob.hpp>
#include <Diff2D/patch_group.hpp>
#include <Diff2D/stitch.hpp>
#include <Diff2D/log.hpp>
#include <Diff2D/boundary.hpp>

void solve_source(std::shared_ptr<Prob> prob) {
	//prob->solve2(1e-4, 1e-2, True);

	prob->write_binary("s");
	prob->write("s");

	prob->solve("s", 1e-4, 0, 0);

	prob->write_binary("s");
	prob->write("s");

}
void solve_temp(std::shared_ptr<Prob> prob) {
	//prob->solve2(1e-4, 1e-2, True);

	prob->write_binary("T");
	prob->write("T");
	
	prob->solve("T", 1e-4, 0, 0);

	prob->write_binary("T");
	prob->write("T");

}
void solve_with_source(std::shared_ptr<Prob> prob) {
	//prob->solve2(1e-4, 1e-2, True);

	prob->solve("s", 1e-4, 0, 0);

	prob->write_binary("s");

	prob->value_add("s", -1.0);
	prob->value_normalize("s");
	prob->copy_value_to_source("s","T");

	prob->solve2("T", 1e-2, 1e-3);

	prob->write_binary("T");
	prob->write("T");
}

int main(int ac, char** av) {
	// program

	d2d::log::init();

	// solve

	size_t n = 16;
	
	coor_type x;
	cell_count_type N;

	/*x.push_back(make_array_1<real,1>({0.0, 1.0, 2.0, 3.0}));
	x.push_back(make_array_1<real,1>({0.0, 1.0, 2.0, 3.0}));
	x.push_back(make_array_1<real,1>({0.0, 1.0, 2.0, 3.0}));*/
	
	x.push_back(make_array_1<real,1>({0.0, 1.0, 2.0}));
	x.push_back(make_array_1<real,1>({0.0, 1.0, 2.0}));
	x.push_back(make_array_1<real,1>({0.0, 1.0, 2.0}));

	N.push_back(make_array_1<size_t,1>({n+0,n+0}));
	N.push_back(make_array_1<size_t,1>({n+2,n+2}));
	N.push_back(make_array_1<size_t,1>({n+4,n+4}));
	
	
	auto prob = std::make_shared<Prob>("test", x, N, 1000, 1000);

	prob->create_equation("T", 10.0, 1.5, 1.5);
	prob->create_equation("s", 10.0, 1.5, 1.5);

	//prob.get_3d_axes()
	//sys.exit(0)
	
	Patch_s	p0, p1, p2, p3, p4, p5;
	
	point pt2(1.5,1.5,3.0);
	point pt3(0.0,1.5,1.5);
	point pt4(1.5,0.0,1.5);
	point pt5(1.5,1.5,0.0);
	

	auto g2 = prob->create_patch_group("2", {{"T",20.0},{"s",2.0}}, {{"T",  0.0},{"s",100.0}}, pt2);
	auto g3 = prob->create_patch_group("3", {{"T", 0.0},{"s",2.0}}, {{"T",  0.0},{"s",100.0}}, pt3);
	auto g4 = prob->create_patch_group("4", {{"T", 0.0},{"s",2.0}}, {{"T",  0.0},{"s",100.0}}, pt4);
	auto g5 = prob->create_patch_group("5", {{"T", 0.0},{"s",2.0}}, {{"T",  0.0},{"s",100.0}}, pt5);
	
	auto const_bou(std::make_shared<boundary_single>(1.0));

	auto const_bou2(std::make_shared<boundary_single>(10));
	auto const_bou3(std::make_shared<boundary_single>(20));

	patch_v_bou_vec_type v_bou_T_zm({
			{
			{const_bou2},{const_bou2}
			},
			{
			{const_bou2},{const_bou2}
			}
			});

	patch_v_bou_vec_type v_bou_T_zp({
			{
			{const_bou3},{const_bou3}
			},
			{
			{const_bou3},{const_bou3}
			}
			});

	

	patch_v_bou_vec_type v_bou_s_def({
			{
			{const_bou,const_bou},
			{const_bou,const_bou}
			},
			{
			{const_bou,const_bou},
			{const_bou,const_bou}
			}
			});

	patch_v_bou_type v_bou_def({{"s",v_bou_s_def}});

	patch_v_bou_type v_bou_zm({{"s",v_bou_s_def},{"T",v_bou_T_zm}});
	patch_v_bou_type v_bou_zp({{"s",v_bou_s_def},{"T",v_bou_T_zp}});

	/*	v_bou_def["T"] = {
		{30.0,30.0},
		{30.0,30.0}
		};

		v_bou_def["s"] = {{1.0,1.0},{1.0,1.0}};*/

	//p0 = prob.createPatch(1,	[1,	[0,1],	[0,1]])
	//p1 = prob.createPatch(2,	[[0,1],	1,	[0,1]])

	p2 = g2->create_patch("2",3,	{0,1,2},	{0,1,2},	{2},		v_bou_zp);
	p3 = g3->create_patch("3",-1,	{0},		{2,1,0},	{2,1,0},	v_bou_def);
	p4 = g4->create_patch("4",-2,	{2,1,0},	{0},		{2,1,0},	v_bou_def);
	p5 = g5->create_patch("5",-3,	{2,1,0},	{2,1,0},	{0},		v_bou_zm);


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


	//prob.solve2(1e-2, 1e-4, True)

	//profile.run("prob.solve('s', 1e-1)")


	//prob.solve('T', 1e-1, True)
	
	
	
	//solve_with_source(prob);
	//solve_source(prob);
	solve_temp(prob);

}


