#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

R = ot.CorrelationMatrix(2)
R[0, 1] = -0.99
d1 = ot.Normal([-1.0, 1.0], [1.0, 1.0], R)
R[0, 1] = 0.99
d2 = ot.Normal([1.0, 1.0], [1.0, 1.0], R)
distribution = ot.Mixture([d1, d2], [1.0] * 2)
classifier = ot.MixtureClassifier(distribution)
f1 = ot.SymbolicFunction(['x'], ['-x'])
f2 = ot.SymbolicFunction(['x'], ['x'])
experts = [f1, f2]
moe = ot.ExpertMixture(experts, classifier)
moeNMF = ot.Function(moe)

print('Mixture of experts=', moe)

# Evaluate the mixture of experts on some points
for i in range(2):
    p = [-0.3 + 0.8 * i / 4.0]
    print('moe   ( %.6g )=' % p[0], moe(p))
    print('moeNMF( %.6g )=' % p[0], moeNMF(p))
# and on a sample
x = [[-0.3], [0.1]]
print('x=', ot.Sample(x), 'moeNMF(x)=', moeNMF(x))

# non-supervised mode (2d)
f1 = ot.SymbolicFunction(['x1', 'x2'], ['-8'])
f2 = ot.SymbolicFunction(['x1', 'x2'], ['8'])
experts = [f1, f2]
moe = ot.ExpertMixture(experts, classifier, False)
moeNMF = ot.Function(moe)

print('Mixture of experts=', moe)

# Evaluate the mixture of experts on some points
for i in range(2):
    p = (-1.0 + 2 * i, 1.0)
    print('moe   ( %.6g, %.6g )=' % p, moe(p))
    print('moeNMF( %.6g, %.6g )=' % p, moeNMF(p))
# and on a sample
x = [[-1.0, 1.0], [1.0, 1.0]]
print('x=', ot.Sample(x), 'moeNMF(x)=', moeNMF(x))
