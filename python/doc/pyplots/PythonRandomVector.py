import openturns as ot
from openturns.viewer import View

class NormalTruncatedToBall(ot.PythonRandomVector):
    def __init__(self, dim, max_dist):
        super().__init__(dim)
        self._center = ot.Point(dim)
        self._normal = ot.Normal(dim)
        self._max_dist = max_dist
        self.setParameter(ot.Point(dim))

    def getRealization(self):
        dist = ot.SpecFunc.MaxScalar
        while dist>self._max_dist:
            candidate = self._normal.getRealization()
            dist = (candidate - self._center).norm()
        return candidate

    def setParameter(self, center):
        self._center = center

    def getParameter(self):
        return self._center

    def getParameterDescription(self):
        return ["center_{}".format(i) for i in range(self.getDimension())]

myRV = ot.RandomVector(NormalTruncatedToBall(2, 1.5))
myRV.setParameter([1.3, 0.6])
sample = myRV.getSample(1000)

graph = ot.Graph("Sample from a PythonRandomVector", "", "", True, '')
cloud = ot.Cloud(sample)
graph.add(cloud)
View(graph)

