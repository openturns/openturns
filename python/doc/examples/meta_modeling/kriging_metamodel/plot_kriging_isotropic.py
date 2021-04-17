"""
Kriging with an isotropic covariance function
=============================================

In typical machine learning applications, Gaussian process regression/Kriging
surrogate models take several inputs,
and those inputs are usually heterogeneous
(e.g. in the :doc:`cantilever beam
</auto_meta_modeling/kriging_metamodel/plot_kriging_cantilever_beam>` use case,
inputs are various physical quantities).

In geostatistical applications however, inputs are typically spatial
coordinates, which means one can assume the output varies at the same rate
in all directions.
This calls for a specific kind of covariance kernel, represented
in the library by the :class:`~openturns.IsotropicCovarianceModel` class.
"""

# %%
# Modeling temperature across a surface
# -------------------------------------
# In this example, we collect temperature data over a floorplan using sensors.

# %%
import numpy as np
import openturns as ot
import matplotlib.pyplot as plt
ot.Log.Show(ot.Log.NONE)

coordinates = ot.Sample([[100.0,100.0],[500.0,100.0],[900.0,100.0], \
                         [100.0,350.0],[500.0,350.0],[900.0,350.0], \
                         [100.0,600.0],[500.0,600.0],[900.0,600.0]])
observations = ot.Sample([[25.0],[25.0],[10.0],[20.0],[25.0],[20.0],[15.0],[25.0],[25.0]])

# %%
# Let us plot the data.

# Extract coordinates.
x = np.array(coordinates[:,0])
y = np.array(coordinates[:,1])

# Plot the data with a scatter plot and a color map.
fig = plt.figure()
plt.scatter(x, y, c=observations, cmap='viridis')
plt.colorbar()
plt.show()

# %%
# Because we are going to view several Kriging models in this example,
# we use a function to automate the process of optimizing the scale parameter
# and producing the metamodel.
# Since version 1.15 of the library, input data are no longer rescaled by the
# :class:`~openturns.KrigingAlgorithm` class, so we need to manually set
# sensible bounds for the scale parameter.

lower = 50.0
upper = 1000.0

def fitKriging(coordinates, observations, covarianceModel, basis):
    '''
    Fit the parameters of a Kriging metamodel.
    '''
    # Define sensible initial scale parameters
    scale_dimension = covarianceModel.getScale().getDimension()
    covarianceModel.setScale([(lower + upper) / 2] * scale_dimension)

    # Define the Kriging algorithm.
    algo = ot.KrigingAlgorithm(coordinates, observations, covarianceModel, basis)

    # Set the optimization bounds for the scale parameter
    # to sensible values given the data set.
    algo.setOptimizationBounds(ot.Interval([lower] * scale_dimension,
                                           [upper] * scale_dimension))

    # Run the Kriging algorithm and extract the fitted surrogate model.
    algo.run()
    krigingResult = algo.getResult()
    krigingMetamodel = krigingResult.getMetaModel()
    return krigingResult, krigingMetamodel

# %%
# Let us define a helper function to plot Kriging predictions.

def plotKrigingPredictions(krigingMetamodel):
    '''
    Plot the predictions of a Kriging metamodel.
    '''
    # Create the mesh of the box [0., 1000.] * [0., 700.]
    myInterval = ot.Interval([0., 0.], [1000., 700.])

    # Define the number of intervals in each direction of the box
    nx = 20
    ny = 20
    myIndices = [nx-1, ny-1]
    myMesher = ot.IntervalMesher(myIndices)
    myMeshBox = myMesher.build(myInterval)

    # Predict
    vertices = myMeshBox.getVertices()
    predictions = krigingMetamodel(vertices)

    # Format for plot
    X = np.array(vertices[:,0]).reshape((ny,nx))
    Y = np.array(vertices[:,1]).reshape((ny,nx))
    predictions_array = np.array(predictions).reshape((ny,nx))

    # Plot
    plt.figure()
    plt.pcolormesh(X, Y, predictions_array, shading='auto')
    plt.colorbar()
    plt.show()
    return

# %%
# Predict with an anisotropic geometric covariance kernel
# -------------------------------------------------------
# In order to illustrate the usefulness of isotropic covariance kernels,
# we first perform prediction with an anisotropic geometric kernel.

# %%
# Keep in mind that, when there are more than one input dimension,
# the :class:`~openturns.CovarianceModel` classes use a multidimensional
# scale parameter :math:`\vect{\theta}`.
# They are anisotropic geometric by default.
#
# Our example has two input dimensions,
# so :math:`\vect{\theta} = (\theta_1, \theta_2)`.

inputDimension = 2
basis = ot.ConstantBasisFactory(inputDimension).build()
covarianceModel = ot.SquaredExponential(inputDimension)
krigingResult, krigingMetamodel = fitKriging(coordinates, observations, covarianceModel, basis)
plotKrigingPredictions(krigingMetamodel)

# %%
# We see weird vertical columns on the plot.
# How did this happen? Let us have a look at the optimized scale parameter
# :math:`\hat{\vect{\theta}} = (\hat{\theta}_1, \hat{\theta}_2)`.

print(krigingResult.getCovarianceModel().getScale())
# %%
# The value of :math:`\hat{\theta}_1` is actually equal to the lower bound:

print(lower)

# %%
# This means that temperatures are likely to vary a lot along the X axis
# and much slower accross the Y axis based on the observation data.

# %%
# Predict with an isotropic covariance kernel
# ---------------------------------------------------
# If we know that variations of the temperature are isotropic
# (i.e. with no priviledged direction),
# we can embed this information within the covariance kernel.

isotropic = ot.IsotropicCovarianceModel(ot.SquaredExponential(), inputDimension)

# %%
# The :class:`~openturns.IsotropicCovarianceModel` class creates an isotropic
# version with a given input dimension of a :class:`~openturns.CovarianceModel`.
# Because is is isotropic, it only needs one scale parameter :math:`\theta_{iso}`
# and it will make sure :math:`\theta_1 = \theta_2 = \theta_{iso}` at all times
# during the optimization.

krigingResult, krigingMetamodel = fitKriging(coordinates, observations, isotropic, basis)
print(krigingResult.getCovarianceModel().getScale())

# %%
# Prediction with the isotropic covariance kernel is much more satisfactory.

# sphinx_gallery_thumbnail_number = 3
plotKrigingPredictions(krigingMetamodel)
