"""
Kriging: metamodel with continuous and categorical variables
============================================================
"""
# %%
# We consider here the surrogate modeling of an analytical function characterized by
# continuous and categorical variables
#

# %%
import openturns as ot
import numpy as np
import matplotlib.pyplot as plt

ot.RandomGenerator_SetSeed(0)

# %%
# The considered function is a modified version of the Goldstein function.
# This function depends on 2 continuous variables and 2 categorical ones.
# Each categorical variable is characterized by 3 levels.


# %%
def h(x1, x2, x3, x4):
    y = (
        53.3108
        + 0.184901 * x1
        - 5.02914 * x1 ** 3 * 1e-6
        + 7.72522 * x1 ** 4 * 1e-8
        - 0.0870775 * x2
        - 0.106959 * x3
        + 7.98772 * x3 ** 3 * 1e-6
        + 0.00242482 * x4
        + 1.32851 * x4 ** 3 * 1e-6 * 0.00146393 * x1 * x2
        - 0.00301588 * x1 * x3
        - 0.00272291 * x1 * x4
        + 0.0017004 * x2 * x3
        + 0.0038428 * x2 * x4
        - 0.000198969 * x3 * x4
        + 1.86025 * x1 * x2 * x3 * 1e-5
        - 1.88719 * x1 * x2 * x4 * 1e-6
        + 2.50923 * x1 * x3 * x4 * 1e-5
        - 5.62199 * x2 * x3 * x4 * 1e-5
    )
    return y


def Goldstein(inp):
    x1, x2, z1, z2 = inp
    x1 = 100 * x1
    x2 = 100 * x2

    if z1 == 0:
        x3 = 80
    elif z1 == 1:
        x3 = 20
    elif z1 == 2:
        x3 = 50
    else:
        print("error, no matching category z1")

    if z2 == 0:
        x4 = 20
    elif z2 == 1:
        x4 = 80
    elif z2 == 2:
        x4 = 50
    else:
        print("error, no matching category z2")

    return [h(x1, x2, x3, x4)]


dim = 4
fun = ot.PythonFunction(dim, 1, Goldstein)
lvls1 = 3  # Number of discrete levels for z1
lvls2 = 3  # Number of discrete levels for z2
# Input distribution
dist = ot.ComposedDistribution(
    [
        ot.Uniform(0, 1),
        ot.Uniform(0, 1),
        ot.UserDefined(np.arange(0, lvls1).reshape((-1, 1))),
        ot.UserDefined(np.arange(0, lvls2).reshape((-1, 1))),
    ]
)

# %%
# In this example, we compare the performances of two different discrete kernels :
# the GowerDistance and the LatentVariableModel. Moreover, we also consider the
# possibility of modeling each combination of categorical variables through a
# separate and independent Gaussian process as a benchmark solution.

# %%
# In order to deal with mixed continuous / discrete problems we can rely on the
# ProductCovarianceModel class. We start here by defining the two product kernels:

# %%
# One relying on the Gower distance kernel

# %%
kx1 = ot.SquaredExponential(1)
kx2 = ot.SquaredExponential(1)
kz1 = ot.GowerDistanceModel(lvls1)
kz2 = ot.GowerDistanceModel(lvls2)
kGow = ot.ProductCovarianceModel([kx1, kx2, kz1, kz2])
kGow.setNuggetFactor(1e-6)
lb_Gow = [1e-4, 1e-4, 1e-4, 1e-4]
ub_Gow = [20, 20, 20, 20]
bounds_Gow = ot.Interval(lb_Gow, ub_Gow)
initdist_Gow = ot.DistributionCollection()
for i in range(len(lb_Gow)):
    initdist_Gow.add(ot.Uniform(lb_Gow[i], ub_Gow[i]))
initdist_Gow = ot.ComposedDistribution(initdist_Gow)

# %%
# The scond one relying on the latent variable kernel

# %%
latdim = 2  # Dimension of the latent space
activecoord = (
    2 + latdim * (lvls1 - 2) + latdim * (lvls2 - 2)
)  # Nb ative coordinates in the latent space
kx1 = ot.SquaredExponential(1)
kx2 = ot.SquaredExponential(1)
kz1 = ot.LatentVariableModel(lvls1, latdim)
kz2 = ot.LatentVariableModel(lvls2, latdim)
kLV = ot.ProductCovarianceModel([kx1, kx2, kz1, kz2])
kLV.setNuggetFactor(1e-6)
lb_LV = [1e-4, 1e-4, 1e-4, 1e-4] + [-10] * activecoord
ub_LV = [20, 20, 20, 20] + [10] * activecoord
bounds_LV = ot.Interval(lb_LV, ub_LV)
initdist_LV = ot.DistributionCollection()
for i in range(len(lb_LV)):
    initdist_LV.add(ot.Uniform(lb_LV[i], ub_LV[i]))
initdist_LV = ot.ComposedDistribution(initdist_LV)

# %%
# Alternatively, we consider a purely continuous kernel for independent Gaussian processes

# %%
k_ind = ot.SquaredExponential(2)
lb_ind = [1e-4, 1e-4]
ub_ind = [20, 20]
bounds_ind = ot.Interval(lb_ind, ub_ind)
initdist_ind = ot.DistributionCollection()
for i in range(len(lb_ind)):
    initdist_ind.add(ot.Uniform(lb_ind[i], ub_ind[i]))
initdist_ind = ot.ComposedDistribution(initdist_ind)
initSample_ind = initdist_ind.getSample(10)
optalg_ind = ot.MultiStart(ot.NLopt("LN_COBYLA"), initSample_ind)

# %%
# In order to assess their respective robustness with regards to the training data set,
# we repeat the experiments 10 times with different training of size 72,
# and compute each time the normalized prediction Root Mean Squared Error (RMSE) on a
# test data set # of size 1000.

# %%
RMSE_LV = []
RMSE_Gow = []
RMSE_ind = []
for rep in range(5):
    # We generate the normalized training data set
    X = dist.getSample(72)
    Y = fun(X)
    Ymax = Y.getMax()
    Ymin = Y.getMin()
    Y = (Y - Ymin) / (Ymin - Ymax)

    # Initialize  and parameterize the optimization algorithm
    initSample_Gow = initdist_Gow.getSample(10)
    optalg_Gow = ot.MultiStart(ot.NLopt("LN_COBYLA"), initSample_Gow)

    initSample_LV = initdist_LV.getSample(10)
    optalg_LV = ot.MultiStart(ot.NLopt("LN_COBYLA"), initSample_LV)

    # Create and train the Gaussian process models
    basis = ot.ConstantBasisFactory(4).build()
    algo_Gow = ot.KrigingAlgorithm(X, Y, kGow, basis)
    algo_Gow.setOptimizationAlgorithm(optalg_Gow)
    algo_Gow.setOptimizationBounds(bounds_Gow)
    algo_Gow.run()
    res_Gow = algo_Gow.getResult()

    algo_LV = ot.KrigingAlgorithm(X, Y, kLV, basis)
    algo_LV.setOptimizationAlgorithm(optalg_LV)
    algo_LV.setOptimizationBounds(bounds_LV)
    algo_LV.run()
    res_LV = algo_LV.getResult()

    # Compute the models predictive performances on a validation data set
    Xval = dist.getSample(1000)
    Yval = fun(Xval)
    Yval = (Yval - Ymin) / (Ymin - Ymax)

    Val_Gow = ot.MetaModelValidation(Xval, Yval, res_Gow.getMetaModel())
    rmse_Gow = np.sqrt(np.mean(np.array(Val_Gow.getResidualSample()) ** 2))
    RMSE_Gow.append(rmse_Gow)

    Val_LV = ot.MetaModelValidation(Xval, Yval, res_LV.getMetaModel())
    rmse_LV = np.sqrt(np.mean(np.array(Val_LV.getResidualSample()) ** 2))
    RMSE_LV.append(rmse_LV)

    error = np.empty((0, 1))
    for z1 in range(3):
        for z2 in range(3):
            # We select the training samples corresponding to the correct combination
            # of categorical levels
            ind = np.where(np.all(np.array(X[:, 2:]) == [z1, z2], axis=1))[0]
            Xl = X[ind][:, :2]
            Yl = Y[ind]

            # Create and train the Gaussian process models
            basis = ot.ConstantBasisFactory(2).build()
            algo_ind = ot.KrigingAlgorithm(Xl, Yl, k_ind, basis)
            algo_ind.setOptimizationAlgorithm(optalg_ind)
            algo_ind.setOptimizationBounds(bounds_ind)
            algo_ind.run()
            res_ind = algo_ind.getResult()

            # Compute the models predictive performances on a validation data set
            ind = np.where(np.all(np.array(Xval[:, 2:]) == [z1, z2], axis=1))[0]
            Xval_ind = Xval[ind][:, :2]
            Yval_ind = Yval[ind]
            Val_ind = ot.MetaModelValidation(Xval_ind, Yval_ind, res_ind.getMetaModel())
            error = np.append(error, Val_ind.getResidualSample())
    rmse_ind = np.sqrt(np.mean(error ** 2))
    RMSE_ind.append(rmse_ind)

plt.figure()
plt.boxplot([RMSE_Gow, RMSE_LV, RMSE_ind])
plt.xticks([1, 2, 3], ["Gower distance", "LV", "Independent GPs"])
plt.ylabel("RMSE")

# %%
# The obtained results show, for this test-case, a better modeling performance
# when modeling the function as a mixed categorical/continuous function, rather
# than relying on multiple purely continuous Gaussian processes.

# %%
# If now focus only on the results related to the Gower distance and the latent
# variables discrete kernels, we can see that here the latent variable approach
# seems to yield better modeling performances, thanks to its more flexible parameterization.

# %%
plt.figure()
plt.boxplot([RMSE_Gow, RMSE_LV])
plt.xticks([1, 2], ["Gower distance", "LV"])
plt.ylabel("RMSE")
