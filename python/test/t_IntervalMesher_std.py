#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

for diamond in [False, True]:
    mesher1D = ot.IntervalMesher([5])
    print("mesher1D=", mesher1D)
    mesh1D = mesher1D.build(ot.Interval(-1.0, 2.0), diamond)
    print("mesh1D=", mesh1D)

    mesher2D = ot.IntervalMesher([5, 5])
    print("mesher2D=", mesher2D)
    mesh2D = mesher2D.build(ot.Interval([-1.0, -1.0], [2.0, 2.0]), diamond)
    print("mesh2D=", mesh2D)

    mesher3D = ot.IntervalMesher([5]*3)
    print("mesher3D=", mesher3D)
    try:
        mesh3D = mesher3D.build(ot.Interval(3), diamond)
        print("mesh3D=", mesh3D)
    except RuntimeError:
        print('notyetimpl')
