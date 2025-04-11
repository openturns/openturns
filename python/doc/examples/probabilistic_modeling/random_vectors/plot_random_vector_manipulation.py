"""
Create a random vector
======================
"""

# %%
# The :class:`~openturns.RandomVector` object represents the concept of random variable.
#
# This class can be cerated by:
#
# - Case 1: directly using the class :class:`~openturns.RandomVector`,
# - Case 2: from python, using the class :class:`~openturns.PythonRandomVector` which enables to
# redefine some methods (as the sampling method for example).

# %%
import openturns as ot
ot.Log.Show(ot.Log.NONE)

# %%
# Case 1: use the class :class:`~openturns.RandomVector`
# ------------------------------------------------------
#
# We create the random vector :math:`(X_0, X_1, X_2)` following the Normal
# distribution with zero mean and unit variance.
dist3d = ot.Normal(3)
X = ot.RandomVector(dist3d)

# %%
# Get the dimension
X.getDimension()

# %%
# Get the mean
X.getMean()

# %%
# Get the covariance
X.getCovariance()

# %%
# Draw a sample
X.getSample(5)

# %%
# Extract the component :math:`X_1`:
X1 = X.getMarginal(1)
X1.getSample(5)

# %%
# Extract the component :math:`(X_1, X_2)`.
X02 = X.getMarginal([0, 2])
X02.getSample(5)

# %%
# Case 2: use the class :class:`~openturns.PythonRandomVector`
# ------------------------------------------------------------
#
# We create a random vector using the :class:`~openturns.PythonRandomVector` class, which
# enables to overload the following methods: *getRealization*, *getSample*,
# *getMean* and **getCovariance*.
#
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
# Get its mean
randomVector.getMean()

# %%
# Compute the probability contained in an interval
randomVector.getCovariance()
