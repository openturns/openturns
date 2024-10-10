from sympy import *

theta_, u, v = symbols('theta_ u v')


thetaMinus1_ = theta_ - 1
uv = thetaMinus1_ * (2.0 * u * v - u - v)
pdf = -theta_ * (uv - 1.0) / ((thetaMinus1_ * (u - v))**2 - 2.0 * uv + 1.0)**(3/2)

print("dpdf/dtheta=", simplify(diff(pdf, theta_)))

cdf = (1.0 + thetaMinus1_ * (u + v) - ((1.0 + thetaMinus1_ * (u + v))**2 - 4.0 * u * v * theta_ * thetaMinus1_)**0.5) / (2.0 * thetaMinus1_)

print("dcdf/dtheta=", simplify(diff(cdf, theta_)))




