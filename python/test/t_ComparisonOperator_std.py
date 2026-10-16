#! /usr/bin/env python

import openturns as ot
import os

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

# Extra coverage for Equal / GreaterOrEqual converters and persistence

for op in [ot.Equal(), ot.GreaterOrEqual()]:
    assert len(repr(op)) > 0
    assert len(str(op)) > 0
    assert op == op
    assert not (op != op)
    study = ot.Study()
    study.setStorageManager(ot.XMLStorageManager("t_ComparisonOperator_std_tmp.xml"))
    study.add("op", op)
    study.save()
    loadedStudy = ot.Study()
    loadedStudy.setStorageManager(
        ot.XMLStorageManager("t_ComparisonOperator_std_tmp.xml")
    )
    loadedStudy.load()
    loaded = ot.Equal() if isinstance(op, ot.Equal) else ot.GreaterOrEqual()
    loadedStudy.fillObject("op", loaded)
    assert loaded == op
os.remove("t_ComparisonOperator_std_tmp.xml")
