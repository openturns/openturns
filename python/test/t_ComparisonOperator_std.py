#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

for op in [ot.Less(), ot.LessOrEqual(), ot.Equal(), ot.GreaterOrEqual(), ot.Greater()]:
    print("op=", op, "op(10,20)=", op(10, 20))
    print("op=", op, "op(20,20)=", op(20, 20))
    print("op=", op, "op(30,20)=", op(30, 20))

# getImplementation() crash #1461
print(ot.ComparisonOperator().getImplementation())

# ComparisonOperator equality tests
assert ot.Less() == ot.Less(), "Less == Less"
assert not (ot.Less() == ot.Greater()), "Less != Greater"
assert ot.Greater() != ot.LessOrEqual(), "Greater != LessOrEqual"
assert not (ot.Greater() != ot.Greater()), "Greater == Greater"
