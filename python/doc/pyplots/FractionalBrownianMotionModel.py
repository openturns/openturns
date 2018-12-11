import openturns as ot
from math import exp
from matplotlib import pyplot as plt
from openturns.viewer import View

a = 4.0
covarianceModel = ot.FractionalBrownianMotionModel(1.0, 1.0, 0.25)
cov_graph = covarianceModel.draw(0, 0, -a, a, 512)
cov_graph.setTitle(str(covarianceModel))
fig = plt.figure(figsize=(10, 4))
cov_axis = fig.add_subplot(111)
View(cov_graph, figure=fig, axes=[cov_axis],
     add_legend=False, square_axes=True)
