from sympy import *

theta_, u, v = symbols('theta_ u v')

prod = theta_ * (1.0 - u) * (1.0 - v)
pdf = (1.0 + theta_ * ((1.0 + u) * (1.0 + v) - 3.0 + prod)) / (1.0 - prod)**3

print("dpdf/dtheta=", simplify(diff(pdf, theta_)))

cdf = u * v / (1.0 - theta_ * (1.0 - u) * (1.0 - v))

print("dcdf/dtheta=", simplify(diff(cdf, theta_)))




