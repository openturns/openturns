import openturns as ot
from scipy.special import lambertw
from mpmath import hyper

# Lambert W principal branch
errMax = 0.0
for x in [-0.3, -0.1, 0.0, 1.0, 10.0, 100.0, 1000.0]:
    ref = float(lambertw(x).real)
    val = ot.SpecFunc.LambertW(x)
    err = abs(val - ref) / max(1.0, abs(ref))
    errMax = max(errMax, err)
    if err > 1e-14:
        print("LambertW x=", x, "val=", val, "ref=", ref, "err=", err)
print("LambertW principal max rel err:", errMax)

# Lambert W non-principal branch
errMax = 0.0
for x in [-0.3, -0.1, -0.01, -0.001]:
    ref = float(lambertw(x, k=-1).real)
    val = ot.SpecFunc.LambertW(x, False)
    err = abs(val - ref) / max(1.0, abs(ref))
    errMax = max(errMax, err)
    if err > 1e-14:
        print("LambertW- x=", x, "val=", val, "ref=", ref, "err=", err)
print("LambertW non-principal max rel err:", errMax)

# HyperGeom_2_2
errMax = 0.0
params = [
    (1.0, 1.0, 2.0, 2.0, 0.5),
    (2.0, 3.0, 4.0, 5.0, 0.3),
    (0.5, 1.5, 2.5, 3.5, 0.1),
    (5.0, 5.0, 10.0, 10.0, 0.8),
    (0.25, 0.75, 1.25, 1.75, 0.6),
    (0.1, 0.2, 0.3, 0.4, 0.9),
    (10.0, 10.0, 20.0, 20.0, 0.5),
]
for p1, p2, q1, q2, x in params:
    ref = float(hyper([p1, p2], [q1, q2], x))
    val = ot.SpecFunc.HyperGeom_2_2(p1, p2, q1, q2, x)
    err = abs(val - ref) / max(1.0, abs(ref))
    errMax = max(errMax, err)
    if err > 1e-14:
        print("HyperGeom_2_2 p1=", p1, "p2=", p2, "q1=", q1,
              "q2=", q2, "x=", x, "val=", val, "ref=", ref, "err=", err)
print("HyperGeom_2_2 max rel err:", errMax)
