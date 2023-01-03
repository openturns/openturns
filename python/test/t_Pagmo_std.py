#! /usr/bin/env python

import openturns as ot
import math
import sys

ot.TESTPREAMBLE()
# ot.Log.Show(ot.Log.ALL)


def progress(percent):
    sys.stderr.write("-- progress=" + str(percent) + "%\n")


def stop():
    sys.stderr.write("-- stop?\n")
    return False


print("----- multi-obj -----")
dim = 2

# multi-objective zdt1 problem
f = ot.SymbolicFunction(
    ["x1", "x2"], ["x1", "var g := 1.0 + 9.0 * (x1 + x2); g * (1.0 - sqrt(x1 / g))"]
)
bounds = ot.Interval([0.0] * 2, [1.0] * 2)
ineq = ot.SymbolicFunction(["x1", "x2"], ["x1-x2"])  # x1>x2

# Create the initial population
size = 100
dist = ot.ComposedDistribution([ot.Uniform(0.0, 1.0)] * 2)
pop0 = dist.getSample(size)

multi_obj = ["nsga2", "moead", "mhaco", "nspso"]
for name in multi_obj:
    for use_ineq in [False, True]:
        zdt1 = ot.OptimizationProblem(f)
        zdt1.setBounds(bounds)
        if use_ineq:
            zdt1.setInequalityConstraint(ineq)
        algo = ot.Pagmo(zdt1, name, pop0)
        algo.setBlockSize(8)
        # algo.setProgressCallback(progress)
        # algo.setStopCallback(stop)
        algo.run()
        result = algo.getResult()
        x = result.getFinalPoints()
        y = result.getFinalValues()
        fronts = result.getParetoFrontsIndices()
        assert len(fronts) > 0, "no pareto"
        print(name, len(fronts))
        assert (
            result.getEvaluationNumber() == algo.getGenerationNumber() * size
        ), "wrong size"

# rosenbrock for the other algorithms
print("----- mono-obj -----")
dim = 2
f = ot.SymbolicFunction(["x1", "x2"], ["1+100*(x2-x1^2)^2+(1-x1)^2"])
ineq = ot.SymbolicFunction(["x1", "x2"], ["-x2"])  # x2<0
bounds = ot.Interval([-1.5] * dim, [1.5] * dim)
pop0 = ot.ComposedDistribution([ot.Uniform(-1.5, 1.5)] * 2).getSample(80)

for name in ot.Pagmo.GetAlgorithmNames():
    if name in multi_obj:
        continue
    for use_ineq in [False, True]:
        problem = ot.OptimizationProblem(f)
        problem.setBounds(bounds)
        if use_ineq:
            problem.setInequalityConstraint(ineq)
        algo = ot.Pagmo(problem, name, pop0)
        algo.setBlockSize(8)
        # algo.setProgressCallback(progress)
        # algo.setStopCallback(stop)
        algo.run()
        result = algo.getResult()
        x = result.getOptimalPoint()
        y = result.getOptimalValue()
        if not use_ineq:
            assert result.getFinalPoints().getSize() == pop0.getSize(), "no final pop"
        assert y[0] < 40.0, str(y)
        print(name, x, y)
        if use_ineq:
            assert x[1] < 1e-5, "verified constraint"

print("----- minlp -----")


def minlp_obj(x):
    obj = 0
    for i in range(3):
        obj += (
            (x[2 * i - 2] - 3) ** 2 / 1000.0
            - (x[2 * i - 2] - x[2 * i - 1])
            + math.exp(20.0 * (x[2 * i - 2] - x[2 * i - 1]))
        )
    return [obj]


def minlp_cstr(x):
    ce1 = 4 * (x[0] - x[1]) ** 2 + x[1] - x[2] ** 2 + x[2] - x[3] ** 2
    ce2 = (
        8 * x[1] * (x[1] ** 2 - x[0])
        - 2 * (1 - x[1])
        + 4 * (x[1] - x[2]) ** 2
        + x[0] ** 2
        + x[2]
        - x[3] ** 2
        + x[3]
        - x[4] ** 2
    )
    ce3 = (
        8 * x[2] * (x[2] ** 2 - x[1])
        - 2 * (1 - x[2])
        + 4 * (x[2] - x[3]) ** 2
        + x[1] ** 2
        - x[0]
        + x[3]
        - x[4] ** 2
        + x[0] ** 2
        + x[4]
        - x[5] ** 2
    )
    ce4 = (
        8 * x[3] * (x[3] ** 2 - x[2])
        - 2 * (1 - x[3])
        + 4 * (x[3] - x[4]) ** 2
        + x[2] ** 2
        - x[1]
        + x[4]
        - x[5] ** 2
        + x[1] ** 2
        + x[5]
        - x[0]
    )
    ci1 = (
        8 * x[4] * (x[4] ** 2 - x[3])
        - 2 * (1 - x[4])
        + 4 * (x[4] - x[5]) ** 2
        + x[3] ** 2
        - x[2]
        + x[5]
        + x[2] ** 2
        - x[1]
    )
    ci2 = -(
        8 * x[5] * (x[5] ** 2 - x[4])
        - 2 * (1 - x[5])
        + x[4] ** 2
        - x[3]
        + x[3] ** 2
        - x[4]
    )
    return [-ce1, -ce2, -ce3, -ce4, -ci1, -ci2]


f = ot.PythonFunction(6, 1, minlp_obj)
bounds = ot.Interval([-5.0] * 6, [5.0] * 6)
ineq = ot.PythonFunction(6, 6, minlp_cstr)
pop0 = ot.ComposedDistribution(
    [ot.Uniform(-5.0, 5.0)] * 4 + [ot.UserDefined([[i - 5] for i in range(11)])] * 2
).getSample(100)
problem = ot.OptimizationProblem(f)
problem.setBounds(bounds)
problem.setInequalityConstraint(ineq)
problem.setVariablesType(
    [ot.OptimizationProblemImplementation.CONTINUOUS] * 4
    + [ot.OptimizationProblemImplementation.INTEGER] * 2
)
for name in ["gaco", "ihs", "sga"]:
    algo = ot.Pagmo(problem, name, pop0)
    algo.setBlockSize(8)
    algo.run()
    result = algo.getResult()
    x = result.getOptimalPoint()
    y = result.getOptimalValue()
    print(name, x, y)
    if name != "ihs":
        assert y[0] < 200.0, str(y)


# check we don't expose penalized values
f = ot.SymbolicFunction(
    ["x1", "x2"], ["x1", "var g := 1.0 + 9.0 * (x1 + x2); g * (1.0 - sqrt(x1 / g))"]
)
zdt1 = ot.OptimizationProblem(f)
bounds = ot.Interval([0.0] * 2, [5.0] * 2)
zdt1.setBounds(bounds)
ineq = ot.ComposedFunction(ot.SymbolicFunction(["y1", "y2"], ["2-y1", "2-y2"]), f)  # y1,y2 <2
zdt1.setInequalityConstraint(ineq)
dist = ot.ComposedDistribution([ot.Uniform(0.0, 5.0)] * 2)
pop0 = dist.getSample(50)
algo = ot.Pagmo(zdt1, "nsga2", pop0)
algo.run()
result = algo.getResult()
x = result.getFinalPoints()
y = result.getFinalValues()
for i in range(y.getSize()):
    assert y[i, 1] < 100.0, "penalized y value"
fi0 = result.getParetoFrontsIndices()[0]
print(fi0)
assert len(fi0) <= 6, "pareto indices " + str(fi0)
