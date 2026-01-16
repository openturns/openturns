"""
Create a covariance model with and without nugget effect
========================================================
"""

# %%
# This example shows the impact of a nugget effect on the covariance function of a process:
# - we create two covariance functions: the second one adds a nugget effect to the first one,
# - we draw the covariance functions.
#
# The nugget effect allows us to model noise observed in the output values of a data set. This noise may be, for
# example, measurement noise from a sensor with finite precision.
#
# See :ref:`Covariance models <covariance_model>` to get more details on covariance models. We consider a process
# :math:`\vect{X}: \Omega \times \cD \rightarrow \Rset^d` on a
# domain :math:`\cD \in \Rset^{n}`. Let :math:`C` be its
# covariance function.  The general expression of  :math:`C` is given in :eq:`def_cov`. We recall it here:
#
# .. math::
#     :label: def_cov_rho_bis
#
#     C(\vect{s}, \vect{t})  = \rho\left(\dfrac{\vect{s}}{\theta},
#                                       \dfrac{\vect{t}}{\theta}\right)\, \mat{C}^{spatial}, \quad
#                             \forall (\vect{s}, \vect{t}) \in \cD
#
# where :math:`\mat{C}^{spatial} = \mbox{Diag}(\vect{\sigma}) \, \mat{R} \,\mbox{Diag}(\vect{\sigma})` is
# the spatial correlation of the process (assumed to be independent on :math:`(\vect{s}, \vect{t})`)
# and where the correlation function :math:`\mat{\rho}(\vect{s}, \vect{t})` is supposed to be defined
# by the scalar correlation function :math:`\rho(\vect{s}, \vect{t})` by
# :math:`\mat{\rho}(\vect{s}, \vect{t}) = \rho(\vect{s}, \vect{t})\, \mat{I}_d`.
#
# The nugget effect is taken into account by modifying the scalar correlation function :math:`\rho` at
# any point :math:`(\vect{s}, \vect{s})` by adding a term denoted :math:`\sigma^2_{nugget}`:
#
#  .. math::
#      :label: cor_with_noise
#
#      \rho_{nugget}(\vect{s}, \vect{t}) &  =
#          \left |
#          \begin{array}{l}
#            \rho(\vect{s}, \vect{t}) \quad \mbox{if}  \quad \vect{s} \neq \vect{t}  \\
#            1 + \sigma^2_{nugget}
#          \end{array}
#          \right.
#
# The nugget effect transforms the covariance function :math:`C` into the covariance function :math:`C_{nugget}` as follows:
#
#  .. math::
#      :label: cov_with_noise
#
#      C_{nugget}(\vect{s}, \vect{t}) = C(\vect{s}, \vect{t}) + \sigma^2_{nugget} \mat{C}^{spatial} 1_{\vect{s} = \vect{t}}
#

# %%
import openturns as ot
import openturns.viewer as otv

# %%
# First we create a covariance function for a process defined on :math:`\cD \in \Rset` and with scalar outputs. We use the
# :class:`~openturns.SquaredExponential` with unit scale, unit amplitude. By default, the nugget factor is null.
# sphinx_gallery_thumbnail_number = 1
cov_model = ot.SquaredExponential()

# %%
# Then, we create a second covariance function wich is the first one with a nugget factor. We take :math:`\sigma_{nugget} = 1`.
cov_model_with_nugget = ot.SquaredExponential()
sigma_nugget = 1
cov_model_with_nugget.setNuggetFactor(sigma_nugget**2)

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
