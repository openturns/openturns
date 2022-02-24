import openturns as ot
import math as m
import time

#ot.ResourceMap.SetAsString("SymbolicParser-Backend", "MuParser")

# one evaluation of a big sample, varying formula length
N = int(1e7)
x = ot.ComposedDistribution([ot.Uniform(-m.pi, m.pi)] * 3).getSample(N)
for M in [1, 10, 50]:
    f = ot.SymbolicFunction(['x1', 'x2', 'x3'], ['+'.join(['cosh(x1)+cosh(x2)+cosh(x3)'] * M)])
    t0 = time.time()
    y = f(x)
    t1 = time.time()
    print('M=', M, 't=', round(t1 - t0, 2))

# before:
# M= 1 t= 0.76
# M= 10 t= 5.02
# M= 50 t= 23.5

# after:
# M= 1 t= 0.2
# M= 10 t= 0.49
# M= 50 t= 1.78


# tune small sample threshold
N = int(1e7)
for M in [1, 10, 50]:
    print('-' * 20)
    print('M=', M)
    f = ot.SymbolicFunction(['x1', 'x2', 'x3'], ['+'.join(['cosh(x1)+cosh(x2)+cosh(x3)'] * M)])
    for p in range(1, 7):
        m1 = int(10 ** p)
        x = ot.ComposedDistribution([ot.Uniform(-m.pi, m.pi)] * 3).getSample(m1)
        t0 = time.time()
        for i in range(N // m1):
            y = f(x)
        t1 = time.time()
        print(N // m1, '*', m1, 't=', round(t1 - t0, 2))

# before
#10000000 * 10 t= 111.24
#1000000 * 100 t= 12.32
#100000 * 1000 t= 6.22
#10000 * 10000 t= 5.54
#1000 * 100000 t= 5.45
#100 * 1000000 t= 5.59
#10 * 10000000 t= 6.41

# after
#10000000 * 10 t= 82.97
#1000000 * 100 t= 24.28
#100000 * 1000 t= 4.47
#10000 * 10000 t= 0.98
#1000 * 100000 t= 0.58
#100 * 1000000 t= 0.53
#10 * 10000000 t= 1.31
