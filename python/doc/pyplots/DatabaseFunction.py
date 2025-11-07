import openturns as ot
import openturns.viewer as otv

inputSample = [[1.2, 1.0], [2.3, 0.8], [2.5, 2.3], [1.4, 4.1], [4.5, 5.3]]
outputSample = [[0.2], [-2], [3.2], [4.0], [1.3]]
f = ot.DatabaseFunction(inputSample, outputSample)

ot.ResourceMap.SetAsBool("Contour-DefaultIsFilled", True)
graph = f.draw([0.5, 0.5], [5.0, 6.0], [501] * 2)
data_cloud = ot.Cloud(inputSample)
data_cloud.setPointStyle('star')
data_cloud.setColor('red')
data_cloud.setLegend('data')
graph.add(data_cloud)
graph.setXTitle("$x$")
graph.setYTitle("$y$")
graph.setTitle("Data-based function")
graph.setLegendPosition('bottomright')

view = otv.View(graph)
view.show()
