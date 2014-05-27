/*
import profile
import pylab as pl

//from solver import *
import solver.prob
from solver.patch import stitch
*/

int main() {

	n = 10;

	x = [[0.0, 1.0, 2.0, 3.0], [0.0, 1.0, 2.0, 3.0], [0.0, 1.0, 2.0, 3.0]];
	n = [[n+0]*2,[n+2]*2,[n+4]*2];

	prob = solver.prob.Problem('test4', x, n, it_max_1 = 1000, it_max_2 = 1000);

	prob.create_equation('T', 10.0, 1.5, 1.5);
	prob.create_equation('s', 10.0, 1.5, 1.5);

	//prob.get_3d_axes()
	//sys.exit(0)

	p0 = None;
	p1 = None;
	p2 = None;
	p3 = None;
	p4 = None;
	p5 = None;

	g2 = prob.create_patch_group('2', v_0 = {'T':10.0,'s':2.0}, S = {'T':0.0,'s':10.0});
	g3 = prob.create_patch_group('3', v_0 = {'T': 0.0,'s':2.0}, S = {'T':0.0,'s':10.0});
	g4 = prob.create_patch_group('4', v_0 = {'T': 0.0,'s':2.0}, S = {'T':0.0,'s':10.0});
	g5 = prob.create_patch_group('5', v_0 = {'T': 0.0,'s':2.0}, S = {'T':0.0,'s':10.0});

	//p0 = prob.createPatch(1,	[1,	[0,1],	[0,1]])
	//p1 = prob.createPatch(2,	[[0,1],	1,	[0,1]])
	p2 = g2.create_patch('2',3,	[[0,1,2],	[0,1,2],	2],		v_bou = {'T':[[30.0,30.0],[30.0,30.0]],'s':[[1.0,1.0],[1.0,1.0]]});
	p3 = g3.create_patch('3',-1,	[0,		[2,1,0],	[2,1,0]],	v_bou = {'T':[[30.0,30.0],[30.0,30.0]],'s':[[1.0,1.0],[1.0,1.0]]});
	p4 = g4.create_patch('4',-2,	[[2,1,0],	0,		[2,1,0]],	v_bou = {'T':[[30.0,30.0],[30.0,30.0]],'s':[[1.0,1.0],[1.0,1.0]]});
	p5 = g5.create_patch('5',-3,	[[2,1,0],	[2,1,0],	0],		v_bou = {'T':[[30.0,30.0],[30.0,30.0]],'s':[[1.0,1.0],[1.0,1.0]]});


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



	//f0 = p0.faces[0,0]
	//f1 = p1.faces[0,0]
	f2 = p2.faces[0,0];
	f3 = p3.faces[0,0];
	f4 = p4.faces[0,0];
	f5 = p5.faces[0,0];



	//f0.create_equ('T', 0., [[30.,0.],[0.,0.]], k, al)

	//f1.create_equ('T', 0., [[30.,0.],[0.,0.]], k, al)

	f2.equs['T'].v_bou = [[30.,30.],[30.,30.]];
	f3.equs['T'].v_bou = [[30.,30.],[30.,30.]];
	f4.equs['T'].v_bou = [[30.,30.],[30.,30.]];
	f5.equs['T'].v_bou = [[10.,10.],[10.,10.]];


	//prob.solve2(1e-2, 1e-4, True)

	//profile.run("prob.solve('s', 1e-1)")
	prob.solve('s', 1e-3, True);


	prob.value_add('s',-1.0);
	prob.value_normalize('s');

	//prob.copy_value_to_source('s','T')

	//prob.solve('T', 1e-1, True)
	prob.solve2('T', 1e-2, 1e-2, True);

	//prob.plot3()

	//prob.plot('s')
	prob.plot('T');

	//pl.show()

	prob.write('T');


}


