#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()

try:
    # The 1D interface
    dim = 2
    a = Point(dim, -1.0)
    b = Point(dim, 2.0)
    domain = Domain(Interval(a, b))
    p1 = Point(dim, 0.5)
    p2 = Point(dim, 2.5)
    print("Domain=", domain)
    # Accessors
    print("Dimension=", domain.getDimension())
    # Contains
    print("is point ", p1, " inside ",  domain,  "? ",  p1 in domain)
    print("is point ", p2, " inside ",  domain,  "? ",  p2 in domain)
except:
    import sys
    print("t_Domain_std.py", sys.exc_info()[0], sys.exc_info()[1])
