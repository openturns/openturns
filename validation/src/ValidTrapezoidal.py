import sympy as sp

x, h_, a_, b_, c_, d_ = sp.symbols("x h_ a_ b_ c_ d_")
h_ = 2.0 / (c_ - a_ + d_ - b_)
cdf_cd = 1 - h_ / 2 * (x - d_) * (x - d_) / (d_ - c_)

print("dcdfcd/dd", sp.simplify(sp.diff(cdf_cd, d_)))
