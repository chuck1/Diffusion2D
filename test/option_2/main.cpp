
#include <Diff2D/init.hpp>
#include <Diff2D/prob.hpp>
#include <Diff2D/patch_group.hpp>
#include <Diff2D/stitch.hpp>
#include <Diff2D/log.hpp>

void solve_with_source() {
	//prob->solve2(1e-4, 1e-2, True);

	prob->solve("s", 1e-2, True);

	prob->value_add("s", -1.0);
	prob->value_normalize("s");
	prob->copy_value_to_source("s","T");

	prob->solve2("T", 1e-2, 1e-2, 1e-3, True);
}

int main(int ac, char** av) {
	// program
	d2d::log::init();

	d2d::log::core = d2d::log::sl::debug;

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

	auto xd = make_array_1([w_1, pipe, w_1, w_irrad, w_1, pipe, w_1]);

	auto yd = make_array_1np.array([1e-2, 5e-2]);

	auto zd = make_array_1np.array([l_1, pipe, l_3, pipe, l_1]);



	real nom_size = 5e-4;

	size_t nx = np.ceil(xd / nom_size);
	size_t ny = np.ceil(yd / nom_size);
	size_t nz = np.ceil(zd / nom_size);


	x = np.cumsum(np.append([0],xd));
	y = np.cumsum(np.append([0],yd));
	z = np.cumsum(np.append([0],zd));
	
	
	// irradiated edge temperatures
	T_irr_xm = [
		np.ones(nz[1]) * 10.0,
		np.ones(nz[2]) * 20.0,
		np.ones(nz[3]) * 30.0];
	T_irr_xp = [
		np.ones(nz[1]) * 10.0,
		np.ones(nz[2]) * 20.0,
		np.ones(nz[3]) * 30.0];
	T_irr_zm = [np.ones(nx[3]) * 10.0];
	T_irr_zp = [np.ones(nx[3]) * 30.0];

	//nx = [10, 10, 10, 50, 10, 10, 10];
	//ny = [50, 50];
	//nz = [10, 10, 30, 10, 10];

	print nx;
	print ny;
	print nz;

	X = [x,y,z];
	n = [nx,ny,nz];

	prob = d2d.prob->Problem('opt2', X, n, it_max_1 = 1000, it_max_2 = 1000);

	prob->create_equation("T", 10.0, 1.5, 1.5);
	prob->create_equation("s", 10.0, 1.5, 1.5);

	s = 1.0e10;

	// create patch groups;

	pt_xm = [
		0,
		(y[0] + y[1]) / 2.0,
		(z[2] + z[3]) / 2.0];

	pt_xp = [
		x[-1],
		(y[0] + y[1]) / 2.0,
		(z[2] + z[3]) / 2.0];

	pt_ym = [
		(x[1] + x[2]) / 2.0,
		0,
		(z[2] + z[3]) / 2.0];

	pt_yp = [
		(x[1] + x[2]) / 2.0,
		y[1],
		(z[2] + z[3]) / 2.0];

	pt_zm = [
		(x[3] + x[4]) / 2.0,
		(y[0] + y[1]) / 2.0,
		0]

			pt_zp = [
			(x[3] + x[4]) / 2.0,
		(y[0] + y[1]) / 2.0,
		z[5]];


	g_xm		= prob->create_patch_group('xm',		v_0 = {"T":100.0,"s":2.0}, S = {"T":0.0,"s":s}, v_0_point = pt_xm);
	g_xp		= prob->create_patch_group('xp',		v_0 = {"T":000.0,"s":2.0}, S = {"T":0.0,"s":s}, v_0_point = pt_xp);
	g_ym		= prob->create_patch_group('ym',		v_0 = {"T":000.0,"s":2.0}, S = {"T":0.0,"s":s}, v_0_point = pt_ym);
	g_yp		= prob->create_patch_group('yp',		v_0 = {"T":000.0,"s":2.0}, S = {"T":0.0,"s":s}, v_0_point = pt_yp);
	g_zm		= prob->create_patch_group('zm',		v_0 = {"T":000.0,"s":2.0}, S = {"T":0.0,"s":s}, v_0_point = pt_zm);
	g_zp		= prob->create_patch_group('zp',		v_0 = {"T":000.0,"s":2.0}, S = {"T":0.0,"s":s}, v_0_point = pt_zp);


	// create patches
	// ym
	p_ym_0_0	= g_ym->create_patch('p_ym_0_0',	-2,[[0,1,2,3],	0,	[0,1]],		v_bou = {"T":[[0.0,0.0],[0.0,0.0]],	"s":[[1.0,1.0],[1.0,1.0]]});
	p_ym_0_1	= g_ym->create_patch('p_ym_0_1',	-2,[[0,1,2,3],	0,	[1,2,3,4]],	v_bou = {"T":[[T_irr_xm,0.0],[0.0,0.0]],"s":[[1.0,1.0],[1.0,1.0]]});
	p_ym_0_2	= g_ym->create_patch('p_ym_0_2',	-2,[[0,1,2,3],	0,	[4,5]],		v_bou = {"T":[[0.0,0.0],[0.0,0.0]],	"s":[[1.0,1.0],[1.0,1.0]]});
	p_ym_1_0	= g_ym->create_patch('p_ym_1_0',	-2,[[3,4],	0,	[0,1]],		v_bou = {"T":[[0.0,0.0],[T_irr_zm,0.0]],"s":[[1.0,1.0],[1.0,1.0]]});
	p_ym_1_2	= g_ym->create_patch('p_ym_1_2',	-2,[[3,4],	0,	[4,5]],		v_bou = {"T":[[0.0,0.0],[0.0,T_irr_zp]],"s":[[1.0,1.0],[1.0,1.0]]});
	p_ym_2_0	= g_ym->create_patch('p_ym_2_0',	-2,[[4,5,6,7],	0,	[0,1]],		v_bou = {"T":[[0.0,0.0],[0.0,0.0]],	"s":[[1.0,1.0],[1.0,1.0]]});
	p_ym_2_1	= g_ym->create_patch('p_ym_2_1',	-2,[[4,5,6,7],	0,	[1,2,3,4]],	v_bou = {"T":[[0.0,T_irr_xp],[0.0,0.0]],"s":[[1.0,1.0],[1.0,1.0]]});
	p_ym_2_2	= g_ym->create_patch('p_ym_2_2',	-2,[[4,5,6,7],	0,	[4,5]],		v_bou = {"T":[[0.0,0.0],[0.0,0.0]],	"s":[[1.0,1.0],[1.0,1.0]]});

	// yp
	p_yp_0_0	= g_yp->create_patch('p_yp_0_0',	2,[[0,1,2,3],	1,	[0,1]],		v_bou = {"T":[[0.0,0.0],[0.0,0.0]],"s":[[1.0,1.0],[1.0,1.0]]});

	p_yp_0_1_0	= g_yp->create_patch('p_yp_0_1',	2,[[0,1],	1,	[1,2,3,4]],	v_bou = {"T":[[0.0,0.0],[0.0,0.0]],"s":[[1.0,1.0],[1.0,1.0]]});
	p_yp_0_1_1	= g_yp->create_patch('p_yp_0_1',	2,[[1,2],	1,	[2,3,4]],	v_bou = {"T":[[0.0,0.0],[0.0,0.0]],"s":[[1.0,1.0],[1.0,1.0]]});
	p_yp_0_1_2	= g_yp->create_patch('p_yp_0_1',	2,[[2,3],	1,	[1,2,3,4]],	v_bou = {"T":[[0.0,0.0],[0.0,0.0]],"s":[[1.0,1.0],[1.0,1.0]]});

	p_yp_0_2	= g_yp->create_patch('p_yp_0_2',	2,[[0,1,2,3],	1,	[4,5]],		v_bou = {"T":[[0.0,0.0],[0.0,0.0]],"s":[[1.0,1.0],[1.0,1.0]]});
	p_yp_1_0	= g_yp->create_patch('p_yp_1_0',	2,[[3,4],	1,	[0,1]],		v_bou = {"T":[[0.0,0.0],[0.0,0.0]],"s":[[1.0,1.0],[1.0,1.0]]});
	p_yp_1_1	= g_yp->create_patch('p_yp_1_1',	2,[[3,4],	1,	[1,2,3,4]],	v_bou = {"T":[[0.0,0.0],[0.0,0.0]],"s":[[1.0,1.0],[1.0,1.0]]});
	p_yp_1_2	= g_yp->create_patch('p_yp_1_2',	2,[[3,4],	1,	[4,5]],		v_bou = {"T":[[0.0,0.0],[0.0,0.0]],"s":[[1.0,1.0],[1.0,1.0]]});
	p_yp_2_0	= g_yp->create_patch('p_yp_2_0',	2,[[4,5,6,7],	1,	[0,1]],		v_bou = {"T":[[0.0,0.0],[0.0,0.0]],"s":[[1.0,1.0],[1.0,1.0]]});

	p_yp_2_1_0	= g_yp->create_patch('p_yp_2_1',	2,[[4,5],	1,	[1,2,3,4]],	v_bou = {"T":[[0.0,0.0],[0.0,0.0]],"s":[[1.0,1.0],[1.0,1.0]]});
	p_yp_2_1_1	= g_yp->create_patch('p_yp_2_1',	2,[[5,6],	1,	[1,2,3]],	v_bou = {"T":[[0.0,0.0],[0.0,0.0]],"s":[[1.0,1.0],[1.0,1.0]]});
	p_yp_2_1_2	= g_yp->create_patch('p_yp_2_1',	2,[[6,7],	1,	[1,2,3,4]],	v_bou = {"T":[[0.0,0.0],[0.0,0.0]],"s":[[1.0,1.0],[1.0,1.0]]});

	p_yp_2_2	= g_yp->create_patch('p_yp_2_2',	2,[[4,5,6,7],	1,	[4,5]],		v_bou = {"T":[[0.0,0.0],[0.0,0.0]],"s":[[1.0,1.0],[1.0,1.0]]});

	// xp
	p_xm		= g_xm->create_patch('p_xm', -1,	[0,			[0,1],		[0,1,2,3,4,5]],	v_bou = {"T":[[0.0,0.0],[0.0,0.0]],"s":[[1.0,1.0],[1.0,1.0]]});
	p_xp		= g_xp->create_patch('p_xp', 1,	[7,			[0,1],		[0,1,2,3,4,5]],	v_bou = {"T":[[0.0,0.0],[0.0,0.0]],"s":[[1.0,1.0],[1.0,1.0]]});

	p_zm		= g_zm->create_patch('p_zm', -3,	[[7,6,5,4,3,2,1,0],	[1,0],		0],		v_bou = {"T":[[0.0,0.0],[0.0,0.0]],"s":[[1.0,1.0],[1.0,1.0]]});
	p_zp		= g_zp->create_patch('p_zp', 3,	[[0,1,2,3,4,5,6,7],	[0,1],		5],		v_bou = {"T":[[0.0,0.0],[0.0,0.0]],"s":[[1.0,1.0],[1.0,1.0]]});


	// stitching
	// xm
	d2d.patch.stitch(p_xm,p_ym_0_0);
	d2d.patch.stitch(p_xm,p_ym_0_1);
	d2d.patch.stitch(p_xm,p_ym_0_2);

	d2d.patch.stitch(p_xm,p_yp_0_0);
	d2d.patch.stitch(p_xm,p_yp_0_1_0);
	d2d.patch.stitch(p_xm,p_yp_0_2);

	d2d.patch.stitch(p_xm,p_zm);
	d2d.patch.stitch(p_xm,p_zp);

	// xp
	d2d.patch.stitch(p_xp,p_ym_2_0);
	d2d.patch.stitch(p_xp,p_ym_2_1);;
	d2d.patch.stitch(p_xp,p_ym_2_2);

	d2d.patch.stitch(p_xp,p_yp_2_0);
	d2d.patch.stitch(p_xp,p_yp_2_1_2);
	d2d.patch.stitch(p_xp,p_yp_2_2);
	
	d2d.patch.stitch(p_xp,p_zm);
	d2d.patch.stitch(p_xp,p_zp);

	//zm;
	d2d.patch.stitch(p_zm,p_ym_0_0);
	d2d.patch.stitch(p_zm,p_ym_1_0);
	d2d.patch.stitch(p_zm,p_ym_2_0);

	d2d.patch.stitch(p_zm,p_yp_0_0);
	d2d.patch.stitch(p_zm,p_yp_1_0);
	d2d.patch.stitch(p_zm,p_yp_2_0);

	// zp;
	d2d.patch.stitch(p_zp,p_ym_0_2);
	d2d.patch.stitch(p_zp,p_ym_1_2);
	d2d.patch.stitch(p_zp,p_ym_2_2);

	d2d.patch.stitch(p_zp,p_yp_0_2);
	d2d.patch.stitch(p_zp,p_yp_1_2);
	d2d.patch.stitch(p_zp,p_yp_2_2);

	// yp;
	d2d.patch.stitch(p_yp_0_0,	p_yp_1_0);
	d2d.patch.stitch(p_yp_0_0,	p_yp_0_1_0);
	d2d.patch.stitch(p_yp_0_0,	p_yp_0_1_2);

	d2d.patch.stitch(p_yp_1_0,	p_yp_1_1);
	d2d.patch.stitch(p_yp_1_0,	p_yp_2_0);

	d2d.patch.stitch(p_yp_2_0,	p_yp_2_1_0);
	d2d.patch.stitch(p_yp_2_0,	p_yp_2_1_1);
	d2d.patch.stitch(p_yp_2_0,	p_yp_2_1_2);

	d2d.patch.stitch(p_yp_0_1_0,	p_yp_0_1_1);
	d2d.patch.stitch(p_yp_0_1_0,	p_yp_0_2);

	d2d.patch.stitch(p_yp_0_1_1,	p_yp_0_1_2);
	d2d.patch.stitch(p_yp_0_1_1,	p_yp_0_2);

	d2d.patch.stitch(p_yp_0_1_2,	p_yp_1_1);
	d2d.patch.stitch(p_yp_0_1_2,	p_yp_0_2);

	d2d.patch.stitch(p_yp_1_1,	p_yp_2_1_0);
	d2d.patch.stitch(p_yp_1_1,	p_yp_1_2);

	d2d.patch.stitch(p_yp_2_1_0,	p_yp_2_1_1);
	d2d.patch.stitch(p_yp_2_1_0,	p_yp_2_2);

	d2d.patch.stitch(p_yp_2_1_1,	p_yp_2_1_2);

	d2d.patch.stitch(p_yp_2_1_2,	p_yp_2_2);

	d2d.patch.stitch(p_yp_0_2,	p_yp_1_2);

	d2d.patch.stitch(p_yp_1_2,	p_yp_2_2);

	// ym;
	d2d.patch.stitch(p_ym_0_0,	p_ym_0_1);
	d2d.patch.stitch(p_ym_0_0,	p_ym_1_0);

	d2d.patch.stitch(p_ym_1_0,	p_ym_2_0);

	d2d.patch.stitch(p_ym_2_0,	p_ym_2_1);

	d2d.patch.stitch(p_ym_0_1,	p_ym_0_2);

	d2d.patch.stitch(p_ym_2_1,	p_ym_2_2);

	d2d.patch.stitch(p_ym_0_2,	p_ym_1_2);

	d2d.patch.stitch(p_ym_1_2,	p_ym_2_2);



	// solve;

	solve_with_source();

	//prob->solve("T", 1e-3, True);


	prob->plot("T");

	prob->write("T");



};


