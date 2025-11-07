import openturns as ot
from matplotlib import pyplot as plt
import openturns.viewer as otv

a = 4.0
covarianceModel = ot.FractionalBrownianMotionModel(1.0, 1.0, 0.25)
cov_graph = covarianceModel.draw(0, 0, -a, a, 512)
cov_graph.setTitle(str(covarianceModel))
fig = plt.figure(figsize=(10, 4))
cov_axis = fig.add_subplot(111)
otv.View(cov_graph, figure=fig, axes=[cov_axis], add_legend=False, square_axes=True)
