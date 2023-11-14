from sympy import *

x, a,b,a_, b_, point, gamma_, expX, beta_, alpha_, mu, sigma = symbols('x a b a_ b_ point gamma_ expX beta_ alpha_ mu sigma')

pdf = alpha_*beta_**alpha_ / (x-gamma_)**(alpha_+1)
print('dpdf/dbeta',simplify(diff(pdf, beta_)))
print('dpdf/dalpha',simplify(diff(pdf, alpha_)))
print('dpdf/dgamma',simplify(diff(pdf, gamma_)))


cdf = 1-(beta_ / (x-gamma_))**alpha_
print('dcdf/dbeta',simplify(diff(cdf, beta_)))
print('dcdf/dalpha',simplify(diff(cdf, alpha_)))
print('dcdf/dgamma',simplify(diff(cdf, gamma_)))


print('ddf',simplify(diff(pdf, x)))
