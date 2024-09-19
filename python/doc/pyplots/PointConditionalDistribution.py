import openturns as ot
import openturns.experimental as otexp
from matplotlib import pyplot as plt
from openturns.viewer import View

R = ot.CorrelationMatrix(3)
R[1, 0] = R[2, 0] = R[2, 1] = 0.9
conditioned = ot.Normal(3)
conditioned.setR(R)
distribution = otexp.PointConditionalDistribution(conditioned, [1], [2.0])

pdf_graph = distribution.drawPDF()
pdf_graph.setTitle("PointConditionalDistribution(X~Normal|X1=2)")
fig = plt.figure(figsize=(10, 5))
pdf_axis = fig.add_subplot(111)
View(pdf_graph, figure=fig, axes=[pdf_axis], add_legend=False)
