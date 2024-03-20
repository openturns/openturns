from sympy import *

x, a,b,a_, b_, point, gamma_, expX, beta_, alpha_, mu, sigma, skew= symbols('x a b a_ b_ point gamma_ expX beta_ alpha_ mu sigma skew', real=True)

alpha_ = Symbol('alpha_', positive=True)

# skew=2(1+a)/(a-3)sqrt((a-2)/a)=>a(a-3)^2*s^2=4(1+a)^2(a-2)
fskew = expand(alpha_*(alpha_-3)**2*skew**2-4*(1+alpha_)**2*(alpha_-2))
#iskew = solve(fskew, alpha_)

#iskew = solveset(fskew, alpha_, domain=S.Reals)
#print(iskew)
#print(str(iskew).replace('skew**2', 'sk2').replace('skew**4', 'sk4').replace('skew**6', 'sk6'))

roots = solve(2*(alpha_+1)/(alpha_-3)*sqrt((alpha_-2)/alpha_)-skew, alpha_)
root1 = roots[0]



#roots = solve(fskew, alpha_)

for root in roots:
    print(root)
    #print(str(root).replace('skew**2', 'sk2').replace('skew**4', 'sk4').replace('skew**6', 'sk6'))
    print(root.subs(skew, 5.4))
    fskew.subs(alpha_, root).subs(skew, 5.4)
    #print('-'*100)

root1 = roots[0]
print('alpha=', root1)
#print(simplify(root1))
print(simplify(fskew.subs(alpha_, root1)))


#print(root1.subs(skew, 5.4))
