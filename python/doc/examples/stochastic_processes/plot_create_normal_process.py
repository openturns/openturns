"""
Create a Gaussian process including a nugget effect
===================================================
"""

# %%
# This example shows how to create a Gaussian process:
#
# - from its covariance function,
# - from its covariance function including a nugget effect,
# - from its spectral density in the stationary case.

# %%
# sphinx_gallery_thumbnail_number = 2
import openturns as ot
import openturns.viewer as otv

# %%
# Create a Gaussian process from its covariance model
# ---------------------------------------------------
#
# Here, we create a Gaussian process :math:`\vect{X}: \Omega \times \cD \rightarrow \Rset` on a
# domain :math:`\cD \in \Rset` from its covariance model.
#
# See :ref:`Covariance models <covariance_model>` to get more details on covariance models.
#
# We first define a covariance model. We use a :class:`~openturns.MaternModel` model. By default, the nugget factor
# is equal to :math:`10^{-12}`. That is why we set it to zero.
amplitude = [1.0]
scale = [1.0]
myModel = ot.MaternModel(scale, amplitude, 2.5)
myModel.setNuggetFactor(0.0)

# %%
# We define a mesh that discretizes the domain :math:`\cD`. We use a  :class:`~openturns.RegularGrid`.
tmin = 0.0
N = 200
tmax = 5
step = (tmin - tmax) / (N - 1)
myTimeGrid = ot.RegularGrid(tmin, step, N)

# %%
# We create the Gaussian process from the covariance model and the mesh.
process = ot.GaussianProcess(myModel, myTimeGrid)
print(process)

# %%
# We draw some realizations of the Gaussian process.
n_real = 3
sample = process.getSample(n_real)
graph = sample.drawMarginal(0)
graph.setTitle("Processs realizations")
view = otv.View(graph)


# %%
# Add a nugget effect to the Gaussian process
# -------------------------------------------
#
# Here, we add a nugget effect to the Gaussian process. We use the previous Gaussian process. We add a nugget
# factor
# :math:`\varepsilon_{nugget}`.
#
# Refer to :ref:`Covariance models <covariance_model>` to get more details on covariance models and the
# introduction of a
# nugget factor, and in particular see equation :eq:`Css`.
#
# Refer also to the example :doc:`/auto_stochastic_processes/plot_covariance_model_nugget` which illustrates
# the impact of the nugget effect on a covariance model.
#
# Adding a nugget factor modifies the covariance model. It transforms the process by adding a white noise
# of dimension :math:`d` with zero mean and a covariance matrix equal to
# :math:`\varepsilon_{nugget} \mat{C}^{spatial}`:
#
# .. math::
#
#   \vect{X}_{nugget}(\omega, \vect{t}) = \vect{X}(\omega, \vect{t}) +
#   \vect{\varepsilon}(\omega), \quad \vect{\varepsilon} \sim \cN(\vect{0}, \sigma_{nugget}^2 =
#   \varepsilon_{nugget} \mat{C}^{spatial})
#
# We fix :math:`\varepsilon_{nugget} = 0.05`.
epsilon_nugget = 0.05
myModel.setNuggetFactor(epsilon_nugget)
process_nugget = ot.GaussianProcess(myModel, myTimeGrid)
print(process_nugget)

# %%
# We draw some realizations of the Gaussian process. We notice that the
# realizations of the process with the nugget factor are more chaotic than the other ones.
sample_nugget = process_nugget.getSample(n_real)
graph_nugget = sample_nugget.drawMarginal(0)
graph_nugget.setTitle("Processs realizations with nugget effect")
view = otv.View(graph_nugget)

# %%
# Create a Gaussian process from its spectral density
# ---------------------------------------------------
#
# Here, we create a Gaussian process :math:`\vect{X}: \Omega \times \cD \rightarrow \Rset` on a
# domain :math:`\cD \in \Rset` from its spectral density.
#
# We first define a spectral model. We use a :class:`~openturns.CauchyModel`.
amplitude = [1.0]
scale = [4.0]
mySpectralModel = ot.CauchyModel(scale, amplitude)

# %%
# We define the associated mesh which is a :class:`~openturns.RegularGrid`.
myTimeGrid = ot.RegularGrid(0.0, 0.1, 20)

# %%
# We create the Gaussian process using a :class:`~openturns.SpectralGaussianProcess`.
process = ot.SpectralGaussianProcess(mySpectralModel, myTimeGrid)
print(process)

# %%
# Eventually we draw some realizations of the Gaussian process.
sample = process.getSample(n_real)
graph = sample.drawMarginal(0)
graph.setTitle("Processs realizations")
view = otv.View(graph)

# %%
# Display all figures.
otv.View.ShowAll()
