
#include <Diff2D/boundary.hpp>
#include <Diff2D/init.hpp>
#include <Diff2D/prob.hpp>
#include <Diff2D/patch_group.hpp>
#include <Diff2D/stitch.hpp>
#include <Diff2D/log.hpp>

real r_source = 1e-3;
real r_inner = 1e-6;
real r_outer = 1e-3;


// x x-distance from center
// a x-distance from center to edge
//
real		solar(real x, real y, real a, real b) {
	real d2 = x*x + y*y;

	real f = a * exp(b * d2);

	return f;
}



int main(int ac, char** av) {

	// data
	
	std::string prob_name = "option_1_solar";

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
	/*
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
	*/

	// the default boundary for solve source is const = 1.0
	auto const_bou(std::make_shared<boundary_single>(1.0));


	patch_v_bou_vec_type v_bou_s_def({
			{{const_bou},{const_bou}},
			{{const_bou},{const_bou}}
			});



	patch_v_bou_type v_bou_def({{"s",v_bou_s_def}});


	// create patch groups;


	point pt(
			(x->get(3) + x->get(4)) / 2.0,
			0.0,
			(x->get(1) + x->get(5)) / 2.0);
	//==============================================================
	
	auto prob = std::make_shared<Prob>(prob_name, X, n, 1E5, 1E5);
	
	prob->create_equation("T", 10.0, 1.5, 1.5);
	prob->create_equation("s", 10.0, 1.5, 1.5);

	auto g = prob->create_patch_group("g", {{"T",0.0}, {"s",2.0}}, {{"T",0.0},{"s",s}}, pt);

	// create patches

	auto p = g->create_patch("p",	-2,	{3,4},		{0},	{1,2,3,4,5},		v_bou_def);
	
	
	// solve;

	prob->connection_info();

	p->info_geom();
	

};


