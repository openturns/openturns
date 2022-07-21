from __future__ import print_function
from sympy import *

x, a,b,a_, b_, point, gamma_, expX, beta_, alpha_, mu, sigma = symbols('x a b a_ b_ point gamma_ expX beta_ alpha_ mu sigma')

alpha = ((mu-a)/(b-a))*(((b-mu)*(mu-a))/(sigma*sigma)-1)
print('dalpha/dmu',simplify(diff(alpha, mu)))
print('dalpha/dsigma',simplify(diff(alpha, sigma)))
print('dalpha/da',simplify(diff(alpha, a)))
print('dalpha/db',simplify(diff(alpha, b)))

beta = (((b-mu)*(mu-a))/(sigma*sigma)-1)*((b-mu)/(b-a))
print('dbeta/dmu',simplify(diff(beta, mu)))
print('dbeta/dsigma',simplify(diff(beta, sigma)))
print('dbeta/da',simplify(diff(beta, a)))
print('dbeta/db',simplify(diff(beta, b)))
