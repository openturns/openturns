#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

ot.TESTPREAMBLE()

for op in [ot.Less(), ot.LessOrEqual(), ot.Equal(), ot.GreaterOrEqual(), ot.Greater()]:
    print('op=', op, 'op(10,20)=', op(10, 20))
    print('op=', op, 'op(20,20)=', op(20, 20))
    print('op=', op, 'op(30,20)=', op(30, 20))

# getImplementation() crash #1461
print(ot.ComparisonOperator().getImplementation())
