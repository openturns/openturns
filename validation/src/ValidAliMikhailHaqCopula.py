import sympy as sp

theta_, u, v = sp.symbols("theta_ u v")

prod = theta_ * (1.0 - u) * (1.0 - v)
pdf = (1.0 + theta_ * ((1.0 + u) * (1.0 + v) - 3.0 + prod)) / (1.0 - prod) ** 3

print("dpdf/dtheta=", sp.simplify(sp.diff(pdf, theta_)))

cdf = u * v / (1.0 - theta_ * (1.0 - u) * (1.0 - v))

print("dcdf/dtheta=", sp.simplify(sp.diff(cdf, theta_)))
