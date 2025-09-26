"""
Compare frequentist and Bayesian estimation
===========================================
"""

# %%
# In this example, we want to estimate of the parameter
# :math:`\vect{\Theta}` of the distribution of a random vector :math:`\inputRV` from which we have some data.
# We compare the frequentist and the Bayesian approaches to estimate :math:`\vect{\theta}`.
#
# Let :math:`\inputRV = (X_0, X_1)` be a random vector following a bivariate normal distribution
# with zero mean, unit variance and independent components:
# :math:`\cN_2 \left(\vect{\theta}\right)`
# where the parameter :math:`\vect{\theta} = (\mu_0, \sigma_0, \mu_1, \sigma_1, \rho) = (0, 1, 0, 1, 0)`.
# We assume to have a sample generated from :math:`\vect{X}` denoted by
# :math:`(\inputReal_1, \dots, \inputReal_\sampleSize)` where :math:`\sampleSize = 25`.
#
# We assume to know the parameters :math:`(\mu_0, \mu_1, \rho)` and we want to estimate the parameters :math:`(\sigma_0, \sigma_1)`.
# In the Bayesian approach, we assume that :math:`\vect{\Theta} = (0, \Sigma_0, 0, \Sigma_1, 0)` is a random vector and we define
# a link function :math:`g : \Rset^2 \rightarrow \Rset^5` such that:
#
# .. math::
#
#     \vect{\Theta} = g(\vect{Y})
#
# where :math:`\vect{Y}` follows the prior distribution denoted by :math:`\pi_{\vect{Y}}^0`.
#
# Note that the link function :math:`g` already contains the information that the two components of :math:`\inputRV`
# are independent (as :math:`\rho = 0`) and centered (as :math:`\mu_i = 0`).
#
# We consider two interesting link functions:
#
# .. math::
#
#   g_1(\vect{y}) & = (0, y_0, 0, y_1, 0) \\
#   g_2(\vect{y}) & = (0,0.5+y_0^2, 0, 0.5+y_1^2, 0)
#
# each one being associated to the respective prior distributions:
#
# .. math::
#
#   \pi_{\vect{Y}}^{0,1} & = \cT(0,1,2) \times \cT(0,1,2) \\
#   \pi_{\vect{Y}}^{0,2} & = \cT(-1, 0, 1) \times \cT(-1, 0,1)
#
# The second case is such that the link function :math:`g_2` is not bijective on the range of :math:`\pi_{\vect{Y}}^{0,2}`.
#
# The Bayesian approach uses the :class:`~openturns.experimental.PosteriorDistribution` that estimates the posterior distribution of :math:`\vect{Y}` denoted by
# :math:`\pi_{\vect{Y}}^\sampleSize` maximizing the likelihood of the conditioned model on the sample, weighted by the prior distribution
# :math:`\pi_{\vect{Y}}^0`. From the :math:`\pi_{\vect{Y}}^\sampleSize` distribution, we extract the vector of modes
# denoted by :math:`\vect{Y}_n^m`: this point maximizes :math:`\pi_{\vect{Y}}^\sampleSize`.
#
# It is interesting to note that when :math:`n \rightarrow +\infty`, then :math:`\pi_{\vect{Y}}^\sampleSize \rightarrow \pi_{\vect{Y}}^*`
# such that :math:`g(\pi_{\vect{Y}}^*)` is a Dirac distribution centered at :math:`\vect{\theta}`.
#
# The frequentist approach estimates the parameter :math:`\vect{\theta}` by maximizing the likelihood of the normal model on the
# sample. To model the same level of information, we consider a centered bivariate normal model with independent components.
# We denote by :math:`\vect{\theta}_n^{MC}` the parameter obtained.


# %%
import openturns as ot
import openturns.viewer as otv
import openturns.experimental as otexp

ot.ResourceMap.SetAsUnsignedInteger(
    "CompoundDistribution-MarginalIntegrationNodesNumber", 32
)
ot.ResourceMap.SetAsString(
    "CompoundDistribution-ContinuousDiscretizationMethod", "GaussProduct"
)


# %%
# Here, we define a function that computes the mode of a distribution, which is the point that maximises its PDF.


def computeMode(distribution):
    def obj_py(X):
        return distribution.computeLogPDF(X) * (-1.0)

    obj = ot.PythonFunction(distribution.getDimension(), 1, func_sample=obj_py)
    pb = ot.OptimizationProblem(obj)
    pb.setBounds(distribution.getRange())
    algo = ot.Cobyla(pb)
    algo.setStartingPoint(distribution.getMean())
    algo.run()
    return algo.getResult().getOptimalPoint()


# %%
# Define the theoretical :math:`\vect{X}` distribution which is a normal distribution with zero mean,
# unit variance and independent components.
conditioned = ot.Normal(2)
conditioned.setDescription(["X0", "X1"])

# %%
# We create the sample that we are going to use to estimate the parameters.
Nsample = 25
observations = conditioned.getSample(Nsample)


# %%
# Case 1: Bijective link function
# -------------------------------
#
# Here, :math:`g_1(\vect{y}) = (0, y_0, 0, y_1, 0)` and :math:`\pi_{\vect{Y}}^{0,1} = \cT(0,1,2) \times \cT(0,1,2)`.
#
# We create the link function.
linkFunction = ot.SymbolicFunction(["u0", "u1"], ["0.0", "u0", "0.0", "u1", "0.0"])

# %%
# We create the prior distribution of :math:`\vect{Y}`.
conditioning = ot.JointDistribution([ot.Triangular(0.0, 1.0, 2.0)] * 2)
conditioning.setDescription(["Y0", "Y1"])

# %%
# We have to decondition the :math:`\vect{X}|\vect{\Theta} = g(\vect{Y})` distribution with
# respect to the  prior distribution :math:`\pi_{\vect{Y}}^{0,1}` in order to get the
# final distribution of :math:`\vect{X}`. To do that, we use the :class:`~openturns.CompoundDistribution`.
deconditioned = ot.CompoundDistribution(conditioned, conditioning, linkFunction)

# %%
# Then, we can create the posterior distribution :math:`\pi_{\vect{Y}}^\sampleSize` based on the deconditioned distribution of :math:`\vect{X}` and
# the sample.
posterior_Y = otexp.PosteriorDistribution(deconditioned, observations)

# %%
# From  :math:`\pi_{\vect{Y}}^\sampleSize`, we get:
#
# - the mode parameter :math:`\vect{Y}_n^m` that maximizes the PDF,
# - the distribution of :math:`\inputRV`  parameterized by :math:`\vect{\theta}_n^m = g(\vect{Y}_n^m)`,
# - a bilateral condifence interval of level :math:`\alpha` of the posterior distribution of :math:`\vect{Y}`,
# - the volume of this condifence interval,
# - the Kullback-Leibler distance between the Bayesian mode based distribution and the theoretical one:
#   :math:`KL\left(\cN_2\left(g(\vect{Y}_n^m)\right), \cN_2 \left(\vect{\theta}\right) \right)`.
#
theta_Bay = linkFunction(computeMode(posterior_Y))
print("Theta Bay =", theta_Bay)
model_Bay = ot.Distribution(conditioned)
model_Bay.setParameter(theta_Bay)
dist_estimateur_Bay = posterior_Y
alpha = 0.95
interval_Bay, beta = (
    dist_estimateur_Bay.computeBilateralConfidenceIntervalWithMarginalProbability(alpha)
)
print("Beta =", beta)
print("Condifence interval Bay =\n", interval_Bay)
print("Volume =", interval_Bay.getVolume())
sample = model_Bay.getSample(1000000)
dist_Bay = (
    model_Bay.computeLogPDF(sample) - conditioned.computeLogPDF(sample)
).computeMean()
print("Kullback-Leibler distance Bay =", dist_Bay[0])


# %%
# The frequentist approach estimates a normal distribution from the sample. We fix the known parameters :math:`(\mu_0, \mu_1, \rho)`
# to their true values.
lh_factory = ot.NormalFactory()
lh_factory.setKnownParameter([0.0, 0.0, 0.0], [0, 2, 4])
lh_est = lh_factory.buildEstimator(observations)

# %%
# We extract from the likelihood estimate:
#
# - the asymptotic distribution of the estimator,
# - the parameters estimates,
# - a bilateral condifence interval of level :math:`\alpha` of the asymptotic distribution of the estimator,
# - the volume of this condifence interval,
# - the Kullback-Leibler distance between the maximum likelihood based distribution and the theoretical one:
#   :math:`KL\left(\cN_2 \left(\vect{\theta}_n^{MV} \right), \cN_2 \left(\vect{\theta}\right) \right)`.
#
model_ML = lh_est.getDistribution()
theta_ML = model_ML.getParameter()
print("Theta ML = ", theta_Bay)
dist_estimator_ML = lh_est.getParameterDistribution().getMarginal([1, 3])
interval_ML, beta = (
    dist_estimator_ML.computeBilateralConfidenceIntervalWithMarginalProbability(alpha)
)
print("Beta =", beta)
print("Condifence interval ML =\n", interval_ML)
print("Volume =", interval_ML.getVolume())
sample = model_ML.getSample(1000000)
dist_KL = (
    model_ML.computeLogPDF(sample) - conditioned.computeLogPDF(sample)
).computeMean()
print("Kullback-Leibler distance ML =", dist_KL[0])


# %%
# In the following figure, we  plot:
#
# - the theoretical distribution of :math:`\inputRV`:  :math:`\cN_2 \left(\vect{\theta}\right)` (solid lines),
# - its maximum likelihood based distribution: :math:`\cN_2 \left(\vect{\theta}_n^{MV}\right)` (dashed lines),
# - its Bayesian mode based distribution: :math:`\cN_2(g(\vect{Y}_n^m))` (dotted lines).
#
# We conclude that both approaches lead to the same results.
ot.ResourceMap.SetAsString("Contour-DefaultColorMapNorm", "rank")
g = conditioned.drawPDF()
levels = g.getDrawable(0).getLevels()
dr_ML = model_ML.drawPDF().getDrawable(0).getImplementation()
dr_ML.setLevels(levels)
dr_ML.setColorBarPosition("")
dr_ML.setLineStyle("dashed")
g.add(dr_ML)
dr_Bay = model_Bay.drawPDF().getDrawable(0).getImplementation()
dr_Bay.setLevels(levels)
dr_Bay.setColorBarPosition("")
dr_Bay.setLineStyle("dotted")
g.add(dr_Bay)
g.add(ot.Cloud(observations))
g.setLegends(["Theoretical dist", "ML dist", "Bay dist", "Observations"])
g.setXTitle(r"$X_0$")
g.setYTitle(r"$X_1$")
g.setTitle("Initial distribution, ML estimated dist and Bayesian estimated dist.")
view = otv.View(g, (800, 800), square_axes=True)

# %%
# In the following figure, we consider the parameter :math:`\vect{\Theta}` and we plot:
#
# - the asymptotic distribution of the maximum likelihood estimator: :math:`\vect{\theta}_n^{MV}` (left),
# - the Bayesian mode based distribution: :math:`g(\pi_{\vect{Y}}^\sampleSize)` (right).
#
# On each figure, we draw the bilateral confidence interval or bilateral credibility interval of
# level :math:`\alpha = 0.95`
# computed from the estimator distribution.
#
# First the maximum likelihood estimator.
g_ML = dist_estimator_ML.drawPDF([0.5] * 2, [1.5] * 2)
c = ot.Cloud([theta_ML[[1, 3]]])
c.setColor("red")
c.setPointStyle("bullet")
g_ML.add(c)
c = ot.Cloud([theta_Bay[[1, 3]]])
c.setColor("red")
c.setPointStyle("+")
g_ML.add(c)
a = interval_ML.getLowerBound()
b = interval_ML.getUpperBound()
c = ot.Curve([a, [a[0], b[1]], b, [b[0], a[1]], a])
c.setColor("blue")
g_ML.add(c)
a = interval_Bay.getLowerBound()
b = interval_Bay.getUpperBound()
c = ot.Curve([a, [a[0], b[1]], b, [b[0], a[1]], a])
c.setColor("blue")
c.setLineStyle("dashed")
g_ML.add(c)
g_ML.setLegends(
    [
        r"dist of $\mathbf{\theta}_n^{MV}$",
        r"$\mathbf{\theta}_n^{MV}$",
        r"$g(\mathbf{Y}_n^m)$",
        "ML CI(" + str(int(100 * alpha)) + "%)",
        "Bay CI(" + str(int(100 * alpha)) + "%)",
    ]
)
g_ML.setXTitle(r"$\sigma_0$")
g_ML.setYTitle(r"$\sigma_1$")
g_ML.setTitle("ML Estimator")

# %%
# Then the Bayesian estimator.
g_Bay = dist_estimateur_Bay.drawPDF([0.5] * 2, [1.5] * 2)
c = ot.Cloud([theta_Bay[[1, 3]]])
c.setColor("red")
c.setPointStyle("bullet")
g_Bay.add(c)
c = ot.Cloud([theta_ML[[1, 3]]])
c.setColor("red")
c.setPointStyle("+")
g_Bay.add(c)
a = interval_Bay.getLowerBound()
b = interval_Bay.getUpperBound()
c = ot.Curve([a, [a[0], b[1]], b, [b[0], a[1]], a])
c.setColor("blue")
g_Bay.add(c)
a = interval_ML.getLowerBound()
b = interval_ML.getUpperBound()
c = ot.Curve([a, [a[0], b[1]], b, [b[0], a[1]], a])
c.setColor("blue")
c.setLineStyle("dashed")
g_Bay.add(c)
g_Bay.setLegends(
    [
        r"dist of $g(\pi_{n, \mathbf{Y}})$",
        r"$g(\mathbf{Y}_n^m)$",
        r"$\mathbf{\theta}_n^{MV}$",
        "Bay CI(" + str(int(100 * alpha)) + "%)",
        "ML CI(" + str(int(100 * alpha)) + "%)",
    ]
)
g_Bay.setXTitle(r"$\sigma_0$")
g.setYTitle(r"$\sigma_1$")
g_Bay.setTitle("Bayesian Estimator")

# Gather both graph in a grid.
grid = ot.GridLayout(1, 2)
grid.setGraph(0, 0, g_ML)
grid.setGraph(0, 1, g_Bay)

view = otv.View(grid)

# %%
# Finally, the following table sums up the previous computed quantities.
#
# ===========      ========================  ========================  =====================    =====================
# Approach         :math:`\tilde{\sigma}_0`  :math:`\tilde{\sigma}_1`  IC(:math:`\sigma_0`)     IC(:math:`\sigma_1`)
# ===========      ========================  ========================  =====================    =====================
# Frequentist      0.967                     0.951                     :math:`[0.651, 1.30]`    :math:`[0.659, 1.20]`
# Bayesian         0.950                     0.934                     :math:`[0.736, 1.38]`    :math:`[0.724, 1.35]`
# ===========      ========================  ========================  =====================    =====================
#
# ===========      =============   ======================   =================================================
# Approach         :math:`\beta`   :math:`KL`               :math:`\cV(IC(\tilde{\sigma}_0, \tilde{\sigma}))`
# ===========      =============   ======================   =================================================
# Frequentist      0.974           :math:`3.46\, 10^{-3}`   0.352
# Bayesian         0.974           :math:`6.87\, 10^{-3}`   0.381
# ===========      =============   ======================   =================================================


# %%
# Case 2: we consider the second link function
# --------------------------------------------
#
# Here, :math:`g_2(\vect{y}) = (0,0.5+y_0^2, 0, 0.5+y_1^2, 0)` and :math:`\pi_{\vect{Y}}^{0,2} = \cT(-1, 0, 1) \times \cT(-1, 0,1)`.
#
# We note that the posterior distribution of :math:`\vect{Y}` is quadri-modal, as the link function :math:`g_2` is no more bijective
# on the range of :math:`\pi_{\vect{Y}}^{0,2}`.
#
# We go through the same steps as described previously. The maximum estimator is not changed.
# But in order to get the posterior distribution of :math:
linkFunction = ot.SymbolicFunction(
    ["u0", "u1"], ["0.0", "0.5+u0^2", "0.0", "0.5+u1^2", "0.0"]
)
conditioning = ot.JointDistribution([ot.Triangular(-1.0, 0.0, 1.0)] * 2)
conditioning.setDescription(["Y0", "Y1"])
deconditioned = ot.CompoundDistribution(conditioned, conditioning, linkFunction)
posterior_Y = otexp.PosteriorDistribution(deconditioned, observations)
sample_posterior = linkFunction(posterior_Y.getSample(100000)).getMarginal([1, 3])
dist_estimateur_Bay = ot.KernelSmoothing().build(sample_posterior)

theta_Bay = linkFunction(computeMode(posterior_Y))
print("Theta Bay =", theta_Bay)
model_Bay = ot.Distribution(conditioned)
model_Bay.setParameter(theta_Bay)
alpha = 0.95
interval_Bay, beta = (
    dist_estimateur_Bay.computeBilateralConfidenceIntervalWithMarginalProbability(alpha)
)
print("Beta =", beta)
print("Condifence interval Bay =\n", interval_Bay)
print("Volume =", interval_Bay.getVolume())
sample = model_Bay.getSample(1000000)
dist_Bay = (
    model_Bay.computeLogPDF(sample) - conditioned.computeLogPDF(sample)
).computeMean()
print("Kullback-Leibler distance Bay =", dist_Bay[0])

# %%
g = conditioned.drawPDF()
levels = g.getDrawable(0).getLevels()
dr_ML = model_ML.drawPDF().getDrawable(0).getImplementation()
dr_ML.setLevels(levels)
dr_ML.setColorBarPosition("")
dr_ML.setLineStyle("dashed")
g.add(dr_ML)
dr_Bay = model_Bay.drawPDF().getDrawable(0).getImplementation()
dr_Bay.setLevels(levels)
dr_Bay.setColorBarPosition("")
dr_Bay.setLineStyle("dotted")
g.add(dr_Bay)
g.add(ot.Cloud(observations))
g.setLegends(["Theoretical dist", "ML dist", "Bay dist", "Observations"])
g.setXTitle(r"$X_0$")
g.setYTitle(r"$X_1$")
g.setTitle("Initial distribution, ML estimated dist and Bayesian estimated dist.")
view = otv.View(g, (800, 800), square_axes=True)

# %%
g_ML = dist_estimator_ML.drawPDF([0.5] * 2, [1.5] * 2)
c = ot.Cloud([theta_ML[[1, 3]]])
c.setColor("red")
c.setPointStyle("bullet")
g_ML.add(c)
c = ot.Cloud([theta_Bay[[1, 3]]])
c.setColor("red")
c.setPointStyle("+")
g_ML.add(c)
a = interval_ML.getLowerBound()
b = interval_ML.getUpperBound()
c = ot.Curve([a, [a[0], b[1]], b, [b[0], a[1]], a])
c.setColor("blue")
g_ML.add(c)
a = interval_Bay.getLowerBound()
b = interval_Bay.getUpperBound()
c = ot.Curve([a, [a[0], b[1]], b, [b[0], a[1]], a])
c.setColor("blue")
c.setLineStyle("dashed")
g_ML.add(c)
g_ML.setLegends(
    [
        r"dist of $\mathbf{\theta}_n^{MV}$",
        r"$\mathbf{\theta}_n^{MV}$",
        r"$g(\mathbf{Y}_n^m)$",
        "ML CI(" + str(int(100 * alpha)) + "%)",
        "Bay CI(" + str(int(100 * alpha)) + "%)",
    ]
)
g_ML.setXTitle(r"$\sigma_0$")
g_ML.setYTitle(r"$\sigma_1$")
g_ML.setTitle("ML Estimator")


g_Bay = dist_estimateur_Bay.drawPDF([0.5] * 2, [1.5] * 2)
c = ot.Cloud([theta_Bay[[1, 3]]])
c.setColor("red")
c.setPointStyle("bullet")
g_Bay.add(c)
c = ot.Cloud([theta_ML[[1, 3]]])
c.setColor("red")
c.setPointStyle("+")
g_Bay.add(c)
a = interval_Bay.getLowerBound()
b = interval_Bay.getUpperBound()
c = ot.Curve([a, [a[0], b[1]], b, [b[0], a[1]], a])
c.setColor("blue")
g_Bay.add(c)
a = interval_ML.getLowerBound()
b = interval_ML.getUpperBound()
c = ot.Curve([a, [a[0], b[1]], b, [b[0], a[1]], a])
c.setColor("blue")
c.setLineStyle("dashed")
g_Bay.add(c)
g_Bay.setLegends(
    [
        r"dist of $g(\pi_{n, \mathbf{Y}})$",
        r"$g(\mathbf{Y}_n^m)$",
        r"$\mathbf{\theta}_n^{MV}$",
        "Bay CI(" + str(int(100 * alpha)) + "%)",
        "ML CI(" + str(int(100 * alpha)) + "%)",
    ]
)
g_Bay.setXTitle(r"$\sigma_0$")
g.setYTitle(r"$\sigma_1$")
g_Bay.setTitle("Bayesian Estimator")

grid = ot.GridLayout(1, 2)
grid.setGraph(0, 0, g_ML)
grid.setGraph(0, 1, g_Bay)

view = otv.View(grid)

# %%
# Finally, the following table sums up the previous computed quantities.
#
# ===========      ========================  ========================  =====================    =====================
# Approach         :math:`\tilde{\sigma}_0`  :math:`\tilde{\sigma}_1`  IC(:math:`\sigma_0`)     IC(:math:`\sigma_1`)
# ===========      ========================  ========================  =====================    =====================
# Frequentist      0.967                     0.951                     :math:`[0.651, 1.30]`    :math:`[0.659, 1.20]`
# Bayesian         0.929                     0.915                     :math:`[0.706, 1.27]`    :math:`[0.693, 1.25]`
# ===========      ========================  ========================  =====================    =====================
#
# ===========      =============   ======================   =================================================
# Approach         :math:`\beta`   :math:`KL`               :math:`\cV(IC(\tilde{\sigma}_0, \tilde{\sigma}))`
# ===========      =============   ======================   =================================================
# Frequentist      0.974           :math:`3.46\, 10^{-3}`   0.352
# Bayesian         0.974           :math:`1.21\, 10^{-2}`   0.316
# ===========      =============   ======================   =================================================
#
# We also plot the PDF of the posterior distribution :math:`\pi_{\vect{Y}}^\sampleSize` of :math:`\vect{Y}` which is quadri-modal, with a sample.
# sphinx_gallery_thumbnail_number =  5
g_pinY = posterior_Y.drawPDF()
g_pinY.setXTitle(r"$Y_0$")
g_pinY.setYTitle(r"$Y_1$")
g_pinY.setTitle(r"Posterior Bayesian Distribution of $\mathbf{Y}$")
g_pinY.add(ot.Cloud(posterior_Y.getSample(100)))
view = otv.View(g_pinY, (800, 800))

# %%
view.ShowAll()
