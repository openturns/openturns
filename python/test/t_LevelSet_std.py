#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()

try:
    # The function defining the level-set
    inVars = ["x", "y"]
    formula1 = ["x^4 + y^4"]
    function1 = SymbolicFunction(inVars, formula1)
    levelSet1 = LevelSet(function1, LessOrEqual(), 1.0)
    print("levelSet1=", levelSet1)
    p0 = [-0.5, -0.5]
    p1 = [0.5, 0.0]
    p2 = [1.5, 0.0]
    print("levelSet1 contains ", p0, "? ", p0 in levelSet1)
    print("levelSet1 contains ", p1, "? ", p1 in levelSet1)
    print("levelSet1 contains ", p2, "? ", p2 in levelSet1)
    # Intersection
    formula2 = ["(x-1)^2 + y^2"]
    function2 = SymbolicFunction(inVars, formula2)
    levelSet2 = LevelSet(function2, LessOrEqual(), 1.0)
    print("levelSet2=", levelSet2)
    print("levelSet2 contains ", p0, "? ", levelSet2.contains(p0))
    print("levelSet2 contains ", p1, "? ", levelSet2.contains(p1))
    print("levelSet2 contains ", p2, "? ", levelSet2.contains(p2))
    intersection = levelSet1.intersect(levelSet2)
    print("intersection of ", levelSet1, " and ",
          levelSet2, " equals ", intersection)
    print("intersection contains ", p0, "? ", intersection.contains(p0))
    print("intersection contains ", p1, "? ", intersection.contains(p1))
    print("intersection contains ", p2, "? ", intersection.contains(p2))
    # Union
    join = levelSet1.join(levelSet2)
    print("join of ", levelSet1, " and ", levelSet2, " equals ", join)
    print("join contains ", p0, "? ", join.contains(p0))
    print("join contains ", p1, "? ", join.contains(p1))
    print("join contains ", p2, "? ", join.contains(p2))

except:
    import sys
    print("t_LevelSet_std.py", sys.exc_info()[0], sys.exc_info()[1])
