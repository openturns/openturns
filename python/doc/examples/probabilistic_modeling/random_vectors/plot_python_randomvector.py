"""
Create a random vector
======================
"""
# %%
# In this example we are going to create a distribution or copula.
#
# The way to go is inheriting the :class:`~openturns.PythonRandomVector` class and overload its methods:
#
# - `:meth:`~openturns.PythonRandomVector.getRealization`
# - `:meth:`~openturns.PythonRandomVector.getSample`
# - `:meth:`~openturns.PythonRandomVector.getMean`
# - `:meth:`~openturns.PythonRandomVector.getCovariance`

# %%
import openturns as ot

ot.Log.Show(ot.Log.NONE)


# %%
# Inherit :class:`~openturns.PythonRandomVector`
class RVEC(ot.PythonRandomVector):
    def __init__(self):
        super(RVEC, self).__init__(2)
        self.setDescription(["R", "S"])

    def getRealization(self):
        X = [ot.RandomGenerator.Generate(), 2.0 + ot.RandomGenerator.Generate()]
        return X

    def getSample(self, size):
        X = []
        for i in range(size):
            X.append(
                [ot.RandomGenerator.Generate(), 2.0 + ot.RandomGenerator.Generate()]
            )
        return X

    def getMean(self):
        return [0.5, 2.5]

    def getCovariance(self):
        return [[1.0, 0.0], [0.0, 1.0]]


# %%
# Instantiate the distribution
randomVector = ot.RandomVector(RVEC())

# %%
# Get a sample
randomVector.getSample(5)

# %%
# Get mean
randomVector.getMean()

# %%
# Compute the probability contained in an interval
randomVector.getCovariance()
