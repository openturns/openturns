#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

for diamond in [False, True]:
    mesher1D = ot.IntervalMesher([5])
    print("mesher1D=", mesher1D)
    mesh1D = mesher1D.build(ot.Interval(-1.0, 2.0), diamond)
    print("mesh1D=", mesh1D)
    ott.assert_almost_equal(mesh1D.getVolume(), 3.0)

    mesher2D = ot.IntervalMesher([5, 5])
    print("mesher2D=", mesher2D)
    mesh2D = mesher2D.build(ot.Interval([-1.0, -1.0], [2.0, 2.0]), diamond)
    print("mesh2D=", mesh2D)
    ott.assert_almost_equal(mesh2D.getVolume(), 9.0)

    mesher3D = ot.IntervalMesher([5] * 3)
    print("mesher3D=", mesher3D)
    mesh3D = mesher3D.build(ot.Interval(3), diamond)
    print("mesh3D=", mesh3D)
    ott.assert_almost_equal(mesh3D.getVolume(), 1.0)
    volume = mesh3D.computeSimplicesVolume()
    if diamond:
        assert mesh3D.getVerticesNumber() == 791
        nv = 3000
        assert mesh3D.getSimplicesNumber() == nv
        ott.assert_almost_equal(volume, [1.0 / nv] * nv)

    mesher4D = ot.IntervalMesher([2] * 4)
    print("mesher4D=", mesher4D)
    try:
        mesh4D = mesher4D.build(ot.Interval(4), diamond)
        print("mesh4D=", mesh4D)
    except RuntimeError:
        print("notyetimpl")
    ott.assert_almost_equal(mesh4D.getVolume(), 1.0)

    mesher5D = ot.IntervalMesher([2] * 5)
    print("mesher5D=", mesher5D)
    try:
        mesh5D = mesher5D.build(ot.Interval(5), diamond)
        print("mesh5D=", mesh5D)
    except RuntimeError:
        print("notyetimpl")
    ott.assert_almost_equal(mesh5D.getVolume(), 1.0)
