import sympy as sp

x, a, b, a_, b_, point, gamma_, expX, beta_, alpha_, mu, sigma = sp.symbols(
    "x a b a_ b_ point gamma_ expX beta_ alpha_ mu sigma"
)

pdf = alpha_ * beta_**alpha_ / (x - gamma_) ** (alpha_ + 1)
print("dpdf/dbeta", sp.simplify(sp.diff(pdf, beta_)))
print("dpdf/dalpha", sp.simplify(sp.diff(pdf, alpha_)))
print("dpdf/dgamma", sp.simplify(sp.diff(pdf, gamma_)))


cdf = 1 - (beta_ / (x - gamma_)) ** alpha_
print("dcdf/dbeta", sp.simplify(sp.diff(cdf, beta_)))
print("dcdf/dalpha", sp.simplify(sp.diff(cdf, alpha_)))
print("dcdf/dgamma", sp.simplify(sp.diff(cdf, gamma_)))


print("ddf", sp.simplify(sp.diff(pdf, x)))
