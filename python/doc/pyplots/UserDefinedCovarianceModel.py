import openturns as ot
from math import exp
from matplotlib import pyplot as plt
from openturns.viewer import View


def C(s, t):
    return exp(-4.0 * abs(s - t) / (1 + (s * s + t * t)))

N = 64
a = 4.0
# myMesh = ot.IntervalMesher([N]).build(ot.Interval(-a, a))
myMesh = ot.RegularGrid(-a, 2 * a / N, N + 1)

myCovarianceCollection = ot.CovarianceMatrixCollection()
for k in range(myMesh.getVerticesNumber()):
    t = myMesh.getVertices()[k]
    for l in range(k + 1):
        s = myMesh.getVertices()[l]
        matrix = ot.CovarianceMatrix(1)
        matrix[0, 0] = C(s[0], t[0])
        myCovarianceCollection.add(matrix)

covarianceModel = ot.UserDefinedCovarianceModel(myMesh, myCovarianceCollection)


def f(x):
    return [covarianceModel([x[0]], [x[1]])[0, 0]]

func = ot.PythonFunction(2, 1, f)
func.setDescription(['$s$', '$t$', '$cov$'])
cov_graph = func.draw([-a] * 2, [a] * 2, [512] * 2)
fig = plt.figure(figsize=(10, 4))
plt.suptitle('User defined covariance model')
cov_axis = fig.add_subplot(111)
View(cov_graph, figure=fig, axes=[cov_axis], add_legend=False)
