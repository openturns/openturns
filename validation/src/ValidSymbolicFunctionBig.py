import openturns as ot
from time import time

backends = ["MuParser", "ExprTk"]
#backends = ["MuParser"]

ot.ResourceMap.SetAsUnsignedInteger('SymbolicParserExprTk-MaxNodeDepth', 100000)

size = 1000

N = 1000
X = ["x" + str(i) for i in range(N)]

P = 10
nb_factors_generator = ot.Binomial(P - 1, 0.5)
# Create the generators once and for all
indices_generator = list()
for k in range(P):
    indices_generator.append(ot.KPermutationsDistribution(k+1, N))
    
for j in range(17):
    print("#"*50)
    M = 2**j
    print("M=", M)
    t0 = time()
    print("Create formula...")
    formula = ''
    for i in range(M):
        K = int(nb_factors_generator.getRealization()[0])
        factor = ''
        indices = indices_generator[K].getRealization()
        for j in range(K+1):
            factor += X[int(indices[j])]
            if j < K:
                factor += "*"
        formula += factor
        if i < M-1:
            formula += "+"
    t1 = time()
    print("Formula length=", len(formula))
    print("Formula creation time=", t1 - t0, "s")
    for backend in backends:
        try:
            print("-"*50)
            ot.ResourceMap.SetAsString("SymbolicParser-Backend", backend)
            print(ot.ResourceMap.GetAsString("SymbolicParser-Backend"))
            print("Create function...")
            t0 = time()
            function = ot.SymbolicFunction(X, [formula])
            t1 = time()
            print("Function creation time=", t1 - t0, "s")
            print("Evaluate function (first)...")
            t0 = time()
            res = function([0.001]*N)
            t1 = time()
            print("value=", res)
            print("First function evaluation time=", t1 - t0, "s")
            print("Evaluate function (other)...")
            t0 = time()
            res = function([[0.001]*N for i in range(size)])
            t1 = time()
            print("Other function evaluation time=", 1000.0 * (t1 - t0) / size, "ms")
        except Exception as ex:
            print("Failed", ex)
            pass
