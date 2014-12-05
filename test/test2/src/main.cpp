
#include <Diff2D/init.hpp>
#include <Diff2D/prob.hpp>
#include <Diff2D/patch_group.hpp>
#include <Diff2D/stitch.hpp>
#include <Diff2D/log.hpp>
#include <Diff2D/boundary.hpp>

void solve_source(std::shared_ptr<Prob> prob) {
	//prob->solve2(1e-4, 1e-2, True);

	//prob->write_binary("s");
	//prob->write("s");

	prob->solve("s", 1e-4, 0, 0);

	//prob->write_binary("s");
	//prob->write("s");

}
void solve_temp(std::shared_ptr<Prob> prob) {
	//prob->solve2(1e-4, 1e-2, True);

	//prob->write_binary("T");
	//prob->write("T");
	
	prob->solve("T", 1e-4, 0, 0);

	//prob->write_binary("T");
	//prob->write("T");

}
void solve_with_source(std::shared_ptr<Prob> prob) {
	//prob->solve2(1e-4, 1e-2, True);

	prob->solve("s", 1e-4, 0, 0);

	//prob->write_binary("s");

	prob->value_add("s", -1.0);
	prob->value_normalize("s");
	prob->copy_value_to_source("s","T");

	prob->solve2("T", 1e-4, 1e-6);

	//prob->write_binary("T");
	prob->write("T");
}

int main(int ac, char** av) {
	// program

	d2d::log::init();

	// solve

	size_t n = 16;
	
	coor_type x;
	cell_count_type N;

	x.push_back(math::make_array_1<real,1>({0.0, 1.0, 2.0}));
	x.push_back(math::make_array_1<real,1>({0.0, 1.0, 2.0}));
	x.push_back(math::make_array_1<real,1>({0.0}));

	N.push_back(math::make_array_1<size_t,1>({n+0,n+0}));
	N.push_back(math::make_array_1<size_t,1>({n+2,n+2}));
	N.push_back(math::make_array_1<size_t,1>({n+4,n+4}));
	
	
	auto prob = std::make_shared<Prob>("test", x, N, 1E3, 1E2);

	prob->create_equation("T", 10.0, 1.5, 1.5);
	prob->create_equation("s", 10.0, 1.5, 1.5);

	point pt1(0.5,0.5,0.0);
	point pt2(1.5,1.5,0.0);

	auto g1 = prob->create_patch_group("1", {{"T",20.0},{"s",2.0}}, {{"T",  0.0},{"s",100.0}}, pt1);
	auto g2 = prob->create_patch_group("2", {{"T",40.0},{"s",2.0}}, {{"T",  0.0},{"s",100.0}}, pt2);
	
	auto const_bou1(std::make_shared<boundary_single>(1.0));

	auto const_bou2(std::make_shared<boundary_single>(10));
	auto const_bou3(std::make_shared<boundary_single>(10));
	//auto const_bou2(std::make_shared<boundary_insulated>());
	//auto const_bou3(std::make_shared<boundary_insulated>());

	patch_v_bou_vec_type v_bou_T_1({
			{
			{const_bou2},{const_bou2}
			},
			{
			{const_bou2},{const_bou2}
			}
			});

	patch_v_bou_vec_type v_bou_T_2({
			{
			{const_bou3},{const_bou3}
			},
			{
			{const_bou3},{const_bou3}
			}
			});


	patch_v_bou_vec_type v_bou_s_def({
			{
			{const_bou1},{const_bou1}
			},
			{
			{const_bou1},{const_bou1}
			}
			});

	patch_v_bou_type v_bou_def({{"s",v_bou_s_def}});

	patch_v_bou_type v_bou_1({{"s",v_bou_s_def},{"T",v_bou_T_1}});
	patch_v_bou_type v_bou_2({{"s",v_bou_s_def},{"T",v_bou_T_2}});

	/*	v_bou_def["T"] = {
		{30.0,30.0},
		{30.0,30.0}
		};

		v_bou_def["s"] = {{1.0,1.0},{1.0,1.0}};*/

	auto p1 = g1->create_patch("1",3,	{0,1},	{0,1},	{0},	v_bou_1);
	auto p2 = g1->create_patch("2",3,	{1,2},	{0,1},	{0},	v_bou_2);
	auto p3 = g2->create_patch("3",3,	{0,1},	{1,2},	{0},	v_bou_1);
	auto p4 = g2->create_patch("4",3,	{1,2},	{1,2},	{0},	v_bou_2);

	stitch(p1,p2);
	stitch(p1,p3);
	stitch(p2,p4);
	stitch(p3,p4);

	solve_with_source(prob);
	//solve_source(prob);
	//solve_temp(prob);

}


