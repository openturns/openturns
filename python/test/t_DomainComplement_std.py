#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

ot.PlatformInfo.SetNumericalPrecision(6)

dim = 2
p1 = ot.Point(dim, -1.0)
p2 = ot.Point(dim, 2.0)
domain = ot.Interval(p1, p2)

complement = ot.DomainComplement(domain)

# Inside
p3 = (p1 + p2) * 0.5
# Outside
p4 = 2.0 * p2
print("Domain=", domain)
print("Complement=", complement)
# Accessors
print("Dimension=", complement.getDimension())
# Contains
print("is point ", p3, " inside complement? ",  complement.contains(p3))
print("is point ", p4, " inside complement? ",  complement.contains(p4))
sample = [p3, p4]
print("is sample ", sample, " inside? ",  complement.contains(sample))
