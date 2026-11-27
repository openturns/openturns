#!/usr/bin/env python

import math
import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott


def scaled_exp(X):
    alpha = 1.0e6
    return [math.exp(-X[0] / alpha)]


def check_case(name, func, x, step0, exact_prime, tol):
    f = ot.PythonFunction(1, 1, func)
    algo = otexp.SteplemanWinarsky(f, x, step0)
    algo.run()
    h_opt = algo.getResult().getOptimalStep()
    print(name, "h_opt=", h_opt)
    assert len(h_opt) == 1
    assert h_opt[0] < step0[0]
    assert h_opt[0] > 0.0
    h = h_opt[0]
    fd = (f([x[0] + h])[0] - f([x[0] - h])[0]) / (2.0 * h)
    print(name, "fd=", fd, "exact=", exact_prime)
    ott.assert_almost_equal(fd, exact_prime, 0.0, tol)


# scaled_exp at x=1e-2, large initial step (use-case example)
f = ot.PythonFunction(1, 1, scaled_exp)
x = [1.0e-2]
step0 = [1.0e8]
algo = otexp.SteplemanWinarsky(f, x, step0)
print("alpha=", algo.getAlpha())
ott.assert_almost_equal(algo.getAlpha(), 0.25)
ott.assert_almost_equal(algo.getX(), x)
ott.assert_almost_equal(algo.getInitialStep(), step0)
algo.run()
h_opt = algo.getResult().getOptimalStep()
print("h_opt=", h_opt)
assert h_opt[0] < step0[0] and h_opt[0] > 0.0
alpha = 1.0e6
exact = -math.exp(-x[0] / alpha) / alpha
h = h_opt[0]
fd = (f([x[0] + h])[0] - f([x[0] - h])[0]) / (2.0 * h)
print("fd=", fd, "exact=", exact)
ott.assert_almost_equal(fd, exact, 0.0, 1.0e-9)

# the result provides the model with the finite-difference gradient
fd_function = algo.getResult().getFunctionWithOptimalStep()
assert fd_function.getGradient().getImplementation().getClassName() == "CenteredFiniteDifferenceGradient"
ott.assert_almost_equal(fd_function.gradient(x)[0, 0], exact, 0.0, 1.0e-9)

# exp at x=1
check_case("exp", lambda X: [math.exp(X[0])], [1.0], [1.0], math.exp(1.0), 5.0e-10)

# scaled_exp at x=1
check_case(
    "scaled_exp",
    scaled_exp,
    [1.0],
    [1.0e8],
    -math.exp(-1.0 / 1.0e6) / 1.0e6,
    1.0e-15,
)

# sin at zero: f=0, f'=cos(0)=1, f'''=-cos(0)=-1 != 0
check_case("sin_at_zero", lambda X: [math.sin(X[0])], [0.0], [1.0], 1.0, 1.0e-7)

with ott.assert_raises(TypeError):
    otexp.SteplemanWinarsky(f, x, [0.0])
with ott.assert_raises(TypeError):
    algo = otexp.SteplemanWinarsky(f, x, step0)
    algo.setAlpha(1.0)
with ott.assert_raises(TypeError):
    algo = otexp.SteplemanWinarsky(f, x, step0)
    algo.setAlpha(0.0)
with ott.assert_raises(TypeError):
    algo = otexp.SteplemanWinarsky(f, x, step0)
    algo.setMaximumIterationNumber(0)

# input accessors
shifted = otexp.SteplemanWinarsky(f, x, step0)
exp_func = ot.PythonFunction(1, 1, lambda X: [math.exp(X[0])])
shifted.setFunction(exp_func)
shifted.setX([1.0])
shifted.setInitialStep([1.0])
ott.assert_almost_equal(shifted.getFunction()([1.0]), [math.exp(1.0)])
ott.assert_almost_equal(shifted.getX(), [1.0])
ott.assert_almost_equal(shifted.getInitialStep(), [1.0])
shifted.run()
ott.assert_almost_equal(
    shifted.getResult().getFunctionWithOptimalStep().gradient([1.0])[0, 0],
    math.exp(1.0),
    0.0,
    1.0e-9,
)

# comparison operator
clone = otexp.SteplemanWinarsky(f, x, step0)
assert clone == otexp.SteplemanWinarsky(f, x, step0)
moved = otexp.SteplemanWinarsky(f, [1.0], step0)
assert not (clone == moved)
assert moved != clone
assert clone.getResult() == otexp.SteplemanWinarsky(f, x, step0).getResult()
exp_func = ot.SymbolicFunction(["x"], ["exp(x)"])
sin_func = ot.SymbolicFunction(["x"], ["sin(x)"])
algo_exp = otexp.SteplemanWinarsky(exp_func, [1.0], [1.0])
algo_sin = otexp.SteplemanWinarsky(sin_func, [1.0], [1.0])
assert algo_exp == otexp.SteplemanWinarsky(exp_func, [1.0], [1.0])
assert algo_exp != algo_sin
algo_exp.run()
algo_sin.run()
rerun = otexp.SteplemanWinarsky(exp_func, [1.0], [1.0])
rerun.run()
assert algo_exp.getResult() == rerun.getResult()
assert algo_exp.getResult() != algo_sin.getResult()

# exhaustion is reported instead of a step: for f(t)=t^3 at x=0
# the successive differences still decrease when the limit is reached
cubic = ot.PythonFunction(1, 1, lambda X: [X[0] ** 3])
stalled = otexp.SteplemanWinarsky(cubic, [0.0], [1.0])
stalled.setAlpha(1.0 / 1.01)
with ott.assert_raises(RuntimeError):
    stalled.run()

# a tight budget on a converging case is reported as well
limited = otexp.SteplemanWinarsky(f, x, step0)
assert limited.getMaximumIterationNumber() == 100
limited.setMaximumIterationNumber(3)
assert limited.getMaximumIterationNumber() == 3
with ott.assert_raises(RuntimeError):
    limited.run()

# multi-output case: one optimal step vector per marginal
multi = ot.PythonFunction(
    1, 2, lambda X: [math.exp(X[0]), math.exp(-X[0] / 1.0e6)]
)
x_multi = [1.0]
algo_multi = otexp.SteplemanWinarsky(multi, x_multi, [1.0])
algo_multi.run()
result_multi = algo_multi.getResult()
h_multi_0 = result_multi.getOptimalStep(0)
h_multi_1 = result_multi.getOptimalStep(1)
print("multi h_opt_0=", h_multi_0, "h_opt_1=", h_multi_1)
assert h_multi_0[0] < 1.0 and h_multi_0[0] > 0.0
assert h_multi_1[0] < 1.0 and h_multi_1[0] > 0.0
# steps are marginal-specific: the flat marginal needs a larger step
assert h_multi_1[0] / h_multi_0[0] > 10.0
fd_multi = result_multi.getFunctionWithOptimalStep()
g_multi = fd_multi.gradient(x_multi)
print("multi gradient=", g_multi)
ott.assert_almost_equal(g_multi[0, 0], math.exp(1.0), 0.0, 1.0e-9)
ott.assert_almost_equal(
    g_multi[0, 1], -math.exp(-1.0 / 1.0e6) / 1.0e6, 0.0, 1.0e-9
)
with ott.assert_raises(ValueError):
    result_multi.getOptimalStep(5)

# step collection accessors
steps = result_multi.getOptimalStepCollection()
assert steps.getSize() == 2
result_multi.setOptimalStepCollection(steps)
ott.assert_almost_equal(result_multi.getOptimalStep(0), steps[0])
ott.assert_almost_equal(result_multi.getOptimalStep(1), steps[1])

# overflowing differences are skipped instead of stopping the search
huge = ot.PythonFunction(1, 1, lambda X: [1.0e308 * X[0]])
overflow = otexp.SteplemanWinarsky(huge, [0.0], [1.5])
overflow.setAlpha(1.0 / 1.1)
overflow.run()
g_huge = overflow.getResult().getFunctionWithOptimalStep().gradient([0.0])
print("huge gradient=", g_huge)
ott.assert_almost_equal(g_huge[0, 0], 1.0e308, 0.0, 1.0e294)

# no finite quotient at all is reported instead of an infinite step
stuck = otexp.SteplemanWinarsky(huge, [1.0e10], [1.0])
with ott.assert_raises(RuntimeError):
    stuck.run()

# coincident initial quotients converge during initialization
linear = ot.PythonFunction(1, 1, lambda X: [X[0]])
init_conv = otexp.SteplemanWinarsky(linear, [0.0], [1.0])
init_conv.setMaximumIterationNumber(2)
init_conv.run()
ott.assert_almost_equal(init_conv.getResult().getOptimalStep(), [0.25])
ott.assert_almost_equal(
    init_conv.getResult().getFunctionWithOptimalStep().gradient([0.0])[0, 0], 1.0
)

# subnormal initial step: scalar division avoids the reciprocal overflow
tiny = ot.PythonFunction(1, 1, lambda X: [X[0]])
tiny_search = otexp.SteplemanWinarsky(tiny, [0.0], [1.0e-310])
tiny_search.run()
ott.assert_almost_equal(tiny_search.getResult().getOptimalStep(), [1.0e-310])
ott.assert_almost_equal(
    tiny_search.getResult().getFunctionWithOptimalStep().gradient([0.0])[0, 0],
    1.0,
)
