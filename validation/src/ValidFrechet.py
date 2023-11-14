from sympy import *

x, gamma_, beta_, alpha_ = symbols('x gamma_ beta_ alpha_')
cdf = exp(-((x-gamma_)/beta_)**(-alpha_))
print('dcdf/dalpha', diff(cdf, alpha_))
print('dcdf/dbeta',diff(cdf, beta_))
print('dcdf/dgamma',diff(cdf, gamma_))
pdf = alpha_/beta_*((x-gamma_)/beta_)**(-1.-alpha_)*exp(-((x-gamma_)/beta_)**(-alpha_))
print('dpdf/dalpha',diff(pdf, alpha_))
print('dpdf/dbeta',diff(pdf, beta_))
print('dpdf/dgamma',diff(pdf, gamma_))
