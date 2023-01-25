"""
Kriging: metamodel with continuous and categorical variables
============================================================
"""
# %%
# We consider here the surrogate modeling of a bending function characterized by
# continuous and categorical variables
#

# %%
import openturns as ot
import numpy as np
import matplotlib.pyplot as plt
from openturns.viewer import View
ot.RandomGenerator_SetSeed(0)

# %%
# We define the bending function depending on the beam length L, its height h
# and the type of cross-section I. We consider here 3 types of cross-section
# characterized by different moments of inertia


# %%
def bending(inp):
    L, h, i = inp
    L = L * 10 + 10
    h = h + 1
    if i == 0:
        iner = 1 / 12  # inv 12
    elif i == 1:
        iner = np.pi / 64  # inv 20
    elif i == 2:
        iner = 0.0167  # inv 60
    else:
        print('error, no matching category')

    y = L**3 / 3 / 1e9 / h**4 / iner
    return [y * 1e4]


fun = ot.PythonFunction(3, 1, bending)
lvls = 3  # Number of discrete levels
dist = ot.ComposedDistribution([ot.Uniform(0, 1), ot.Uniform(0, 1), ot.UserDefined(np.arange(0, lvls).reshape((-1, 1)))])
X = dist.getSample(200)
Y = fun(X)

# %%
# We now define a tensorized kernel in the mixed continuous-discrete space

# %%
latdim = 2  # Dimension of the latent space
kl = ot.SquaredExponential(1)
kh = ot.SquaredExponential(1)
ki = ot.LatentVariableModel(lvls, latdim)
k = ot.ProductCovarianceModel([kl, kh, ki])
k.setNuggetFactor(1e-6)
k.setActiveParameter([0, 1, 3, 4, 5, 6])

# %%
# We can then define the kriging surrogate model
basis = ot.ConstantBasisFactory(3).build()
algo = ot.KrigingAlgorithm(X, Y, k, basis)

# %%
# We perform the likelihood optimization by relying on a multi-start optimization
# of the COBYLA algorithm

# %%
activecoord = 1 + latdim * (lvls - 2)  # Number of active coordinates in the latent space
lb = [1e-4, 1e-4] + [-1] * activecoord
ub = [3, 3] + [1] * activecoord
bounds = ot.Interval(lb, ub)
initdist = ot.DistributionCollection()
for i in range(len(lb)):
    initdist.add(ot.Uniform(lb[i], ub[i]))
initdist = ot.ComposedDistribution(initdist)
initSample = initdist.getSample(3)
optalg = ot.MultiStart(ot.NLopt('LN_COBYLA'), initSample)
algo.setOptimizationAlgorithm(optalg)
algo.setOptimizationBounds(bounds)
algo.run()


# %%
# We then proceed onto the result analysis. We start by plotting the optimal latent mappings.
# It can be seen that the results are coherent with the function definition, as
# the levels are mapped on points, the coordinates of which are proportional to
# the moment of inertia characterizing the different cross-sections.

# %%
res = algo.getResult()
kopt = res.getCovarianceModel().getImplementation()
kopt = kopt.getCollection()[-1]
kopt = kopt.getImplementation()
a = kopt.getFullLatentVariables()
plt.figure()
plt.plot(a[:, 0], a[:, 1], '*')
for i in range(lvls):
    plt.annotate(str(i + 1), ((a[i, 0], a[i, 1])))
plt.ylim(-0.5, 0.5)
plt.title('Latent Variable mappings')

# %%
# We finally validate the surrogate model on a test sample

# %%
Xval = dist.getSample(1000)
Yval = fun(Xval)
Val = ot.MetaModelValidation(Xval, Yval, res.getMetaModel())
validationPlot = Val.drawValidation()
_ = View(validationPlot)
