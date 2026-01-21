"""
Create a covariance model with and without nugget effect
========================================================
"""

# %%
# This example shows the impact of a nugget effect on the covariance function of a process:
# - we create two covariance functions: the second one adds a nugget effect to the first one,
# - we draw the covariance functions.
#
# The nugget effect allows us to model a noise observed in the output values of a process. This noise may be, for
# example, a measurement noise from a sensor with finite precision.
#
# The noise is characterized by the scalar
# :math:`\varepsilon_{nugget}` which is added to the correlation function evaluated on each :math:`(\vect{s}, \vect{s})` where
# :math:`\vect{s} \in \cD`. See :ref:`Covariance models <covariance_model>` to get more details on covariance models and the introduction of a nugget factor, anbd in particular see equation :eq:`Css`.

# %%
import openturns as ot
import openturns.viewer as otv

# %%
# First we create a covariance function for a process defined on :math:`\cD \in \Rset` and with scalar outputs. We use the
# :class:`~openturns.SquaredExponential` with unit scale, unit amplitude. By default, the nugget factor is equal to
# :math:`10^{-12}` wich allows the regularization of the ocnvariance matrix. We set it to zero.
# sphinx_gallery_thumbnail_number = 1
cov_model = ot.SquaredExponential()
cov_model.setNuggetFactor(0.0)

# %%
# Then, we create a second covariance function which is the first one with a nugget factor.
# We take :math:`\varepsilon_{nugget} = 1`.
cov_model_with_nugget = ot.SquaredExponential()
epsilon_nugget = 1
cov_model_with_nugget.setNuggetFactor(epsilon_nugget)

# %%
# We draw the covariance function with and without the nugget effect.
g = cov_model.draw()
g.setTitle('Without nugget effect')
g_nugget = cov_model_with_nugget.draw()
g_nugget.setTitle('With nugget effect')

grid = ot.GridLayout(1, 2)
grid.setGraph(0, 0, g)
grid.setGraph(0, 1, g_nugget)
view = otv.View(grid)
otv.View.ShowAll()


# %%
# Display all figures
otv.View.ShowAll()
