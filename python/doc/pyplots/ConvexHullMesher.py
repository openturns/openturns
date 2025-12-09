import openturns as ot
import openturns.experimental as otexp
import openturns.viewer as otv

points = ot.JointDistribution([ot.Uniform()] * 2).getSample(100)
mesher = otexp.ConvexHullMesher()

graph = ot.Graph("points", "x1", "x2", True)
cloud = ot.Cloud(points)
graph.add(cloud)
graph.setTitle("Convex hull")

if ot.PlatformInfo.HasFeature("qhull"):
    hull = mesher.build(points)
    graph.add(hull.draw())

view = otv.View(graph)
otv.View.ShowAll()
