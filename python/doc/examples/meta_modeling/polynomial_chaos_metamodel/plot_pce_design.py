"""
Compute leave-one-out error of a polynomial chaos expansion
===========================================================
"""
# %%
#
# Introduction
# ------------
#
# In this example, we compute the design matrix of a polynomial chaos
# expansion using the :class:`~openturns.DesignProxy` class.
# Then we compute the analytical leave-one-out error using the
# diagonal of the projection matrix.
# To do this, we use equations from [blatman2009]_ page 85
# (see also [blatman2011]_).

# %%
# The design matrix
# -----------------
# In this section, we analyze why the :class:`~openturns.DesignProxy`
# is linked to the classical linear least squares regression problem.
# Let :math:`n` be the number of observations and :math:`m` be the number of coefficients
# of the linear model.
# Let :math:`D \in \mathbb{R}^{n \times m}` be the design matrix, i.e.
# the matrix that produces the predictions of the linear regression model from
# the coefficients:
#
# .. math::
#
#     \hat{\vect{y}} = D \vect{a}
#
# where :math:`\vect{a} \in \mathbb{R}^m` is the vector of coefficients,
# :math:`\hat{y} \in \mathbb{R}^n` is the
# vector of predictions.
# The linear least squares problem is:
#
# .. math::
#
#     \operatorname{argmin}_{\vect{a} \in \mathbb{R}^m}
#     \left\| D \vect{a} - \vect{y} \right\|_2^2.
#
# The solution is given by the normal equations, i.e. the vector of coefficients
# is the solution of the following linear system of equations:
#
# .. math::
#
#     G \vect{a} = D^T \vect{y}
#
# where :math:`G \in \Rset^{n \times n}` is the *Gram* matrix:
#
# .. math::
#
#     G := D^T D.
#
# The hat matrix is the projection matrix defined by:
#
# .. math::
#
#     H := D \left(D^T D\right)^{-1} D^T.
#
# The hat matrix puts a hat to the vector of observations to
# produce the vector of predictions of the linear model:
#
# .. math::
#
#    \hat{\vect{y}} = H \vect{y}
#
# To solve a linear least squares problem, we need to evaluate the
# design matrix :math:`D`, which is the primary goal of
# the :class:`~openturns.DesignProxy` class.
# Let us present some examples of situations where the design matrix
# is required.
#
# - When we use the QR decomposition, we actually do not need to evaluate it in
#   our script: the :class:`~openturns.QRMethod` class knows how to compute the
#   solution without evaluating the Gram matrix `D^T D`.
# - We may need the inverse Gram matrix
#   :math:`(D^T D)^{-1}` sometimes, for example when we want to create
#   a D-optimal design.
# - Finally, when we want to compute the analytical leave-one-out error,
#   we need to compute the diagonal of the  projection matrix :math:`H`.
#
# For all these purposes, the `DesignProxy` is _the_ tool.

# %%
# The leave-one-out error
# -----------------------
# In this section, we show that the leave-one-error of a regression problem
# can be computed using an analytical formula which depends on the hat matrix
# :math:`H`.
# We consider the physical model:
#
# .. math::
#
#     y = g(\vect{x})
#
# where :math:`\vect{x} \in \Rset^{n_X}` is the input and :math:`y \in \Rset` is
# the output.
# Consider the problem of approximating the physical model :math:`g` by the
# linear model:
#
# .. math::
#
#     \hat{y} := \tilde{g}(\vect{x}) = \sum_{k = 1}^m a_k \psi_k(\vect{x})
#
# for any :math:`\vect{x} \in \Rset^{n_X}` where :math:`\{\psi_k : \Rset^{n_X} \rightarrow \Rset\}_{k = 1, \ldots, m}` are the basis functions and
# :math:`\vect{a} \in \Rset^m` is a vector of parameters.
# The mean squared error is ([blatman2009]_ eq. 4.30 page 85):
#
# .. math::
#
#     \operatorname{MSE}\left(\tilde{g}\right)
#     = \mathbb{E}_{\vect{X}}\left[\left(g\left(\vect{X}\right) - \tilde{g}\left(\vect{X}\right) \right)^2 \right]
#
# The leave-one-out error is an estimator of the mean squared error.
# Let :math:`\{\vect{x}^{(1)}, \ldots, \vect{x}^{(n)} \in \Rset^{n_X}\}` be
# independent observations of the input random vector :math:`\vect{X}` and
# let :math:`\{y^{(1)}, \ldots, y^{(n)} \in \Rset^{n_X}\}` be the corresponding
# observations of the output of the physical model:
#
# .. math::
#
#     y^{(i)} = g\left(\vect{x}^{(i)}\right)
#
# for :math:`i = 1, ..., n`.
# Let :math:`\vect{y} \in \Rset^n` be the vector of observations:
#
# .. math::
#     \vect{y} = (y^{(1)}, \ldots, y^{(n)})^T.
#
#
# Let :math:`\vect{y}_{-i} \in \Rset^{n - 1}` be the vector of
# observation, let aside the :math:`i`-th observation:
#
# .. math::
#
#     \vect{y}_{-i} = (y_1, \ldots, y_{i - 1}, y_{i + 1}, \ldots, y_n)^T
#
# for :math:`i \in \{1, ..., n\}`.
# The leave-one-out error is:
#
# .. math::
#
#    \widehat{\operatorname{MSE}}_{LOO}\left(\tilde{g}\right)
#    = \frac{1}{n} \sum_{i = 1}^n \left(g\left(\vect{x}^{(i)}\right) - \tilde{g}\left(\vect{x}^{(i)}\right)\right)^2
#
# The leave-one-out error is sometimes referred to as *predicted residual sum of
# squares* (PRESS) or *jacknife error*.
# In the next section, we show how this estimator can be computed analytically,
# using the hat matrix.

# %%
# The analytical leave-one-out error
# ----------------------------------
# One limitation of the previous equation is that we must train
# :math:`n` different surrogate models, which can be long in some situations.
# To overcome this limitation, we can use the following equations.
# Let :math:`\boldsymbol{\Psi} \in \Rset^{n \times m}` design matrix ([blatman2009]_ eq. 4.32 page 85):
#
# .. math::
#
#     \boldsymbol{\Psi}_{ik} = \psi_k\left(\vect{x}^{(i)}\right)
#
# for :math:`i = 1, ..., n` and :math:`i = 1, ..., m`.
# The matrix :math:`\boldsymbol{\Psi}` is mathematically equal to the
# :math:`D` matrix presented earlier in the present document.
# It can be proved that  ([blatman2009]_ eq. 4.33 page 85):
#
# .. math::
#
#    \widehat{\operatorname{MSE}}_{LOO}\left(\tilde{g}\right)
#    = \frac{1}{n} \sum_{i = 1}^n \left(\frac{g\left(\vect{x}^{(i)}\right) - \tilde{g}\left(\vect{x}^{(i)}\right)}{1 - h_i}\right)^2
#
# where :math:`h_i \in \Rset` is the diagonal of the hat matrix:
#
# .. math::
#
#     h_i = H_{ii}
#
# for :math:`i \in \{1, ..., n\}` where :math:`H \in \Rset^{n \times n}`
# is the design matrix:
#
# .. math::
#
#     H = \boldsymbol{\Psi} \left(\boldsymbol{\Psi}^T \boldsymbol{\Psi}\right) \boldsymbol{\Psi}^T
#
# The goal of this example is to show how to implement the previous equation
# using the :class:`~openturns.DesignProxy` class.

import openturns as ot
import numpy as np
from openturns.usecases import ishigami_function


# %%
# Create the polynomial chaos model
# ---------------------------------

# %%
# We load the Ishigami model.
im = ishigami_function.IshigamiModel()

# %%
# Create a training sample.

# %%
nTrain = 100
xTrain = im.distributionX.getSample(nTrain)
yTrain = im.model(xTrain)

# %%
# Create the chaos.


def ComputeSparseLeastSquaresFunctionalChaos(
    inputTrain,
    outputTrain,
    multivariateBasis,
    basisSize,
    distribution,
    sparse=True,
):
    if sparse:
        selectionAlgorithm = ot.LeastSquaresMetaModelSelectionFactory()
    else:
        selectionAlgorithm = ot.PenalizedLeastSquaresAlgorithmFactory()
    projectionStrategy = ot.LeastSquaresStrategy(
        inputTrain, outputTrain, selectionAlgorithm
    )
    adaptiveStrategy = ot.FixedStrategy(multivariateBasis, basisSize)
    chaosAlgorithm = ot.FunctionalChaosAlgorithm(
        inputTrain, outputTrain, distribution, adaptiveStrategy, projectionStrategy
    )
    chaosAlgorithm.run()
    chaosResult = chaosAlgorithm.getResult()
    return chaosResult


# %%
multivariateBasis = ot.OrthogonalProductPolynomialFactory([im.X1, im.X2, im.X3])
totalDegree = 5
enumerateFunction = multivariateBasis.getEnumerateFunction()
basisSize = enumerateFunction.getBasisSizeFromTotalDegree(totalDegree)
print("Basis size = ", basisSize)

sparse = False  # For full PCE and comparison with analytical LOO error
chaosResult = ComputeSparseLeastSquaresFunctionalChaos(
    xTrain,
    yTrain,
    multivariateBasis,
    basisSize,
    im.distributionX,
    sparse,
)

# %%
# The DesignProxy
# ---------------

# %%
# The :class:`~openturns.DesignProxy` class provides methods used to create the objects necessary to solve
# the least squares problem.
# More precisely, it provides the :meth:`~openturns.DesignProxy.computeDesign`
# method that we need to evaluate the design matrix.
# In many cases we do not need that matrix, but the Gram matrix (or its inverse).
# The :class:`~openturns.DesignProxy` class is needed by a least squares solver,
# e.g. :class:`~openturns.QRMethod` that knows how to actually compute the coefficients.
#
# Another class is the :class:`~openturns.Basis` class which manages a set of
# functions as the functional basis for the decomposition.
# This basis is required by the constructor of the :class:`~openturns.DesignProxy` because it defines
# the columns of the matrix.
#
# In order to create that basis, we use the :meth:`~openturns.FunctionalChaosResult.getReducedBasis` method,
# because the model selection (such as :class:`~openturns.LARS` for example)
# may have selected functions which best predict the output.
# This may reduce the number of coefficients to estimate and
# improve their accuracy.
# This is important here, because it defines the number of
# columns in the design matrix.

# %%
reducedBasis = chaosResult.getReducedBasis()  # As a result of the model selection
transformation = (
    chaosResult.getTransformation()
)  # As a result of the input distribution
zTrain = transformation(
    xTrain
)  # Map from the physical input into the transformed input

# %%
# We can now create the design.

# %%
designProxy = ot.DesignProxy(zTrain, reducedBasis)

# %%
# To actually evaluate the design matrix, we
# can specify the columns that we need to evaluate.
# This can be useful when we perform model selection, because
# not all columns are always needed.
# This can lead to CPU and memory savings.
# In our case, we evaluate all the columns, which corresponds
# to evaluate all the functions in the basis.

# %%
reducedBasisSize = reducedBasis.getSize()
print("Reduced basis size = ", reducedBasisSize)
allIndices = range(reducedBasisSize)
designMatrix = designProxy.computeDesign(allIndices)
print("Design matrix : ", designMatrix.getNbRows(), " x ", designMatrix.getNbColumns())

# %%
# Solve the least squares problem.
lsqMethod = ot.QRMethod(designProxy, allIndices)
betaHat = lsqMethod.solve(yTrain.asPoint())

# %%
# Compute the inverse of the Gram matrix.

# %%
inverseGram = lsqMethod.getGramInverse()
print("Inverse Gram : ", inverseGram.getNbRows(), "x", inverseGram.getNbColumns())

# %%
# Compute the raw leave-one-out error
# -----------------------------------
# In this section, we show how to compute the raw leave-one-out
# error using the naive formula.
# To do this, we could use implement the :class:~openturns.KFoldSplitter` class
# with `K = N`.
# Since this would complicate the script and obscure its purpose,
# we implement the leave-one-out method naively.

# %%
# Compute leave-one-out error
residuals = ot.Point(nTrain)
for j in range(nTrain):
    indicesLOO = list(range(nTrain))
    indicesLOO.pop(j)
    xTrainLOO = xTrain[indicesLOO]
    yTrainLOO = yTrain[indicesLOO]
    xj = xTrain[j]
    yj = yTrain[j]

    chaosResultLOO = ComputeSparseLeastSquaresFunctionalChaos(
        xTrainLOO,
        yTrainLOO,
        multivariateBasis,
        basisSize,
        im.distributionX,
        sparse,
    )
    metamodelLOO = chaosResultLOO.getMetaModel()
    yj_LOO = metamodelLOO(xj)
    residuals[j] = (yj - yj_LOO)[0]
mseLOO = residuals.normSquare() / nTrain
print("mseLOO = ", mseLOO)

# %%
# In the previous method, we must pay attention to the fact that
# the comparison that we are going to make is not necessarily
# valid if we use the `~openturns.LARS` selection method,
# because this may lead to a different active basis for each leave-one-out
# sample.
#
# One limitation of the previous script is that it can be relatively
# long when the sample size increases or when the size of the
# functional basis increases.
# In the next section, we use the analytical formula: this can leads
# to significant time savings in some cases.


# %%
# Compute the analytical leave-one-out error
# ------------------------------------------

# %%
# Get the diagonal of the projection matrix.
# This is a :class:`~openturns.Point`.

# %%
diagonalH = lsqMethod.getHDiag()
print("diagonalH : ", diagonalH.getDimension())

# %%
# Compute the metamodel predictions.

# %%
metamodel = chaosResult.getMetaModel()
yHat = metamodel(xTrain)

# %%
# Compute the residuals.

# %%
residuals = yTrain.asPoint() - yHat.asPoint()

# %%
# Compute the analytical leave-one-out error:
# perform elementwise division and exponentiation

# %%
delta = np.array(residuals) / (1.0 - np.array(diagonalH))
squaredDelta = delta**2
leaveOneOutMSE = ot.Sample.BuildFromPoint(squaredDelta).computeMean()[0]
print("MSE LOO = ", leaveOneOutMSE)
relativeLOOError = leaveOneOutMSE / yTrain.computeVariance()[0]
q2LeaveOneOut = 1.0 - relativeLOOError
print("Q2 LOO = ", q2LeaveOneOut)

# %%
# We see that the MSE leave-one-out error is equal to the naive LOO error.
# The numerical differences between the two values are the consequences
# of the rounding errors in the numerical evaluation of the hat matrix.
