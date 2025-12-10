import openturns.experimental as otexp
import openturns.viewer as otv

mesher = otexp.PolygonMesher()
polyline = [[0.0, 0.0], [0.0, 3.0], [1.0, 2.0], [0.5, 0.5], [1.0, 0.5], [2.0, 2.0], [2.0, 0.0]]
triangulation = mesher.build(polyline)

graph = triangulation.draw()
graph.setTitle("Polygon triangulation")
view = otv.View(graph)
otv.View.ShowAll()
