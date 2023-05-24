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
import openturns.experimental as otexp
import numpy as np
import matplotlib.pyplot as plt

ot.RandomGenerator.SetSeed(5)

# %%
# We first show the advantage of modeling the various levels of a mixed
# continuous-categorical functions through a single surrogate model
# on a simple test-case, defined below.

# %%
def illustrativeFunc(inp):
    x, z = inp
    x = 7 * x
    y = np.cos(x) + 0.5 * z

    return [y]


dim = 2
fun = ot.PythonFunction(dim, 1, illustrativeFunc)
lvls = 2  # Number of discrete levels for z
# Input distribution
dist = ot.ComposedDistribution(
    [ot.Uniform(0, 1), ot.UserDefined(np.arange(0, lvls).reshape((-1, 1)))]
)

# %%
# In this example, we compare the performances of the LatentVariableModel with a naive
# approach, which would consist in modeling each combination of categorical
# variables through a separate and independent Gaussian process.

# %%
# In order to deal with mixed continuous / discrete problems we can rely on the
# ProductCovarianceModel class. We start here by defining the product kernel,
# which combines SquaredExponential kernels for the continuous variables, and
# LatentVariableModel for the discrete ones.

# %%
latdim = 1  # Dimension of the latent space
activecoord = 1 + latdim * (lvls - 2)  # Nb ative coordinates in the latent space
kx = ot.SquaredExponential(1)
kz = otexp.LatentVariableModel(lvls, latdim)
kLV = ot.ProductCovarianceModel([kx, kz])
kLV.setNuggetFactor(1e-6)
# Bounds for the hyperparameter optimization
lb_LV = [1e-4] * dim + [-10] * activecoord
ub_LV = [2.0] * dim + [10] * activecoord
bounds_LV = ot.Interval(lb_LV, ub_LV)
# Distribution for the hyperparameters initialization
initdist_LV = ot.DistributionCollection()
for i in range(len(lb_LV)):
    initdist_LV.add(ot.Uniform(lb_LV[i], ub_LV[i]))
initdist_LV = ot.ComposedDistribution(initdist_LV)

# %%
# As a reference, we consider a purely continuous kernel for independent Gaussian processes.
# One for each combination of categorical variables levels.

# %%
k_ind = ot.SquaredExponential(1)
lb_ind = [1e-4]
ub_ind = [20]
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
# We generate the training data set
X = dist.getSample(10)
Y = fun(X)

# And the plotting data set
Xplt = dist.getSample(200)
Xplt = Xplt.sort()
Yplt = fun(Xplt)

# %%
# Initialize  and parameterize the optimization algorithm
initSample_LV = initdist_LV.getSample(30)
optalg_LV = ot.MultiStart(ot.NLopt("LN_COBYLA"), initSample_LV)

# %%
# Create and train the Gaussian process models
basis = ot.ConstantBasisFactory(2).build()
algo_LV = ot.KrigingAlgorithm(X, Y, kLV, basis)
algo_LV.setOptimizationAlgorithm(optalg_LV)
algo_LV.setOptimizationBounds(bounds_LV)
algo_LV.run()
res_LV = algo_LV.getResult()

algo_ind_list = []
for z in range(2):
    # We select the training samples corresponding to the correct combination
    # of categorical levels
    ind = np.where(np.all(np.array(X[:, 1]) == z, axis=1))[0]
    Xl = X[ind][:, 0]
    Yl = Y[ind]

    # Create and train the Gaussian process models
    basis = ot.ConstantBasisFactory(1).build()
    algo_ind = ot.KrigingAlgorithm(Xl, Yl, k_ind, basis)
    algo_ind.setOptimizationAlgorithm(optalg_ind)
    algo_ind.setOptimizationBounds(bounds_ind)
    algo_ind.run()
    algo_ind_list.append(algo_ind.getResult())

# %%
# We plot the prediction of the mixed continuous-categorical GP,
# as well as the one of the two separate continuous GPs
fig, (ax1, ax2) = plt.subplots(2, 1, sharex=True, figsize=(15, 10))
for z in range(2):
    # We select the training samples corresponding to the correct combination
    # of categorical levels
    ind = np.where(np.all(np.array(X[:, 1]) == z, axis=1))[0]
    Xl = X[ind][:, 0]
    Yl = Y[ind]

    # Compute the models predictive performances on a validation data set
    # We compute the predictions independently for each level of z
    ind = np.where(np.all(np.array(Xplt[:, 1]) == z, axis=1))[0]
    Xplt_ind = Xplt[ind]
    Yplt_ind = Yplt[ind]

    predMeanLV = res_LV.getConditionalMean(Xplt_ind)
    predMeanInd = algo_ind_list[z].getConditionalMean(Xplt_ind[:, 0])
    predSTDLV = np.sqrt(res_LV.getConditionalMarginalVariance(Xplt_ind))
    predSTDInd = np.sqrt(
        algo_ind_list[z].getConditionalMarginalVariance(Xplt_ind[:, 0])
    )

    (trainingData,) = ax1.plot(Xl[:, 0], Yl, "r*")
    (trueFunction,) = ax1.plot(Xplt_ind[:, 0], Yplt_ind, "k--")
    (prediction,) = ax1.plot(Xplt_ind[:, 0], predMeanLV, "b-")
    stdPred = ax1.fill_between(
        Xplt_ind[:, 0].asPoint(),
        (predMeanLV - predSTDLV).asPoint(),
        (predMeanLV + predSTDLV).asPoint(),
        alpha=0.5,
        color="blue",
    )
    ax2.plot(Xl[:, 0], Yl, "r*")
    ax2.plot(Xplt_ind[:, 0], Yplt_ind, "k--")
    ax2.plot(Xplt_ind[:, 0], predMeanInd, "b-")
    ax2.fill_between(
        Xplt_ind[:, 0].asPoint(),
        (predMeanInd - predSTDInd).asPoint(),
        (predMeanInd + predSTDInd).asPoint(),
        alpha=0.5,
        color="blue",
    )
ax1.legend(
    [trainingData, trueFunction, prediction, stdPred],
    ["Training data", "True function", "Prediction", "Prediction standard deviation"],
)
ax1.set_title("Mixed continuous-discrete modeling")
ax2.set_title("Separate modeling")
ax2.set_xlabel("x", fontsize=15)
ax1.set_ylabel("y", fontsize=15)
ax2.set_ylabel("y", fontsize=15)

# %%
# It can be seen that the joint modeling of categorical and continuous variables
# improves the overall prediction accuracy, as the Gaussian process model is
# able to exploit the information provided by the entire training data set.

# %%
# We now consider a more complex which is a modified version of the Goldstein function.
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
# In this example, we compare the performances of the LatentVariableModel with a naive
# approach, which would consist in modeling each combination of categorical
# variables through a separate and independent Gaussian process.

# %%
# In order to deal with mixed continuous / discrete problems we can rely on the
# ProductCovarianceModel class. We start here by defining the product kernel,
# which combines SquaredExponential kernels for the continuous variables, and
# LatentVariableModel for the discrete ones.

# %%
latdim = 2  # Dimension of the latent space
activecoord = (
    2 + latdim * (lvls1 - 2) + latdim * (lvls2 - 2)
)  # Nb ative coordinates in the latent space
kx1 = ot.SquaredExponential(1)
kx2 = ot.SquaredExponential(1)
kz1 = otexp.LatentVariableModel(lvls1, latdim)
kz2 = otexp.LatentVariableModel(lvls2, latdim)
kLV = ot.ProductCovarianceModel([kx1, kx2, kz1, kz2])
kLV.setNuggetFactor(1e-6)
# Bounds for the hyperparameter optimization
lb_LV = [1e-4] * dim + [-10] * activecoord
ub_LV = [3.] * dim + [10.] * activecoord
bounds_LV = ot.Interval(lb_LV, ub_LV)
# Distribution for the hyperparameters initialization
initdist_LV = ot.DistributionCollection()
for i in range(len(lb_LV)):
    initdist_LV.add(ot.Uniform(lb_LV[i], ub_LV[i]))
initdist_LV = ot.ComposedDistribution(initdist_LV)

# %%
# Alternatively, we consider a purely continuous kernel for independent Gaussian processes.
# One for each combination of categorical variables levels.

# %%
k_ind = ot.SquaredExponential(2)
lb_ind = [1e-4, 1e-4]
ub_ind = [3., 3.]
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
RMSE_ind = []
for rep in range(5):
    # We generate the normalized training data set
    X = dist.getSample(72)
    Y = fun(X)
    Ymax = Y.getMax()
    Ymin = Y.getMin()
    Y = (Y - Ymin) / (Ymin - Ymax)

    # Initialize  and parameterize the optimization algorithm
    initSample_LV = initdist_LV.getSample(10)
    optalg_LV = ot.MultiStart(ot.NLopt("LN_COBYLA"), initSample_LV)

    # Create and train the Gaussian process models
    basis = ot.ConstantBasisFactory(dim).build()
    algo_LV = ot.KrigingAlgorithm(X, Y, kLV, basis)
    algo_LV.setOptimizationAlgorithm(optalg_LV)
    algo_LV.setOptimizationBounds(bounds_LV)
    algo_LV.run()
    res_LV = algo_LV.getResult()

    # Compute the models predictive performances on a validation data set
    Xval = dist.getSample(1000)
    Yval = fun(Xval)
    Yval = (Yval - Ymin) / (Ymin - Ymax)

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
plt.boxplot([RMSE_LV, RMSE_ind])
plt.xticks([1, 2], ["LV", "Independent GPs"])
plt.ylabel("RMSE")

# %%
# The obtained results show, for this test-case, a better modeling performance
# when modeling the function as a mixed categorical/continuous function, rather
# than relying on multiple purely continuous Gaussian processes.
