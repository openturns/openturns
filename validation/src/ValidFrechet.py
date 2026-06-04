import sympy as sp

x, gamma_, beta_, alpha_ = sp.symbols("x gamma_ beta_ alpha_")
cdf = sp.exp(-(((x - gamma_) / beta_) ** (-alpha_)))
print("dcdf/dalpha", sp.diff(cdf, alpha_))
print("dcdf/dbeta", sp.diff(cdf, beta_))
print("dcdf/dgamma", sp.diff(cdf, gamma_))
pdf = (
    alpha_
    / beta_
    * ((x - gamma_) / beta_) ** (-1.0 - alpha_)
    * sp.exp(-(((x - gamma_) / beta_) ** (-alpha_)))
)
print("dpdf/dalpha", sp.diff(pdf, alpha_))
print("dpdf/dbeta", sp.diff(pdf, beta_))
print("dpdf/dgamma", sp.diff(pdf, gamma_))
