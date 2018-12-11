import openturns as ot
from math import exp
from matplotlib import pyplot as plt
from openturns.viewer import View


def C(tau):
    return 1.0 / (1.0 + tau * tau)


t0 = 0.0
t1 = 20.0
N = 40
dt = (t1 - t0) / (N - 1)
myMesh = ot.RegularGrid(t0, dt, N)

myCovarianceCollection = ot.CovarianceMatrixCollection()
for k in range(N):
    t = myMesh.getValue(k)
    matrix = ot.CovarianceMatrix(1)
    matrix[0, 0] = C(t)
    myCovarianceCollection.add(matrix)

covarianceModel = ot.UserDefinedStationaryCovarianceModel(
    myMesh, myCovarianceCollection)


def f(tau):
    return [covarianceModel(tau)[0, 0]]


func = ot.PythonFunction(1, 1, f)
func.setDescription(['$t$', '$cov$'])
cov_graph = func.draw(0.0, 20.0, 512)
cov_graph.setTitle('User defined stationary covariance model')

fig = plt.figure(figsize=(10, 4))
cov_axis = fig.add_subplot(111)
View(cov_graph, figure=fig, axes=[cov_axis], add_legend=False)
