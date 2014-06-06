
#include <Diff2D/boundary.hpp>
#include <Diff2D/init.hpp>
#include <Diff2D/prob.hpp>
#include <Diff2D/patch_group.hpp>
#include <Diff2D/stitch.hpp>
#include <Diff2D/log.hpp>

real r_source = 1e-3;
real r_inner = 1e-6;
real r_outer = 1e-3;

void solve_source(std::shared_ptr<Prob> prob) {
	//prob->solve2(1e-4, 1e-2, True);
	
	prob->solve("s", r_source, 0, 0);

	prob->value_clamp_per_group("s", 0.0, 1.0);

	prob->write_binary("s");
	//prob->write("s");

}
void solve_temp(std::shared_ptr<Prob> prob) {
	//prob->solve2(1e-4, 1e-2, True);
	
	prob->solve("T", r_inner, 0, 0);

	prob->write_binary("T");
	prob->write("T");

}
void solve_with_source(std::shared_ptr<Prob> prob) {
	//prob->solve2(1e-4, 1e-2, True);

	solve_source(prob);


	prob->copy_value_to_source("s","T");
	
	prob->solve2("T", r_inner, r_outer);

	prob->write_binary("T");
	prob->write("T");

}

int main(int ac, char** av) {

	// thomas's data
	size_t run_index = 0;
	real T_in_pipe[]  = {152.9, 203.6, 415.1};
	real T_in_head[]  = {203.5, 291.8, 487.8};
	real T_in_flux[]  = {227.7, 315.7, 504.7};
	real T_out_flux[] = {244.6, 346.9, 522.5};
	real T_out_head[] = {250.8, 355.7, 538.0};
	real T_out_pipe[] = {229.3, 332.6, 512.8};
	
	const char* prob_names[] = {"opt2_run1","opt2_run2","opt2_run3"};
	
	// convert to kelvin
	for(size_t i = 0; i < 1; ++i) {
		T_in_pipe[i]  += 273.15;
		T_in_head[i]  += 273.15;
		T_in_flux[i]  += 273.15;
		T_out_flux[i] += 273.15;
		T_out_head[i] += 273.15;
		T_out_pipe[i] += 273.15;
	}

	// solve

	// dimensions
	real w_total = 	6.00e-2;
	real w_irrad = 	2.00e-2;
	real pipe = 	6.35e-3;

	real l_total = 	4.00e-2;
	real l_irrad = 	2.00e-2;

	// calculations
	real l_1 = (l_total - l_irrad) / 2.0;
	real l_3 = l_irrad - pipe * 2.0;

	real w_ends = (w_total - w_irrad) / 2.0;
	real w_1 = (w_ends - pipe) / 2.0;


	// lists
	auto xd = make_array_1<real,1>({0, w_1, pipe, w_1, w_irrad, w_1, pipe, w_1});

	auto yd = make_array_1<real,1>({0, 1e-2, 5e-2});

	auto zd = make_array_1<real,1>({0, l_1, pipe, l_3*0.5, l_3*0.5, pipe, l_1});

	real nom_size = 5e-4;

	auto nx = xd->sub({1},{-1})->divide(nom_size)->ceil<size_t>();
	auto ny = yd->sub({1},{-1})->divide(nom_size)->ceil<size_t>();
	auto nz = zd->sub({1},{-1})->divide(nom_size)->ceil<size_t>();

	auto x = xd->cumsum();
	auto y = yd->cumsum();
	auto z = zd->cumsum();

	coor_type X({x,y,z});

	cell_count_type n({nx,ny,nz});

	//=================================================================

	real s = 1.0e10;

	// irradiated edge temperatures
	patch_v_bou_edge_vec_type T_irr_xm({
			std::make_shared<boundary_array>(make_ones<real,1>({nz->get(1)})->multiply_self(T_in_flux[run_index])),
			std::make_shared<boundary_array>(make_ones<real,1>({nz->get(2)})->multiply_self(T_in_flux[run_index])),
			std::make_shared<boundary_array>(make_ones<real,1>({nz->get(3)})->multiply_self(T_in_flux[run_index])),
			std::make_shared<boundary_array>(make_ones<real,1>({nz->get(3)})->multiply_self(T_in_flux[run_index]))
			});

	patch_v_bou_edge_vec_type T_irr_xp({
			std::make_shared<boundary_array>(make_ones<real,1>({nz->get(1)})->multiply_self(T_out_flux[run_index])),
			std::make_shared<boundary_array>(make_ones<real,1>({nz->get(1)})->multiply_self(T_out_flux[run_index])),
			std::make_shared<boundary_array>(make_ones<real,1>({nz->get(2)})->multiply_self(T_out_flux[run_index])),
			std::make_shared<boundary_array>(make_ones<real,1>({nz->get(3)})->multiply_self(T_out_flux[run_index]))
			});

	patch_v_bou_edge_vec_type T_irr_zm({
			std::make_shared<boundary_array>(linspace(T_out_flux[run_index], T_in_flux[run_index], nx->get(3)))
			});

	patch_v_bou_edge_vec_type T_irr_zp({
			std::make_shared<boundary_array>(linspace(T_out_flux[run_index], T_in_flux[run_index], nx->get(3)))
			});

	//nx = [10, 10, 10, 50, 10, 10, 10];
	//ny = [50, 50];
	//nz = [10, 10, 30, 10, 10];

	// the default boundary for solve source is const = 1.0
	auto const_bou(std::make_shared<boundary_single>(1.0));

	auto T_bou_i(std::make_shared<boundary_single>(T_in_pipe[run_index]));
	auto T_bou_o(std::make_shared<boundary_single>(T_out_pipe[run_index]));

	patch_v_bou_vec_type v_bou_s_def({
			{{const_bou},{const_bou}},
			{{const_bou},{const_bou}}
			});


	patch_v_bou_vec_type v_bou_T_irr_xm({
			{T_irr_xm,{}},
			{{},{}}
			});
	patch_v_bou_vec_type v_bou_T_irr_xp({
			{{},T_irr_xp},
			{{},{}}
			});	
	patch_v_bou_vec_type v_bou_T_irr_zm({
			{{},{}},
			{T_irr_zm,{}}

			});	
	patch_v_bou_vec_type v_bou_T_irr_zp({
			{{},{}},
			{{},T_irr_zp}
			});


	patch_v_bou_vec_type v_bou_T_i({
			{{T_bou_i},{T_bou_i}},
			{{T_bou_i},{T_bou_i}}
			});

	patch_v_bou_vec_type v_bou_T_o({
			{{T_bou_o},{T_bou_o}},
			{{T_bou_o},{T_bou_o}}
			});

	patch_v_bou_type v_bou_def({{"s",v_bou_s_def}});

	patch_v_bou_type v_bou_irr_xm({{"s",v_bou_s_def},{"T",v_bou_T_irr_xm}});
	patch_v_bou_type v_bou_irr_xp({{"s",v_bou_s_def},{"T",v_bou_T_irr_xp}});
	patch_v_bou_type v_bou_irr_zm({{"s",v_bou_s_def},{"T",v_bou_T_irr_zm}});
	patch_v_bou_type v_bou_irr_zp({{"s",v_bou_s_def},{"T",v_bou_T_irr_zp}});


	patch_v_bou_type v_bou_i({{"s",v_bou_s_def},{"T",v_bou_T_i}});
	patch_v_bou_type v_bou_o({{"s",v_bou_s_def},{"T",v_bou_T_o}});


	// create patch groups;

	point pt_xz(
			x->get(0),
			(y->get(0) + y->get(1)) / 2.0,
			(z->get(2) + z->get(3)) / 2.0);

	point pt_f_in(
			(x->get(1) + x->get(2)) / 2.0,
			0.0,
			z->get(3));
	point pt_f_out(
			(x->get(5) + x->get(6)) / 2.0,
			0.0,
			z->get(3));

	point pt_h_in(
			(x->get(3) + x->get(4)) / 2.0,
			y->get(1),
			(z->get(1) + z->get(2)) / 2.0);

	point pt_h_out(
			(x->get(3) + x->get(4)) / 2.0,
			y->get(1),
			(z->get(4) + z->get(5)) / 2.0);

	point pt_in(
			x->get(2),
			(y->get(1) + y->get(2)) / 2.0,
			(z->get(1) + z->get(2)) / 2.0);

	point pt_out(
			x->get(5),
			(y->get(1) + y->get(2)) / 2.0,
			(z->get(4) + z->get(5)) / 2.0);

	//==============================================================

	auto prob = std::make_shared<Prob>(prob_names[run_index], X, n, 1E5, 1E5);

	prob->create_equation("T", 10.0, 1.5, 1.5);
	prob->create_equation("s", 10.0, 1.5, 1.5);

	auto g_xyz		= prob->create_patch_group("xyz",		{{"T",0.0},			{"s",2.0}}, {{"T",0.0},{"s",s}}, pt_xz);
	auto g_f_in		= prob->create_patch_group("f_in",		{{"T",T_in_flux[run_index]},	{"s",2.0}}, {{"T",0.0},{"s",s}}, pt_f_in);
	auto g_f_out		= prob->create_patch_group("f_out",		{{"T",T_out_flux[run_index]},	{"s",2.0}}, {{"T",0.0},{"s",s}}, pt_f_out);
	auto g_h_in		= prob->create_patch_group("h_in",		{{"T",T_in_head[run_index]},	{"s",2.0}}, {{"T",0.0},{"s",s}}, pt_h_in);
	auto g_h_out		= prob->create_patch_group("h_out",		{{"T",T_out_head[run_index]},	{"s",2.0}}, {{"T",0.0},{"s",s}}, pt_h_out);
	auto g_in		= prob->create_patch_group("in",		{{"T",T_in_pipe[run_index]},	{"s",2.0}}, {{"T",0.0},{"s",s}}, pt_in);
	auto g_out		= prob->create_patch_group("out",		{{"T",T_out_pipe[run_index]},	{"s",2.0}}, {{"T",0.0},{"s",s}}, pt_out);

	// create patches

	auto p_in_xm	= g_in->create_patch("p_in_xm",		-1,	{1},		{1,2},	{1,2},		v_bou_i);
	auto p_in_xp	= g_in->create_patch("p_in_xp",		1,	{2},		{1,2},	{1,2},		v_bou_i);
	auto p_in_zm	= g_in->create_patch("p_in_zm",		-3,	{1,2},		{1,2},	{1},		v_bou_i);
	auto p_in_zp	= g_in->create_patch("p_in_zp",		3,	{1,2},		{1,2},	{2},		v_bou_i);

	auto p_out_xm	= g_out->create_patch("p_out_xm",	-1,	{5},		{1,2},	{4,5},		v_bou_o);
	auto p_out_xp	= g_out->create_patch("p_out_xp",	1,	{6},		{1,2},	{4,5},		v_bou_o);
	auto p_out_zm	= g_out->create_patch("p_out_zm",	-3,	{5,6},		{1,2},	{4},		v_bou_o);
	auto p_out_zp	= g_out->create_patch("p_out_zp",	3,	{5,6},		{1,2},	{5},		v_bou_o);


	// ym
	auto p_ym_0_0	= g_f_in->create_patch("p_ym_0_0",	-2,	{0,1,2,3},	{0},	{0,1},		v_bou_def);
	auto p_ym_0_1	= g_f_in->create_patch("p_ym_0_1",	-2,	{0,1,2,3},	{0},	{1,2,3,4,5},	v_bou_irr_xm);// = {"T":{{T_irr_xm,0.0},{0.0,0.0}},"s":{{1.0,1.0},{1.0,1.0}}});
	auto p_ym_0_2	= g_f_in->create_patch("p_ym_0_2",	-2,	{0,1,2,3},	{0},	{5,6},		v_bou_def);// = {"T":{{0.0,0.0},{0.0,0.0}},	"s":{{1.0,1.0},{1.0,1.0}}});

	auto p_ym_1_0	= g_xyz->create_patch("p_ym_1_0",	-2,	{3,4},		{0},	{0,1},		v_bou_irr_zm);// = {"T":{{0.0,0.0},{T_irr_zm,0.0}},"s":{{1.0,1.0},{1.0,1.0}}});
	auto p_ym_1_2	= g_xyz->create_patch("p_ym_1_2",	-2,	{3,4},		{0},	{5,6},		v_bou_irr_zp);// = {"T":{{0.0,0.0},{0.0,T_irr_zp}},"s":{{1.0,1.0},{1.0,1.0}}});

	auto p_ym_2_0	= g_f_out->create_patch("p_ym_2_0",	-2,	{4,5,6,7},	{0},	{0,1},		v_bou_def);// = {"T":{{0.0,0.0},{0.0,0.0}},	"s":{{1.0,1.0},{1.0,1.0}}});
	auto p_ym_2_1	= g_f_out->create_patch("p_ym_2_1",	-2,	{4,5,6,7},	{0},	{1,2,3,4,5},	v_bou_irr_xp);// = {"T":{{0.0,T_irr_xp},{0.0,0.0}},"s":{{1.0,1.0},{1.0,1.0}}});
	auto p_ym_2_2	= g_f_out->create_patch("p_ym_2_2",	-2,	{4,5,6,7},	{0},	{5,6},		v_bou_def);// = {"T":{{0.0,0.0},{0.0,0.0}},	"s":{{1.0,1.0},{1.0,1.0}}});

	// yp
	auto p_yp_0_0	= g_in->create_patch("p_yp_0_0",	2,	{0,1,2,3},	{1},	{0,1},		v_bou_def);// = {"T":{{0.0,0.0},{0.0,0.0}},"s":{{1.0,1.0},{1.0,1.0}}});

	auto p_yp_0_1_0	= g_in->create_patch("p_yp_0_1_0",	2,	{0,1},		{1},	{1,2,3},	v_bou_def);// = {"T":{{0.0,0.0},{0.0,0.0}},"s":{{1.0,1.0},{1.0,1.0}}});
	auto p_yp_0_1_1	= g_in->create_patch("p_yp_0_1_1",	2,	{1,2},		{1},	{2,3},		v_bou_def);// = {"T":{{0.0,0.0},{0.0,0.0}},"s":{{1.0,1.0},{1.0,1.0}}});
	auto p_yp_0_1_2	= g_in->create_patch("p_yp_0_1_2",	2,	{2,3},		{1},	{1,2,3},	v_bou_def);// = {"T":{{0.0,0.0},{0.0,0.0}},"s":{{1.0,1.0},{1.0,1.0}}});

	auto p_yp_0_2	= g_h_out->create_patch("p_yp_0_2",	2,	{0,1,2,3},	{1},	{3,4,5},	v_bou_def);// = {"T":{{0.0,0.0},{0.0,0.0}},"s":{{1.0,1.0},{1.0,1.0}}});
	auto p_yp_0_3	= g_h_out->create_patch("p_yp_0_3",	2,	{0,1,2,3},	{1},	{5,6},		v_bou_def);// = {"T":{{0.0,0.0},{0.0,0.0}},"s":{{1.0,1.0},{1.0,1.0}}});

	auto p_yp_1_0	= g_h_in->create_patch("p_yp_1_0",	2,	{3,4},		{1},	{0,1},		v_bou_def);// = {"T":{{0.0,0.0},{0.0,0.0}},"s":{{1.0,1.0},{1.0,1.0}}});
	auto p_yp_1_1	= g_h_in->create_patch("p_yp_1_1",	2,	{3,4},		{1},	{1,2,3},	v_bou_def);// = {"T":{{0.0,0.0},{0.0,0.0}},"s":{{1.0,1.0},{1.0,1.0}}});
	auto p_yp_1_2	= g_h_out->create_patch("p_yp_1_2",	2,	{3,4},		{1},	{3,4,5},	v_bou_def);// = {"T":{{0.0,0.0},{0.0,0.0}},"s":{{1.0,1.0},{1.0,1.0}}});
	auto p_yp_1_3	= g_h_out->create_patch("p_yp_1_3",	2,	{3,4},		{1},	{5,6},		v_bou_def);// = {"T":{{0.0,0.0},{0.0,0.0}},"s":{{1.0,1.0},{1.0,1.0}}});

	auto p_yp_2_0	= g_h_in->create_patch("p_yp_2_0",	2,	{4,5,6,7},	{1},	{0,1},		v_bou_def);// = {"T":{{0.0,0.0},{0.0,0.0}},"s":{{1.0,1.0},{1.0,1.0}}});
	auto p_yp_2_1	= g_h_in->create_patch("p_yp_2_1",	2,	{4,5,6,7},	{1},	{1,2,3},	v_bou_def);// = {"T":{{0.0,0.0},{0.0,0.0}},"s":{{1.0,1.0},{1.0,1.0}}});

	auto p_yp_2_2_0	= g_out->create_patch("p_yp_2_2_0",	2,	{4,5},		{1},	{3,4,5},	v_bou_def);// = {"T":{{0.0,0.0},{0.0,0.0}},"s":{{1.0,1.0},{1.0,1.0}}});
	auto p_yp_2_2_1	= g_out->create_patch("p_yp_2_2_1",	2,	{5,6},		{1},	{3,4},		v_bou_def);// = {"T":{{0.0,0.0},{0.0,0.0}},"s":{{1.0,1.0},{1.0,1.0}}});
	auto p_yp_2_2_2	= g_out->create_patch("p_yp_2_2_2",	2,	{6,7},		{1},	{3,4,5},	v_bou_def);// = {"T":{{0.0,0.0},{0.0,0.0}},"s":{{1.0,1.0},{1.0,1.0}}});

	auto p_yp_2_3	= g_out->create_patch("p_yp_2_3",	2,	{4,5,6,7},	{1},	{5,6},		v_bou_def);// = {"T":{{0.0,0.0},{0.0,0.0}},"s":{{1.0,1.0},{1.0,1.0}}});

	// xp
	auto p_xm	= g_xyz->create_patch("p_xm",		-1,	{0},			{0,1},		{0,1,2,3,4,5,6},	v_bou_def);// = {"T":{{0.0,0.0},{0.0,0.0}},"s":{{1.0,1.0},{1.0,1.0}}});
	auto p_xp	= g_xyz->create_patch("p_xp",		1,	{7},			{0,1},		{0,1,2,3,4,5,6},	v_bou_def);// = {"T":{{0.0,0.0},{0.0,0.0}},"s":{{1.0,1.0},{1.0,1.0}}});

	auto p_zm	= g_xyz->create_patch("p_zm",		-3,	{0,1,2,3,4,5,6,7},	{0,1},		{0},			v_bou_def);// = {"T":{{0.0,0.0},{0.0,0.0}},"s":{{1.0,1.0},{1.0,1.0}}});
	auto p_zp	= g_xyz->create_patch("p_zp",		3,	{0,1,2,3,4,5,6,7},	{0,1},		{6},			v_bou_def);// = {"T":{{0.0,0.0},{0.0,0.0}},"s":{{1.0,1.0},{1.0,1.0}}});


	// stitching

	// in xm
	stitch(p_in_xm,		p_in_zm);
	stitch(p_in_xm,		p_in_zp);
	stitch(p_in_xm,		p_yp_0_1_0);

	// in xp
	stitch(p_in_xp,		p_in_zm);
	stitch(p_in_xp,		p_in_zp);
	stitch(p_in_xp,		p_yp_0_1_2);

	// in z
	stitch(p_in_zm,		p_yp_0_0);
	stitch(p_in_zp,		p_yp_0_1_1);

	// out xm
	stitch(p_out_xm,	p_out_zm);
	stitch(p_out_xm,	p_out_zp);
	stitch(p_out_xm,	p_yp_2_2_0);

	// out xp
	stitch(p_out_xp,	p_out_zm);
	stitch(p_out_xp,	p_out_zp);
	stitch(p_out_xp,	p_yp_2_2_2);

	// out z
	stitch(p_out_zp,	p_yp_2_3);
	stitch(p_out_zm,	p_yp_2_2_1);



	// xm
	stitch(p_xm,p_ym_0_0);
	stitch(p_xm,p_ym_0_1);
	stitch(p_xm,p_ym_0_2);

	stitch(p_xm,p_yp_0_0);
	stitch(p_xm,p_yp_0_1_0);
	stitch(p_xm,p_yp_0_2);
	stitch(p_xm,p_yp_0_3);

	stitch(p_xm,p_zm);
	stitch(p_xm,p_zp);

	// xp
	stitch(p_xp,p_ym_2_0);
	stitch(p_xp,p_ym_2_1);;
	stitch(p_xp,p_ym_2_2);

	stitch(p_xp,p_yp_2_0);
	stitch(p_xp,p_yp_2_1);
	stitch(p_xp,p_yp_2_2_2);
	stitch(p_xp,p_yp_2_3);

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

	stitch(p_zp,p_yp_0_3);
	stitch(p_zp,p_yp_1_3);
	stitch(p_zp,p_yp_2_3);

	// yp
	// row 0
	stitch(p_yp_0_0,	p_yp_1_0);
	stitch(p_yp_0_0,	p_yp_0_1_0);
	stitch(p_yp_0_0,	p_yp_0_1_2);

	stitch(p_yp_1_0,	p_yp_1_1);
	stitch(p_yp_1_0,	p_yp_2_0);

	stitch(p_yp_2_0,	p_yp_2_1);

	// row 1
	stitch(p_yp_0_1_0,	p_yp_0_1_1);
	stitch(p_yp_0_1_0,	p_yp_0_2);

	stitch(p_yp_0_1_1,	p_yp_0_1_2);
	stitch(p_yp_0_1_1,	p_yp_0_2);

	stitch(p_yp_0_1_2,	p_yp_1_1);
	stitch(p_yp_0_1_2,	p_yp_0_2);

	stitch(p_yp_1_1,	p_yp_2_1);
	stitch(p_yp_1_1,	p_yp_1_2);

	stitch(p_yp_2_1,	p_yp_2_2_0);
	stitch(p_yp_2_1,	p_yp_2_2_1);
	stitch(p_yp_2_1,	p_yp_2_2_2);


	// row 2
	stitch(p_yp_0_2,	p_yp_1_2);
	stitch(p_yp_0_2,	p_yp_0_3);

	stitch(p_yp_1_2,	p_yp_2_2_0);
	stitch(p_yp_1_2,	p_yp_1_3);

	stitch(p_yp_2_2_0,	p_yp_2_2_1);
	stitch(p_yp_2_2_0,	p_yp_2_3);

	stitch(p_yp_2_2_1,	p_yp_2_2_2);

	stitch(p_yp_2_2_2,	p_yp_2_3);

	// row 3
	stitch(p_yp_0_3,	p_yp_1_3);

	stitch(p_yp_1_3,	p_yp_2_3);

	// ym;
	stitch(p_ym_0_0,	p_ym_0_1);
	stitch(p_ym_0_0,	p_ym_1_0);

	stitch(p_ym_1_0,	p_ym_2_0);

	stitch(p_ym_2_0,	p_ym_2_1);

	stitch(p_ym_0_1,	p_ym_0_2);

	stitch(p_ym_2_1,	p_ym_2_2);

	stitch(p_ym_0_2,	p_ym_1_2);

	stitch(p_ym_1_2,	p_ym_2_2);


	for(auto g : prob->patch_groups_) {
		g->get_value_of_interest_residual("T");
	}	

	// solve;

	prob->connection_info();

	//solve_with_source(prob);
	//solve_source(prob);
	solve_temp(prob);

	prob->value_stats("T");

};


