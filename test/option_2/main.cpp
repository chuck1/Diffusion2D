
#include <Diff2D/boundary.hpp>
#include <Diff2D/init.hpp>
#include <Diff2D/prob.hpp>
#include <Diff2D/patch_group.hpp>
#include <Diff2D/stitch.hpp>
#include <Diff2D/log.hpp>


void solve_source(std::shared_ptr<Prob> prob) {
	//prob->solve2(1e-4, 1e-2, True);
	
	prob->solve("s", 1e-4, 0, 0);

	prob->write_binary("s");
	prob->write("s");

}
void solve_temp(std::shared_ptr<Prob> prob) {
	//prob->solve2(1e-4, 1e-2, True);
	
	prob->solve("T", 1e-4, 0, 0);

	prob->write_binary("T");
	prob->write("T");

}
void solve_with_source(std::shared_ptr<Prob> prob) {
	//prob->solve2(1e-4, 1e-2, True);

	prob->solve("s", 1e-4, 0, 0);

	prob->write_binary("S");

	prob->value_add("s", -1.0);
	prob->value_normalize("s");
	prob->copy_value_to_source("s","T");

	prob->solve2("T", 1e-4, 1e-4);

	prob->write_binary("T");
	prob->write("T");

}

int main(int ac, char** av) {

	// solve


	real w_total = 6e-2;
	real w_irrad = 2e-2;
	real w_ends = (w_total - w_irrad) / 2.0;
	real pipe = 1.5e-3;
	real w_1 = (w_ends - pipe) / 2.0;

	real l_total = 4e-2;
	real l_irrad = 2e-2;
	real l_1 = (l_total - l_irrad) / 2.0;
	real l_3 = l_irrad - pipe * 2.0;

	auto xd = make_array_1<real,1>({0, w_1, pipe, w_1, w_irrad, w_1, pipe, w_1});

	auto yd = make_array_1<real,1>({0, 1e-2, 5e-2});

	auto zd = make_array_1<real,1>({0, l_1, pipe, l_3, pipe, l_1});

	real nom_size = 5e-4;

	auto nx = xd->sub({1},{-1})->divide(nom_size)->ceil<size_t>();
	auto ny = yd->sub({1},{-1})->divide(nom_size)->ceil<size_t>();
	auto nz = zd->sub({1},{-1})->divide(nom_size)->ceil<size_t>();
	
	auto x = xd->cumsum();
	auto y = yd->cumsum();
	auto z = zd->cumsum();

	// irradiated edge temperatures
	patch_v_bou_edge_vec_type T_irr_xm({
			std::make_shared<boundary_array>(make_ones<real,1>({nz->get(1)})->multiply_self(10.0)),
			std::make_shared<boundary_array>(make_ones<real,1>({nz->get(2)})->multiply_self(20.0)),
			std::make_shared<boundary_array>(make_ones<real,1>({nz->get(3)})->multiply_self(30.0))
			});


	patch_v_bou_edge_vec_type T_irr_xp({
			std::make_shared<boundary_array>(make_ones<real,1>({nz->get(1)})->multiply_self(10.0)),
			std::make_shared<boundary_array>(make_ones<real,1>({nz->get(2)})->multiply_self(20.0)),
			std::make_shared<boundary_array>(make_ones<real,1>({nz->get(3)})->multiply_self(30.0))
			});

	patch_v_bou_edge_vec_type T_irr_zm({
			std::make_shared<boundary_array>(make_ones<real,1>({nx->get(3)})->multiply_self(10.0))
			});

	patch_v_bou_edge_vec_type T_irr_zp({
			std::make_shared<boundary_array>(make_ones<real,1>({nx->get(3)})->multiply_self(30.0))
			});

	//nx = [10, 10, 10, 50, 10, 10, 10];
	//ny = [50, 50];
	//nz = [10, 10, 30, 10, 10];

	//print nx;
	//print ny;
	//print nz;


	coor_type X({x,y,z});

	cell_count_type n({nx,ny,nz});

	auto prob = std::make_shared<Prob>("opt2", X, n, 1E5, 1E5);

	prob->create_equation("T", 10.0, 1.5, 1.5);
	prob->create_equation("s", 10.0, 1.5, 1.5);

	real s = 1.0e10;

	// create patch groups;

	point pt_xm(
			0,
			(y->get(0) + y->get(1)) / 2.0,
			(z->get(2) + z->get(3)) / 2.0);

	point pt_xp(
			x->get(-1),
			(y->get(0) + y->get(1)) / 2.0,
			(z->get(2) + z->get(3)) / 2.0);

	point pt_ym(
			(x->get(1) + x->get(2)) / 2.0,
			0,
			(z->get(2) + z->get(3)) / 2.0);

	point pt_yp(
			(x->get(1) + x->get(2)) / 2.0,
			y->get(1),
			(z->get(2) + z->get(3)) / 2.0);

	point pt_zm(
			(x->get(3) + x->get(4)) / 2.0,
			(y->get(0) + y->get(1)) / 2.0,
			0);

	point pt_zp(
			(x->get(3) + x->get(4)) / 2.0,
			(y->get(0) + y->get(1)) / 2.0,
			z->get(5));


	auto g_xm		= prob->create_patch_group("xm",		{{"T",100.0},{"s",2.0}}, {{"T",0.0},{"s",s}}, pt_xm);
	auto g_xp		= prob->create_patch_group("xp",		{{"T",100.0},{"s",2.0}}, {{"T",0.0},{"s",s}}, pt_xp);
	auto g_ym		= prob->create_patch_group("ym",		{{"T",100.0},{"s",2.0}}, {{"T",0.0},{"s",s}}, pt_ym);
	auto g_yp		= prob->create_patch_group("yp",		{{"T",100.0},{"s",2.0}}, {{"T",0.0},{"s",s}}, pt_yp);
	auto g_zm		= prob->create_patch_group("zm",		{{"T",100.0},{"s",2.0}}, {{"T",0.0},{"s",s}}, pt_zm);
	auto g_zp		= prob->create_patch_group("zp",		{{"T",100.0},{"s",2.0}}, {{"T",0.0},{"s",s}}, pt_zp);
	
	
	// the default boundary for solve source is const = 1.0
	auto const_bou(std::make_shared<boundary_single>(1.0));
	patch_v_bou_vec_type v_bou_s_def({
			{
			{const_bou},
			{const_bou}
			},
			{
			{const_bou},
			{const_bou}
			}
			});

	patch_v_bou_type v_bou_def({{"s",v_bou_s_def}});

	patch_v_bou_type v_bou_irr_xm;
	patch_v_bou_type v_bou_irr_xp;
	patch_v_bou_type v_bou_irr_zm;
	patch_v_bou_type v_bou_irr_zp;


	// create patches
	// ym
	auto p_ym_0_0	= g_ym->create_patch("p_ym_0_0",	-2,	{0,1,2,3},	{0},	{0,1},		v_bou_def);
	auto p_ym_0_1	= g_ym->create_patch("p_ym_0_1",	-2,	{0,1,2,3},	{0},	{1,2,3,4},	v_bou_irr_xm);// = {"T":{{T_irr_xm,0.0},{0.0,0.0}},"s":{{1.0,1.0},{1.0,1.0}}});
	auto p_ym_0_2	= g_ym->create_patch("p_ym_0_2",	-2,	{0,1,2,3},	{0},	{4,5},		v_bou_def);// = {"T":{{0.0,0.0},{0.0,0.0}},	"s":{{1.0,1.0},{1.0,1.0}}});
	auto p_ym_1_0	= g_ym->create_patch("p_ym_1_0",	-2,	{3,4},		{0},	{0,1},		v_bou_irr_zm);// = {"T":{{0.0,0.0},{T_irr_zm,0.0}},"s":{{1.0,1.0},{1.0,1.0}}});
	auto p_ym_1_2	= g_ym->create_patch("p_ym_1_2",	-2,	{3,4},		{0},	{4,5},		v_bou_irr_zp);// = {"T":{{0.0,0.0},{0.0,T_irr_zp}},"s":{{1.0,1.0},{1.0,1.0}}});
	auto p_ym_2_0	= g_ym->create_patch("p_ym_2_0",	-2,	{4,5,6,7},	{0},	{0,1},		v_bou_def);// = {"T":{{0.0,0.0},{0.0,0.0}},	"s":{{1.0,1.0},{1.0,1.0}}});
	auto p_ym_2_1	= g_ym->create_patch("p_ym_2_1",	-2,	{4,5,6,7},	{0},	{1,2,3,4},	v_bou_irr_xp);// = {"T":{{0.0,T_irr_xp},{0.0,0.0}},"s":{{1.0,1.0},{1.0,1.0}}});
	auto p_ym_2_2	= g_ym->create_patch("p_ym_2_2",	-2,	{4,5,6,7},	{0},	{4,5},		v_bou_def);// = {"T":{{0.0,0.0},{0.0,0.0}},	"s":{{1.0,1.0},{1.0,1.0}}});

	// yp
	auto p_yp_0_0	= g_yp->create_patch("p_yp_0_0",	2,	{0,1,2,3},	{1},	{0,1},		v_bou_def);// = {"T":{{0.0,0.0},{0.0,0.0}},"s":{{1.0,1.0},{1.0,1.0}}});

	auto p_yp_0_1_0	= g_yp->create_patch("p_yp_0_1",	2,	{0,1},		{1},	{1,2,3,4},	v_bou_def);// = {"T":{{0.0,0.0},{0.0,0.0}},"s":{{1.0,1.0},{1.0,1.0}}});
	auto p_yp_0_1_1	= g_yp->create_patch("p_yp_0_1",	2,	{1,2},		{1},	{2,3,4},	v_bou_def);// = {"T":{{0.0,0.0},{0.0,0.0}},"s":{{1.0,1.0},{1.0,1.0}}});
	auto p_yp_0_1_2	= g_yp->create_patch("p_yp_0_1",	2,	{2,3},		{1},	{1,2,3,4},	v_bou_def);// = {"T":{{0.0,0.0},{0.0,0.0}},"s":{{1.0,1.0},{1.0,1.0}}});

	auto p_yp_0_2	= g_yp->create_patch("p_yp_0_2",	2,	{0,1,2,3},	{1},	{4,5},		v_bou_def);// = {"T":{{0.0,0.0},{0.0,0.0}},"s":{{1.0,1.0},{1.0,1.0}}});
	auto p_yp_1_0	= g_yp->create_patch("p_yp_1_0",	2,	{3,4},		{1},	{0,1},		v_bou_def);// = {"T":{{0.0,0.0},{0.0,0.0}},"s":{{1.0,1.0},{1.0,1.0}}});
	auto p_yp_1_1	= g_yp->create_patch("p_yp_1_1",	2,	{3,4},		{1},	{1,2,3,4},	v_bou_def);// = {"T":{{0.0,0.0},{0.0,0.0}},"s":{{1.0,1.0},{1.0,1.0}}});
	auto p_yp_1_2	= g_yp->create_patch("p_yp_1_2",	2,	{3,4},		{1},	{4,5},		v_bou_def);// = {"T":{{0.0,0.0},{0.0,0.0}},"s":{{1.0,1.0},{1.0,1.0}}});
	auto p_yp_2_0	= g_yp->create_patch("p_yp_2_0",	2,	{4,5,6,7},	{1},	{0,1},		v_bou_def);// = {"T":{{0.0,0.0},{0.0,0.0}},"s":{{1.0,1.0},{1.0,1.0}}});

	auto p_yp_2_1_0	= g_yp->create_patch("p_yp_2_1",	2,	{4,5},		{1},	{1,2,3,4},	v_bou_def);// = {"T":{{0.0,0.0},{0.0,0.0}},"s":{{1.0,1.0},{1.0,1.0}}});
	auto p_yp_2_1_1	= g_yp->create_patch("p_yp_2_1",	2,	{5,6},		{1},	{1,2,3},	v_bou_def);// = {"T":{{0.0,0.0},{0.0,0.0}},"s":{{1.0,1.0},{1.0,1.0}}});
	auto p_yp_2_1_2	= g_yp->create_patch("p_yp_2_1",	2,	{6,7},		{1},	{1,2,3,4},	v_bou_def);// = {"T":{{0.0,0.0},{0.0,0.0}},"s":{{1.0,1.0},{1.0,1.0}}});

	auto p_yp_2_2	= g_yp->create_patch("p_yp_2_2",	2,	{4,5,6,7},	{1},	{4,5},		v_bou_def);// = {"T":{{0.0,0.0},{0.0,0.0}},"s":{{1.0,1.0},{1.0,1.0}}});

	// xp
	auto p_xm	= g_xm->create_patch("p_xm",		-1,	{0},			{0,1},		{0,1,2,3,4,5},	v_bou_def);// = {"T":{{0.0,0.0},{0.0,0.0}},"s":{{1.0,1.0},{1.0,1.0}}});
	auto p_xp	= g_xp->create_patch("p_xp",		1,	{7},			{0,1},		{0,1,2,3,4,5},	v_bou_def);// = {"T":{{0.0,0.0},{0.0,0.0}},"s":{{1.0,1.0},{1.0,1.0}}});

	auto p_zm	= g_zm->create_patch("p_zm",		-3,	{7,6,5,4,3,2,1,0},	{1,0},		{0},		v_bou_def);// = {"T":{{0.0,0.0},{0.0,0.0}},"s":{{1.0,1.0},{1.0,1.0}}});
	auto p_zp	= g_zp->create_patch("p_zp",		3,	{0,1,2,3,4,5,6,7},	{0,1},		{5},		v_bou_def);// = {"T":{{0.0,0.0},{0.0,0.0}},"s":{{1.0,1.0},{1.0,1.0}}});


	// stitching
	// xm
	stitch(p_xm,p_ym_0_0);
	stitch(p_xm,p_ym_0_1);
	stitch(p_xm,p_ym_0_2);

	stitch(p_xm,p_yp_0_0);
	stitch(p_xm,p_yp_0_1_0);
	stitch(p_xm,p_yp_0_2);

	stitch(p_xm,p_zm);
	stitch(p_xm,p_zp);

	// xp
	stitch(p_xp,p_ym_2_0);
	stitch(p_xp,p_ym_2_1);;
	stitch(p_xp,p_ym_2_2);

	stitch(p_xp,p_yp_2_0);
	stitch(p_xp,p_yp_2_1_2);
	stitch(p_xp,p_yp_2_2);

	stitch(p_xp,p_zm);
	stitch(p_xp,p_zp);

	//zm;
	stitch(p_zm,p_ym_0_0);
	stitch(p_zm,p_ym_1_0);
	stitch(p_zm,p_ym_2_0);

	stitch(p_zm,p_yp_0_0);
	stitch(p_zm,p_yp_1_0);
	stitch(p_zm,p_yp_2_0);

	// zp;
	stitch(p_zp,p_ym_0_2);
	stitch(p_zp,p_ym_1_2);
	stitch(p_zp,p_ym_2_2);

	stitch(p_zp,p_yp_0_2);
	stitch(p_zp,p_yp_1_2);
	stitch(p_zp,p_yp_2_2);

	// yp;
	stitch(p_yp_0_0,	p_yp_1_0);
	stitch(p_yp_0_0,	p_yp_0_1_0);
	stitch(p_yp_0_0,	p_yp_0_1_2);

	stitch(p_yp_1_0,	p_yp_1_1);
	stitch(p_yp_1_0,	p_yp_2_0);

	stitch(p_yp_2_0,	p_yp_2_1_0);
	stitch(p_yp_2_0,	p_yp_2_1_1);
	stitch(p_yp_2_0,	p_yp_2_1_2);

	stitch(p_yp_0_1_0,	p_yp_0_1_1);
	stitch(p_yp_0_1_0,	p_yp_0_2);

	stitch(p_yp_0_1_1,	p_yp_0_1_2);
	stitch(p_yp_0_1_1,	p_yp_0_2);

	stitch(p_yp_0_1_2,	p_yp_1_1);
	stitch(p_yp_0_1_2,	p_yp_0_2);

	stitch(p_yp_1_1,	p_yp_2_1_0);
	stitch(p_yp_1_1,	p_yp_1_2);

	stitch(p_yp_2_1_0,	p_yp_2_1_1);
	stitch(p_yp_2_1_0,	p_yp_2_2);

	stitch(p_yp_2_1_1,	p_yp_2_1_2);

	stitch(p_yp_2_1_2,	p_yp_2_2);

	stitch(p_yp_0_2,	p_yp_1_2);

	stitch(p_yp_1_2,	p_yp_2_2);

	// ym;
	stitch(p_ym_0_0,	p_ym_0_1);
	stitch(p_ym_0_0,	p_ym_1_0);

	stitch(p_ym_1_0,	p_ym_2_0);

	stitch(p_ym_2_0,	p_ym_2_1);

	stitch(p_ym_0_1,	p_ym_0_2);

	stitch(p_ym_2_1,	p_ym_2_2);

	stitch(p_ym_0_2,	p_ym_1_2);

	stitch(p_ym_1_2,	p_ym_2_2);



	// solve;

	//solve_with_source(prob);
	solve_temp(prob);

};


