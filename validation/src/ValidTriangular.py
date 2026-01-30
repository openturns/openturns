import sympy as sp

x, a_, m_, b_ = sp.symbols("x a_ m_ b_")

cdf_am = (x - a_) * (x - a_) / (b_ - a_) / (m_ - a_)

print("cdf_am/db", sp.simplify(sp.diff(cdf_am, b_)))
