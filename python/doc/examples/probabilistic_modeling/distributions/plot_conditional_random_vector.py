"""
Create a deconditioned random vector
====================================

# %%
# In this example we are going to build the random vector :math:`\inputRV` defined as the
# deconditioned distribution of:
#
# .. math::
#
#    \inputRV|\vect{\Theta}
#
# with respect to parameter random vector :math:`\vect{\Theta}` following the distribution :math:`\cL_{\vect{\Theta}}`.
#
# This example creates a :class:`~openturns.ConditionalRandomVector`. Remember that a :class:`~openturns.ConditionalRandomVector`
# (and more generally a :class:`~openturns.RandomVector`) can only be sampled.
#
# There is no restriction on the random vector :math:`\vect{\Theta}`. It can be created from any multivariate distribution or
# as the output of a function :math:`f` applied to an input random vector :math:`\vect{Y}`: :math:`\vect{\Theta} = f(\vect{Y})`.
#
# Note that in some restricted cases, it is possible to create the
# distribution of :math:`\inputRV` using the class :class:`~openturns.DeconditionedDistribution`.
# The :class:`~openturns.DeconditionedDistribution` offers a lot of methods attached to the distribution, in particular the
# computation of the PDF, CDF, the moments if any, :math:`\dots`. The advantage of the :class:`~openturns.ConditionalRandomVector` relies
# on the fact that it is fast to build and can be built in all cases. But it only offers the sampling capacity.
#
# We consider the following cases:
#
# - Case 1: the parameter random vector has continuous marginals,
# - Case 2: the parameter random vector has dependent continuous and discrete marginals,
# - Case 3: the parameter random vector has any dependent marginals.

# %%
import openturns as ot
import openturns.viewer as otv

# %%
# Case 1: the parameter random vector  has continuous marginals
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#
# We consider the case where :math:`X` is of dimension 1 and follows an exponential distribution
# defined by:
#
# ================================ =========================================================  ====================================
# Variable                         Distribution                                               Parameter
# ================================ =========================================================  ====================================
# :math:`X`                        :class:`~openturns.Exponential` (:math:`\Lambda, \Gamma`)  (:math:`\Lambda, \Gamma`) are random
# :math:`\Lambda`                  :class:`~openturns.Uniform` (:math:`a, b`)                 :math:`(a,b) = (0, 1)`
# :math:`\Gamma`                   :class:`~openturns.Uniform` (:math:`a, b`)                 :math:`(a,b) = (1, 2)`
# Copula (:math:`\Lambda, \Gamma`) :class:`~openturns.ClaytonCopula` (:math:`\theta`)         :math:`\theta = 2`
# ================================ =========================================================  ====================================
#

# %%
# Create the parameter random vector :math:`\vect{\Theta} = (\Lambda, \Gamma)`:
lambdaDist = ot.Uniform(0.0, 1.0)
gammaDist = ot.Uniform(1.0, 2.0)
thetaDist = ot.JointDistribution([lambdaDist, gammaDist], ot.ClaytonCopula(2))
thetaRV = ot.RandomVector(thetaDist)

# %%
# Create the :math:`\inputRV|\vect{\Theta}` distribution: as the parameters have no importance, we
# create the default distribution.
XgivenThetaDist = ot.Exponential()

# %%
# Create the :math:`\vect X` distribution
XDist = ot.DeconditionedRandomVector(XgivenThetaDist, thetaRV)

# %%
# Draw a sample
X_RV.getSample(5)

# %%
# If we generate a large sample of the random vector, we can fit its distribution with non-parametric techniques
# such as a kernel smoothing.
sample_large = X_RV.getSample(10000)
dist_KS = ot.KernelSmoothing().build(sample_large)
g_PDF = dist_KS.drawPDF()
g_PDF.setTitle("Case 1: PDF of X by kernel smoothing")
g_PDF.setXTitle("x")
g_PDF.setLegendPosition("")
view = otv.View(g_PDF)

# %%
# Case 2: the parameter random vector has dependent continuous and discrete marginals
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#
# We consider the case where :math:`X` is of dimension 1 and follows an exponential distribution
# defined by:
#
# =================================  =========================================================  ====================================
# Variable                           Distribution                                               Parameter
# =================================  =========================================================  ====================================
# :math:`X`                          :class:`~openturns.Exponential` (:math:`\Lambda, \Gamma`)  (:math:`\Lambda, \Gamma`) are random
# :math:`\Lambda`                    1 + :class:`~openturns.Poisson` (:math:`\ell`)             :math:`\ell = 1`
# :math:`\Gamma`                     :class:`~openturns.Uniform` (:math:`a, b`)                 :math:`(a,b) = (1, 2)`
# Copula  (:math:`\Lambda, \Gamma`)  :class:`~openturns.ClaytonCopula` (:math:`\theta`)         :math:`\theta = 2`
# =================================  =========================================================  ====================================
#

# %%
# Create the parameter random vector :math:`\vect{\Theta} = (\Lambda, \Gamma)`. We shift the Poisson distribution to
# get positive values for :math:`\Lambda`.
lambdaDist = 1 + ot.Poisson(1)
gammaDist = ot.Uniform(1.0, 2.0)
thetaDist = ot.JointDistribution([lambdaDist, gammaDist], ot.ClaytonCopula(2))
thetaRV = ot.RandomVector(thetaDist)

# %%
# Create the :math:`\inputRV|\vect{\Theta}` random vector.
XgivenThetaDist = ot.Exponential()
X_RV = ot.ConditionalRandomVector(XgivenThetaDist, thetaRV)

# %%
# If we generate a large sample of the random vector, we can fit its distribution with non-parametric techniques
# such as a kernel smoothing.
sample_large = X_RV.getSample(10000)
dist_KS = ot.KernelSmoothing().build(sample_large)
g_PDF = dist_KS.drawPDF()
g_PDF.setTitle("Case 2: PDF of X by kernel smoothing")
g_PDF.setXTitle("x")
g_PDF.setLegendPosition("")
view = otv.View(g_PDF)

# %%
# Case 3: the parameter random vector has any dependent marginals
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#
# We consider the case where :math:`X` is of dimension 1 and follows an exponential distribution
# defined by:
#
# =================================  =========================================================  ====================================
# Variable         Distribution                                               Parameter
# =================================  =========================================================  ====================================
# :math:`X`                          :class:`~openturns.Exponential` (:math:`\Lambda, \Gamma`)  (:math:`\Lambda, \Gamma`) are random
# :math:`\Lambda`                    :class:`~openturns.Mixture`                                see below
# :math:`\Gamma`                     :class:`~openturns.Uniform` (:math:`a, b`)                 :math:`(a,b) = (1, 2)`
# Copula  (:math:`\Lambda, \Gamma`)  :class:`~openturns.ClaytonCopula` (:math:`\theta`)         :math:`\theta = 2`
# =================================  =========================================================  ====================================
#
# where the mixture is built from the :class:`~openturns.Exponential` (:math:`\ell`) with :math:`\ell = 1` and the
# :class:`~openturns.Geometric` (:math:`p`) with :math:`p = 0.1`, with equal weights. In this case, the distribution of :math:`\Lambda` is
# not discrete nor continuous.

# %%
# Create the parameter random vector :math:`\vect{\Theta} = (\Lambda, \Gamma)`:
lambdaDist = ot.Mixture([ot.Exponential(1.0), ot.Geometric(0.1)])
gammaDist = ot.Uniform(1.0, 2.0)
thetaDist = ot.JointDistribution([lambdaDist, gammaDist], ot.ClaytonCopula(2))
thetaRV = ot.RandomVector(thetaDist)

# %%
# Create the :math:`\inputRV|\vect{\Theta}` random vector.
XgivenThetaDist = ot.Exponential()
X_RV = ot.ConditionalRandomVector(XgivenThetaDist, thetaRV)

# %%
# If we generate a large sample of the random vector, we can fit its distribution with non-parametric techniques
# such as a kernel smoothing.
sample_large = X_RV.getSample(10000)
dist_KS = ot.KernelSmoothing().build(sample_large)
g_PDF = dist_KS.drawPDF()
g_PDF.setTitle("Case 3: PDF of X by kernel smoothing")
g_PDF.setXTitle("x")
g_PDF.setLegendPosition("")
view = otv.View(g_PDF)

# %%
view.ShowAll()
