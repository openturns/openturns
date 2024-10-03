from sympy import *

x, h_, a_, b_, c_, d_ = symbols('x h_ a_ b_ c_ d_')
h_ = 2.0 / (c_ - a_ + d_ - b_)
cdf_cd = 1-h_/2 * (x-d_)*(x-d_) / (d_ - c_)

print("dcdfcd/dd", simplify(diff(cdf_cd, d_)))
