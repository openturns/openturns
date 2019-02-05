import openturns as ot
from math import exp
from matplotlib import pyplot as plt
from openturns.viewer import View


N = 512
a = 20.0
# myMesh = ot.IntervalMesher([N]).build(ot.Interval(-a, a))
myMesh = ot.RegularGrid(0.0, 2 * a / N, N + 1)
covarianceModel = ot.ExponentialModel([1.0], [1.0])

myProcess = ot.GaussianProcess(covarianceModel, myMesh)

mySample = myProcess.getSample(1000)

myCovarianceFactory = ot.StationaryCovarianceModelFactory()

myEstimatedModel = myCovarianceFactory.build(mySample)


def f(x):
    res = covarianceModel(x)[0, 0]
    return [res]


func = ot.PythonFunction(1, 1, f)
func.setDescription(['$t$', '$cov$'])


def fEst(X):
    res = myEstimatedModel(X)[0, 0]
    return [res]


funcEst = ot.PythonFunction(1, 1, fEst)
funcEst.setDescription(['$t$', '$cov_{est}$'])


cov_graph = func.draw(-a / 4, a / 4, 1024)
cov_graph.add(funcEst.draw(-a / 4, a / 4, 1024))
cov_graph.setColors(['blue', 'red'])
cov_graph.setTitle('Stationary covariance model estimation')

fig = plt.figure(figsize=(10, 4))
cov_axis = fig.add_subplot(111)
view = View(cov_graph, figure=fig, axes=[cov_axis], add_legend=False)
