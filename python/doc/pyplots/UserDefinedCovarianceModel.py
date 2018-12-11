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

vertices = myMesh.getVertices()
myCovariance = ot.CovarianceMatrix(len(vertices))
for k in range(len(vertices)):
    t = vertices[k]
    for l in range(k + 1):
        s = vertices[l]
        myCovariance[k, l] = C(s[0], t[0])

covarianceModel = ot.UserDefinedCovarianceModel(myMesh, myCovariance)
cov_graph = covarianceModel.draw(0, 0, -a, a, 512)
cov_graph.setTitle('User defined covariance model')

fig = plt.figure(figsize=(10, 4))
cov_axis = fig.add_subplot(111)
View(cov_graph, figure=fig, axes=[cov_axis],
     add_legend=False, square_axes=True)
