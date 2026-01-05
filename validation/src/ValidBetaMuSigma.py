import sympy as sp

x, a, b, a_, b_, point, gamma_, expX, beta_, alpha_, mu, sigma = sp.symbols(
    "x a b a_ b_ point gamma_ expX beta_ alpha_ mu sigma"
)

alpha = ((mu - a) / (b - a)) * (((b - mu) * (mu - a)) / (sigma * sigma) - 1)
print("dalpha/dmu", sp.simplify(sp.diff(alpha, mu)))
print("dalpha/dsigma", sp.simplify(sp.diff(alpha, sigma)))
print("dalpha/da", sp.simplify(sp.diff(alpha, a)))
print("dalpha/db", sp.simplify(sp.diff(alpha, b)))

beta = (((b - mu) * (mu - a)) / (sigma * sigma) - 1) * ((b - mu) / (b - a))
print("dbeta/dmu", sp.simplify(sp.diff(beta, mu)))
print("dbeta/dsigma", sp.simplify(sp.diff(beta, sigma)))
print("dbeta/da", sp.simplify(sp.diff(beta, a)))
print("dbeta/db", sp.simplify(sp.diff(beta, b)))
