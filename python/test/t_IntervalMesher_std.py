#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()

try:
    # The 1D interface
    mesher1D = IntervalMesher([5])
    print("mesher1D=", mesher1D)
    mesh1D = mesher1D.build(Interval(-1.0, 2.0))
    print("mesh1D=", mesh1D)
    mesher2D = IntervalMesher([5, 5])
    print("mesher2D=", mesher2D)
    mesh2D = mesher2D.build(Interval([-1.0, -1.0], [2.0, 2.0]))
    print("mesh2D=", mesh2D)

except:
    import sys
    print("t_IntervalMesher_std.py", sys.exc_info()[0], sys.exc_info()[1])
