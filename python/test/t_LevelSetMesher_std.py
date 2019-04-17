#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

ot.TESTPREAMBLE()

try:
    ot.ResourceMap.SetAsUnsignedInteger(
        "OptimizationAlgorithm-DefaultMaximumIterationNumber", 1000)
    ot.ResourceMap.SetAsUnsignedInteger(
        "OptimizationAlgorithm-DefaultMaximumEvaluationNumber", 100000)
    ot.ResourceMap.SetAsScalar(
        "OptimizationAlgorithm-DefaultMaximumAbsoluteError", 1.0e-7)
    ot.ResourceMap.SetAsScalar(
        "OptimizationAlgorithm-DefaultMaximumRelativeError", 1.0e-7)
    ot.ResourceMap.SetAsScalar(
        "OptimizationAlgorithm-DefaultMaximumResidualError", 1.0e-7)
    ot.ResourceMap.SetAsScalar(
        "OptimizationAlgorithm-DefaultMaximumConstraintError", 1.0e-7)
    ot.PlatformInfo.SetNumericalPrecision(2)

    # The 1D mesher
    mesher1D = ot.LevelSetMesher([7])
    print("mesher1D=", mesher1D)

    level = 0.5
    function1D = ot.SymbolicFunction("x", "cos(x)/(1+0.1*x^2)")
    levelSet1D = ot.LevelSet(function1D, ot.LessOrEqual(), level)

    # Manual bounding box
    mesh1D = mesher1D.build(levelSet1D, ot.Interval(-10.0, 10.0))
    print("mesh1D=", mesh1D)

    # The 2D mesher
    mesher2D = ot.LevelSetMesher([5] * 2)
    print("mesher2D=", mesher2D)

    function2D = ot.SymbolicFunction(
        ["x0", "x1"], ["cos(x0 * x1)/(1 + 0.1 * (x0^2 + x1^2))"])
    levelSet2D = ot.LevelSet(function2D, ot.LessOrEqual(), level)

    # Manual bounding box
    mesh2D = mesher2D.build(levelSet2D, ot.Interval([-10.0] * 2, [10.0] * 2))
    print("mesh2D=", mesh2D)

    # The 3D mesher
    mesher3D = ot.LevelSetMesher([3] * 3)
    print("mesher3D=", mesher3D)

    function3D = ot.SymbolicFunction(
        ["x0", "x1", "x2"], ["cos(x0 * x1 + x2)/(1 + 0.1*(x0^2 + x1^2 + x2^2))"])
    levelSet3D = ot.LevelSet(function3D, ot.LessOrEqual(), level)

    # Manual bounding box
    mesh3D = mesher3D.build(levelSet3D, ot.Interval([-10.0] * 3, [10.0] * 3))
    print("mesh3D=", mesh3D)

except:
    import sys
    print("t_IntervalMesher_std.py", sys.exc_info()[0], sys.exc_info()[1])
