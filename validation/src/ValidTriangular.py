from sympy import *

x, a_, m_, b_ = symbols('x a_ m_ b_')

cdf_am = (x - a_) * (x - a_) / (b_ - a_) / (m_ - a_)

print("cdf_am/db", simplify(diff(cdf_am, b_)))
