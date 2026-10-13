#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott
import copy
import os

ot.TESTPREAMBLE()

# base factory: the 4-arg build forwards to the weighted 5-arg build,
# both raise NotYetImplementedException, as does involvesModelSelection
factory = ot.ApproximationAlgorithmImplementationFactory()
assert len(repr(factory)) > 0
assert factory == factory
assert not (factory != factory)

x = ot.Sample([[0.0], [1.0], [2.0]])
y = ot.Sample([[0.0], [1.0], [4.0]])
psi = [ot.SymbolicFunction(["x"], ["1.0"]), ot.SymbolicFunction(["x"], ["x"])]
with ott.assert_raises(Exception):
    factory.build(x, y, psi, [0, 1])
with ott.assert_raises(Exception):
    factory.build(x, y, [1.0, 1.0, 1.0], psi, [0, 1])
with ott.assert_raises(Exception):
    factory.involvesModelSelection()

# clone (exercises the virtual constructor)

assert copy.copy(factory).getClassName() == factory.getClassName()

# a concrete factory implements the interface
concrete = ot.LeastSquaresMetaModelSelectionFactory(
    ot.LARS(), ot.CorrectedLeaveOneOut()
)
assert concrete.involvesModelSelection()
algo = concrete.build(x, y, [1.0, 1.0, 1.0], psi, [0, 1])
assert len(algo.getCoefficients()) == 2

# save/load of the base factory
study = ot.Study()
study.setStorageManager(ot.XMLStorageManager("approx_factory.xml"))
study.add("factory", factory)
study.save()
loaded_study = ot.Study()
loaded_study.setStorageManager(ot.XMLStorageManager("approx_factory.xml"))
loaded_study.load()
loaded = ot.ApproximationAlgorithmImplementationFactory()
loaded_study.fillObject("factory", loaded)
assert loaded.getClassName() == factory.getClassName()

os.remove("approx_factory.xml")
