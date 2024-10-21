"""
Estimate a GEV on the Fremantle sea-levels data
===============================================
"""

# %%
# In this example, we illustrate various techniques of extreme value modeling applied
# to the annual maximum sea-levels recorded at Fremantle, near Perth, western Australia, over the period
# 1897-1989.
# Readers should refer to [coles2001]_ to get more details.
#
# We illustrate techniques to:
#
# - estimate a stationary and a non stationary GEV depending on time or on the covariates (time, SOI),
# - estimate a return level,
#
# using:
#
# - the log-likelihood function,
# - the profile log-likelihood function.
#
# We also illustrate the modelling with covariates.
#
# First, we load the Fremantle dataset of the annual maximum sea-levels. We start by looking at them
# through time. The data also contain the annual mean value of the Southern Oscillation Index (SOI),
# which is a proxy for meteorological volatility due to effects such as El Nino.
import openturns as ot
import openturns.viewer as otv
from openturns.usecases import coles

data = coles.Coles().fremantle
print(data[:5])
graph = ot.Graph(
    "Annual maximum sea-levels at Fremantle", "year", "level (m)", True, ""
)
cloud = ot.Cloud(data[:, :2])
cloud.setColor("red")
graph.add(cloud)
graph.setIntegerXTick(True)
view = otv.View(graph)

# %%
# We select the sea-levels column.
sample = data[:, 1]

# %%
# **Stationary GEV modeling via the log-likelihood function**
#
# We first assume that the dependence through time is negligible, so we first model the data as
# independent observations over the observation period. We estimate the parameters of the
# GEV distribution by maximizing the log-likelihood of the data.
factory = ot.GeneralizedExtremeValueFactory()
result_LL = factory.buildMethodOfLikelihoodMaximizationEstimator(sample)

# %%
# We get the fitted GEV and its parameters :math:`(\hat{\mu}, \hat{\sigma}, \hat{\xi})`.
fitted_GEV = result_LL.getDistribution()
desc = fitted_GEV.getParameterDescription()
param = fitted_GEV.getParameter()
print(", ".join([f"{p}: {value:.3f}" for p, value in zip(desc, param)]))

# %%
# We get the asymptotic distribution of the estimator :math:`(\hat{\mu}, \hat{\sigma}, \hat{\xi})`.
# In that case, the asymptotic distribution is normal.
parameterEstimate = result_LL.getParameterDistribution()
print("Asymptotic distribution of the estimator : ")
print(parameterEstimate)

# %%
# We get the covariance matrix  and the standard deviation of :math:`(\hat{\mu}, \hat{\sigma}, \hat{\xi})`.
print("Cov matrix = \n", parameterEstimate.getCovariance())
print("Standard dev = ", parameterEstimate.getStandardDeviation())

# %%
# We get the marginal confidence intervals of order 0.95.
order = 0.95
for i in range(3):
    ci = parameterEstimate.getMarginal(i).computeBilateralConfidenceInterval(order)
    print(desc[i] + ":", ci)

# %%
# At last, we can validate the inference result thanks the 4 usual diagnostic plots:
#
# - the probability-probability pot,
# - the quantile-quantile pot,
# - the return level plot,
# - the empirical distribution function.
validation = ot.GeneralizedExtremeValueValidation(result_LL, sample)
graph = validation.drawDiagnosticPlot()
view = otv.View(graph)

# %%
# **Stationary GEV modeling via the profile log-likelihood function**
#
# Now, we use the profile log-likehood function rather than log-likehood function  to estimate the parameters of the GEV.
result_PLL = factory.buildMethodOfXiProfileLikelihoodEstimator(sample)

# %%
# The following graph allows one to get the profile log-likelihood plot.
# It also indicates the optimal value of :math:`\xi`, the maximum profile log-likelihood and
# the confidence interval for :math:`\xi` of order 0.95 (which is the default value).
order = 0.95
result_PLL.setConfidenceLevel(order)
view = otv.View(result_PLL.drawProfileLikelihoodFunction())

# %%
# We can get the numerical values of the confidence interval: it appears to be a bit smaller
# than the interval obtained with the log-likelihood function.
# Note that if the order requested is too high, the confidence interval might not be calculated because
# one of its bound is out of the definition domain of the log-likelihood function.
try:
    print("Confidence interval for xi = ", result_PLL.getParameterConfidenceInterval())
except Exception as ex:
    print(type(ex))
    pass

# %%
# **Return level estimate from the estimated stationary GEV**
#
# We estimate the :math:`m`-block return level :math:`z_m`: it is computed as a particular quantile of the
# GEV model estimated using the log-likelihood function. We just have to use the maximum log-likelihood
# estimator built in the previous section.
#
# As the data are annual sea-levels, each block corresponds to one year: the 10-year return level
# corresponds to :math:`m=10` and the 100-year return level corresponds to :math:`m=100`.
#
# The method provides the asymptotic distribution of the estimator :math:`\hat{z}_m`
# which mean is the return-level estimate.
zm_10 = factory.buildReturnLevelEstimator(result_LL, 10.0)
return_level_10 = zm_10.getMean()
print("Maximum log-likelihood function : ")
print(f"10-year return level = {return_level_10}")
return_level_ci10 = zm_10.computeBilateralConfidenceInterval(0.95)
print(f"CI = {return_level_ci10}")

# %%
zm_100 = factory.buildReturnLevelEstimator(result_LL, 100.0)
return_level_100 = zm_100.getMean()
print(f"100-year return level = {return_level_100}")
return_level_ci100 = zm_100.computeBilateralConfidenceInterval(0.95)
print(f"CI = {return_level_ci100}")

# %%
# **Return level estimate via the profile log-likelihood function of a stationary GEV**
#
# We can estimate the :math:`m`-block return level :math:`z_m` directly from the data using the profile
# likelihood with respect to :math:`z_m`.
result_zm_10_PLL = factory.buildReturnLevelProfileLikelihoodEstimator(sample, 10.0)
zm_10_PLL = result_zm_10_PLL.getParameter()
print(f"10-year return level (profile) = {zm_10_PLL}")

# %%
# We can get the confidence interval of :math:`z_m`:  once more, it appears to be a bit smaller
# than the interval obtained from the log-likelihood function.
# As for the confidence interval of :math:`\xi`, depending on the order requested, the interval might
# not be calculated.
result_zm_10_PLL.setConfidenceLevel(0.95)
try:
    return_level_ci10 = result_zm_10_PLL.getParameterConfidenceInterval()
except Exception as ex:
    print(type(ex))
    pass
print("Maximum profile log-likelihood function : ")
print(f"CI={return_level_ci10}")

# %%
# We can also plot the profile log-likelihood function and get the confidence interval, the optimal value
# of :math:`z_m` and its confidence interval.
view = otv.View(result_zm_10_PLL.drawProfileLikelihoodFunction())

# %%
# **Non stationary GEV modeling via the log-likelihood function**
#
# If we look at the data carefully, we see that the pattern of variation has not remained constant over
# the observation period. There is an increase in the data through time.
# We want to model this dependence because a slight increase in extreme sea-levels might have
# a significant impact on the safety of coastal flood defenses.
#
# We have define the functional basis for each parameter of the GEV model. Even if we have
# the possibility to affect a time-varying model to each of the 3 parameters :math:`(\mu, \sigma, \xi)`,
# it is strongly recommended not to vary the parameter :math:`\xi` and to let it constant.
#
# For numerical reasons, it is strongly recommended to normalize all the data as follows:
#
# .. math::
#
#     \tau(t) = \dfrac{t-c}{d}
#
# where:
#
# - the *CenterReduce* method where :math:`c = \dfrac{1}{n} \sum_{i=1}^n t_i` is the mean time stamps
#   and :math:`d = \sqrt{\dfrac{1}{n} \sum_{i=1}^n (t_i-c)^2}` is the standard deviation of the time stamps;
# - the *MinMax* method where :math:`c = t_1` is the initial time and :math:`d = t_n-t_1` the final time;
# - the *None* method where :math:`c = 0` and :math:`d = 1`: in that case, data are not normalized.
#
# We suppose that :math:`\mu` is linear in time, and that the other parameters remain constant:
#
# .. math::
#     :nowrap:
#
#     \begin{align*}
#       \mu(t) & = \beta_1 + \beta_2\tau(t) \\
#       \sigma(t) & = \beta_3 \\
#       \xi(t) & = \beta_4
#     \end{align*}
#
constant = ot.SymbolicFunction(["t"], ["1.0"])
basis = ot.Basis([constant, ot.SymbolicFunction(["t"], ["t"])])
# basis for mu, sigma, xi
muIndices = [0, 1]  # linear
sigmaIndices = [0]  # stationary
xiIndices = [0]  # stationary

# %% We need to get the time stamps (in years here).
timeStamps = data[:, 0]

# %%
# We can now estimate the list of coefficients :math:`\vect{\beta} = (\beta_1, \beta_2, \beta_3, \beta_4)`
# using the log-likelihood of the data.
#
# We test the 3 normalizing methods and both initial points in order to evaluate their impact on the results.
# We can see that:
#
# - both normalization methods lead to the same result for :math:`\beta_1`, :math:`\beta_3` and :math:`\beta_4`
#   (note that :math:`\beta_2` depends on the normalization function),
# - both initial points lead to the same result when the data have been normalized,
# - it is very important to normalize all the data: if not, the result strongly depends on the initial point
#   and it differs from the result obtained with normalized data. The results are not optimal in that case
#   since the associated log-likelihood are much smaller than those obtained with normalized data.
#
print("Linear mu(t) model:")
for normMeth in ["MinMax", "CenterReduce", "None"]:
    for initPoint in ["Gumbel", "Static"]:
        print(f"normMeth = {normMeth}, initPoint = {initPoint}")
        # The ot.Function() is the identity function.
        result = factory.buildTimeVarying(
            sample,
            timeStamps,
            basis,
            muIndices,
            sigmaIndices,
            xiIndices,
            ot.Function(),
            ot.Function(),
            ot.Function(),
            initPoint,
            normMeth,
        )
        beta = result.getOptimalParameter()
        print(f"beta = {beta}")
        print(f"Max log-likelihood = {result.getLogLikelihood()}")

# %%
# According to the previous results, we choose the *MinMax* normalization method and the *Gumbel* initial point.
# This initial point is cheaper than the *Static* one as it requires no optimization computation.
result_NonStatLL = factory.buildTimeVarying(
    sample,
    timeStamps,
    basis,
    muIndices,
    sigmaIndices,
    xiIndices,
    ot.Function(),
    ot.Function(),
    ot.Function(),
    "Gumbel",
    "MinMax",
)
beta = result_NonStatLL.getOptimalParameter()
print(f"beta = {beta}")
print(f"mu(t) = {beta[0]:.4f} + {beta[1]:.4f} * tau(t)")
print(f"sigma = {beta[2]:.4f}")
print(f"xi = {beta[3]:.4f}")

# %%
# You can get the expression of the normalizing function :math:`t \mapsto \tau(t)`:
normFunc = result_NonStatLL.getNormalizationFunction()
print("Function tau(t): ", normFunc)
print("c = ", normFunc.getEvaluation().getImplementation().getCenter()[0])
print("1/d = ", normFunc.getEvaluation().getImplementation().getLinear()[0, 0])

# %%
# You can get the function :math:`t \mapsto \vect{\theta}(t)` where :math:`\vect{\theta}(t) = (\mu(t), \sigma(t), \xi(t))`.
functionTheta = result_NonStatLL.getParameterFunction()

# %%
# We get the asymptotic distribution of :math:`\vect{\beta}` to compute some confidence intervals of
# the estimates, for example of order :math:`p = 0.95`.
dist_beta = result_NonStatLL.getParameterDistribution()
confidence_level = 0.95
for i in range(beta.getSize()):
    lower_bound = dist_beta.getMarginal(i).computeQuantile((1 - confidence_level) / 2)[
        0
    ]
    upper_bound = dist_beta.getMarginal(i).computeQuantile((1 + confidence_level) / 2)[
        0
    ]
    print(
        "Conf interval for beta_"
        + str(i + 1)
        + " = ["
        + str(lower_bound)
        + "; "
        + str(upper_bound)
        + "]"
    )

# %%
# In order to compare different modelings, we get the optimal log-likelihood of the data for both stationary
# and non stationary models. The difference is significant enough to be in favor of the non stationary model.
print("Max log-likelihood: ")
print("Stationary model =  ", result_LL.getLogLikelihood())
print("Non stationary linear mu(t) model =  ", result_NonStatLL.getLogLikelihood())

# %%
# In order to draw some diagnostic plots similar to those drawn in the stationary case, we refer to the
# following result: if :math:`Z_t` is a non stationary GEV model parametrized by :math:`(\mu(t), \sigma(t), \xi(t))`,
# then the standardized variables :math:`\hat{Z}_t` defined by:
#
# .. math::
#
#    \hat{Z}_t = \dfrac{1}{\xi(t)} \log \left[1+ \xi(t)\left( \dfrac{Z_t-\mu(t)}{\sigma(t)} \right)\right]
#
# have  the standard Gumbel distribution which is the GEV model with :math:`(\mu, \sigma, \xi) = (0, 1, 0)`.
#
# As a result, we can validate the inference result thanks the 4 usual diagnostic plots:
#
# - the probability-probability pot,
# - the quantile-quantile pot,
# - the return level plot,
# - the data histogram and the density of the fitted model.
#
# using the transformed data compared to the Gumbel model. We can see that the adequation is better
# than with the stationary model.
graph = result_NonStatLL.drawDiagnosticPlot()
view = otv.View(graph)

# %%
# We can draw the mean function  :math:`t \mapsto \Expect{\mbox{GEV}(t)}`. Be careful, it is not the function
# :math:`t \mapsto \mu(t)`. As a matter of fact, the mean is defined for :math:`\xi <1` only and in that case,
# for :math:`\xi \neq 0`, we have:
#
# .. math::
#     \Expect{\mbox{GEV}(t)} = \mu(t) + \dfrac{\sigma(t)}{\xi(t)} (\Gamma(1-\xi(t))-1)
#
# and for :math:`\xi = 0`, we have:
#
# .. math::
#     \Expect{\mbox{GEV}(t)} = \mu(t) + \sigma(t)\gamma
#
# where :math:`\gamma` is the Euler constant.
#
# We can also draw the function :math:`t \mapsto q_p(t)` where :math:`q_p(t)` is the quantile of
# order :math:`p` of the GEV distribution at time :math:`t`.
# Here, :math:`\mu(t)` is a linear function and the other parameters are constant, so the mean and the quantile
# functions are also linear functions.
graph = ot.Graph(
    r"Annual maximum sea-levels at Fremantle - Linear $\mu(t)$",
    "year",
    "level (m)",
    True,
    "",
)
graph.setIntegerXTick(True)
# data
cloud = ot.Cloud(data[:, :2])
cloud.setColor("red")
graph.add(cloud)
# mean function
meandata = [
    result_NonStatLL.getDistribution(t).getMean()[0] for t in data[:, 0].asPoint()
]
curve_meanPoints = ot.Curve(data[:, 0].asPoint(), meandata)
graph.add(curve_meanPoints)
# quantile function
graphQuantile = result_NonStatLL.drawQuantileFunction(0.95)
drawQuant = graphQuantile.getDrawable(0)
drawQuant = graphQuantile.getDrawable(0)
drawQuant.setLineStyle("dashed")
graph.add(drawQuant)
graph.setLegends(["data", "mean function", "quantile 0.95  function"])
graph.setLegendPosition("lower right")
view = otv.View(graph)


# %%
# At last, we can test the validity of the stationary model :math:`\mathcal{M}_0`
# relative to the model with time varying parameters  :math:`\mathcal{M}_1`. The
# model :math:`\mathcal{M}_0` is parametrized by :math:`(\beta_1, \beta_3, \beta_4)` and the model
# :math:`\mathcal{M}_1` is parametrized by :math:`(\beta_1, \beta_2, \beta_3, \beta_4)`: so we have
# :math:`\mathcal{M}_0 \subset \mathcal{M}_1`.
#
# We use the Likelihood Ratio test. The null hypothesis is the stationary model :math:`\mathcal{M}_0`.
# The Type I error :math:`\alpha` is taken equal to 0.05.
#
# This test confirms that the dependence through time is not negligible: it means that the linear :math:`\mu(t)`
# component explains a large variation in the data.
llh_LL = result_LL.getLogLikelihood()
llh_NonStatLL = result_NonStatLL.getLogLikelihood()
modelM0_Nb_param = 3
modelM1_Nb_param = 4
resultLikRatioTest = ot.HypothesisTest.LikelihoodRatioTest(
    modelM0_Nb_param, llh_LL, modelM1_Nb_param, llh_NonStatLL, 0.05
)
accepted = resultLikRatioTest.getBinaryQualityMeasure()
print(
    f"Hypothesis H0 (stationary model) vs H1 (linear mu(t) model):  accepted ? = {accepted}"
)

# %%
# We detail the statistics of the Likelihood Ratio test: the deviance statistics :math:`\mathcal{D}_p` follows
# a :math:`\chi^2_1` distribution.
# The model :math:`\mathcal{M}_0` is rejected if the deviance statistics estimated on the data is greater than
# the threshold :math:`c_{\alpha}` or if the p-value is less than the Type I error  :math:`\alpha = 0.05`.
print(f"Dp={resultLikRatioTest.getStatistic():.2f}")
print(f"alpha={resultLikRatioTest.getThreshold():.2f}")
print(f"p-value={resultLikRatioTest.getPValue():.2f}")

# %%
# We can perform the same study with a quadratic model for :math:`\mu(t)` or a linear model for
# :math:`\mu(t)` and :math:`\sigma(t)`:
#
# .. math::
#     :nowrap:
#
#     \begin{align*}
#       \mu(t) & = \beta_1 + \beta_2 \tau(t) + \beta_3\tau(t)^2 \\
#       \sigma(t) & = \beta_4 \\
#       \xi(t) & = \beta_5
#     \end{align*}
#
# or
#
# .. math::
#     :nowrap:
#
#     \begin{align*}
#     \mu(t) & = \beta_1 + \beta_2 \tau(t) \\
#     \sigma(t) & = \beta_3 + \beta_4\tau(t)\\
#     \xi(t) & = \beta_5
#     \end{align*}
#
# For each model, we give the log-likelihood values and we test the validity of each model with respect
# to the non stationary model where :math:`\mu(t)` is linear.
# We notice that there is no evidence to adopt a quadratic model for :math:`\mu(t)` nor a linear model
# for :math:`\mu(t)` and :math:`\sigma(t)`: the optimal log-likelihood for each model is very near the likelihood
# we obtained with a linear model for :math:`\mu(t)` only. It means that these both models do not bring significant
# improvements with respect to model tested before.
basis = ot.Basis(
    [constant, ot.SymbolicFunction(["t"], ["t"]), ot.SymbolicFunction(["t"], ["t^2"])]
)
result_NonStatLL_2 = factory.buildTimeVarying(
    sample,
    timeStamps,
    basis,
    [0, 1, 2],
    [0],
    [0],
    ot.Function(),
    ot.Function(),
    ot.Function(),
    "Gumbel",
    "MinMax",
)
result_NonStatLL_3 = factory.buildTimeVarying(
    sample,
    timeStamps,
    basis,
    [0, 1],
    [0, 1],
    [0],
    ot.Function(),
    ot.Function(),
    ot.Function(),
    "Gumbel",
    "MinMax",
)
print("Max log-likelihood = ")
print("Non stationary quadratic mu(t) model = ", result_NonStatLL_2.getLogLikelihood())
print(
    "Non stationary linear mu(t) and sigma(t) model = ",
    result_NonStatLL_3.getLogLikelihood(),
)
llh_LL = result_LL.getLogLikelihood()
llh_NonStatLL_2 = result_NonStatLL_2.getLogLikelihood()
llh_NonStatLL_3 = result_NonStatLL_3.getLogLikelihood()
resultLikRatioTest_2 = ot.HypothesisTest.LikelihoodRatioTest(
    4, llh_NonStatLL, 5, llh_NonStatLL_2, 0.05
)
resultLikRatioTest_3 = ot.HypothesisTest.LikelihoodRatioTest(
    4, llh_NonStatLL, 5, llh_NonStatLL_3, 0.05
)
accepted_2 = resultLikRatioTest_2.getBinaryQualityMeasure()
accepted_3 = resultLikRatioTest_3.getBinaryQualityMeasure()
print(
    f"Hypothesis H0 (linear mu(t) model) vs H1 (quadratic mu(t) model):  accepted ? = {accepted_2}"
)
print(
    f"Hypothesis H0 (linear mu(t) model) vs H1 (linear mu(t) and sigma(t) model):  accepted ? = {accepted_3}"
)

# %%
# **Non stationary GEV modeling with the covariates Time and SOI**
#
# Extreme sea-levels can be unusually extreme during periods when the El Nino effect is
# active.
# Hence, we study a modeling that takes into account the dependence between the extreme
# sea-levels
# at Fremantle and the annual mean value of the Southern Oscillation Index (SOI)
# besides the temporal dependence.
# The following figure shows that the annual maximum sea-levels are generally greater
# when the value of SOI is high. It might be due to the time trend in the data for the
# sea-levels
# and the SOI (each one increases with time). But it can also be possible that the
# SOI explains
# some of the variation in annual maximum sea-levels after allowance for the time variation
# in the process.
graph = ot.Graph("SOI at Fremantle", "SOI", "level (m)", True, "")
cloud = ot.Cloud(data.getMarginal([2, 1]))
cloud.setColor("red")
graph.add(cloud)
view = otv.View(graph)

# %%
# To consider this possibility, we study the model:
#
# .. math::
#     :nowrap:
#
#     \begin{align*}
#       \mu(t) & = \beta_1 t + \beta_2 \mbox{SOI} + \beta_3 \\
#       \sigma(t) & = \beta_4 \\
#       \xi(t) & = \beta_5
#     \end{align*}
#
# We consider two covariates: the time and the SOI. We build the sample of the values of both
# covariates: :math:`(t_i, \mbox{SOI}_i)_{1 \leq i \leq n}` where :math:`\mbox{SOI}_i =
# SOI(t_i)`.
# The constant covariate is
# automatically added by the library if not specified in order to allow
# some of the GEV parameters to remain constant (ie independent of both covariates
# :math:`(t, \mbox{SOI})`):
# this is the case for the :math:`\sigma` and :math:`\xi` parameters.
# This last constant covariate is associated to the
# third component of the covariates sample which now gathers the values
# :math:`(t_i, \mbox{SOI}_i, 1)` for :math:`1 \leq i \leq n`.
dataCovariates = data.getMarginal([0, 2])
print(dataCovariates[0:10])
result_Cov = factory.buildCovariates(sample, dataCovariates, [0, 1])

# %%
# We check here that a third component has effectively been added to the covariates
# sample: see the added third column which is constant equal to 1.
print(result_Cov.getCovariates()[0:10])

# %%
# We get the optimal parameter :math:`\vect{\beta}`.
beta = result_Cov.getOptimalParameter()
print("beta = ", beta)

# %%
# We get here the function :math:`(\vect{\beta}, t, \mbox{SOI}) \mapsto \vect{\theta}
# (\vect{\beta}, t, \mbox{SOI})` where :math:`\vect{\theta} = (\mu, \sigma, \xi)`. We see that
# :math:`\mu` depends on the three
# covariates :math:`(t, \mbox{SOI}, 1)` and that :math:`\sigma` and :math:`\xi` depends
# only on the third one
# which is the constant one.
print(result_Cov.getParameterFunction())
print(f"beta = {beta}")
print(f"mu(t) = {beta[0]:.4f} *t + {beta[1]:.4f} * SOI(t) + {beta[2]:.4f}")
print(f"sigma = {beta[3]:.4f}")
print(f"xi = {beta[4]:.4f}")

# %%
# We check here the normalizing function that has been used, which comes from
# the default method (the *MinMax* one).
print(result_Cov.getNormalizationFunction())

# %%
# We test this new model where :math:`\mu(t,SOI)` is modeled as a linear combination
# of the three covariates  :math:`(t, \mbox{SOI}, 1)` against the model
# with the linear-trend only :math:`\mu(t)`. The maximized log-likelihood of this
# new model is 53.9, compared to 49.9 for the first model. Hence, the
# deviance statistics is equal to :math:`D = 8.0`, which is large when judged relative to
# a :math:`\chi_1^2` distribution.
# It provides evidence that the effect of SOI is influential on annual maximum
# sea-levels at Fremantle, even after the allowance for time-variation.
llh_cov = result_Cov.getLogLikelihood()
print("Max log-likelihood: ", llh_cov)
resultLikRatioTest_SOI = ot.HypothesisTest.LikelihoodRatioTest(
    4, llh_NonStatLL, 5, llh_cov, 0.05
)
print(f"Dp={resultLikRatioTest_SOI.getStatistic():.2f}")
accepted = resultLikRatioTest_SOI.getBinaryQualityMeasure()
print(
    f"Hypothesis H0 (linear-trend mu(t) model) vs H1 (linear-trend and SOI mu(t,SOI) model):  accepted ? = {accepted}"
)

# %%
# We plot here the graphs :math:`t \mapsto \mu(t, \mbox{SOI}_0)` where
# :math:`\mbox{SOI}_0` is a given value (the mean value of the sample if not specified),
# and :math:`\mbox{SOI} \mapsto \mu(t_0, \mbox{SOI})` where :math:`t_0` is a given time.
# Care: there are three covariates :math:`(t, SOI, 1)` for the reasons mentioned previously.
# Then the reference point must be of dimension 3.
#
# As the relation is linear (the link function is the Identity function), we get some straight
# lines.
# The third graph is the dependence on the third covariate which is constant.
refSOI = dataCovariates.computeMean()[1]
refTime = 1940
refPoint = [refTime, refSOI, 1]
gridMu = result_Cov.drawParameterFunction1D(0, refPoint)
view = otv.View(gridMu)

# %%
# To adapt the labels and get rid of the last graph:
graphCol = gridMu.getGraphCollection()
graphMu1 = graphCol[0]
graphMu1.setTitle(r"$t \mapsto \mu(t, SOI_0)$, $SOI_0$ = {0:.2f}".format(refSOI))
graphMu1.setXTitle("t")
graphMu2 = graphCol[1]
graphMu2.setTitle(r"$SOI \mapsto \mu(t_0, SOI)$, $t_0 = $" + str(refTime))
graphMu2.setXTitle("SOI")
newGridLayout = ot.GridLayout(1, 2)
newGridLayout.setGraph(0, 0, graphMu1)
newGridLayout.setGraph(0, 1, graphMu2)
view = otv.View(newGridLayout)

# %%
# We plot here the graph :math:`(t, SOI) \mapsto \mu(t, SOI)`.
# As the third covariate is constant, the other graphs :math:`(t, 1)
# \mapsto \mu(t, \mbox{SOI}_0, 1)`
# and :math:`(1, SOI) \mapsto \mu(t_0, 1, SOI)` are not interesting
# as we have already obtained them with the previous method.
graphCol = result_Cov.drawParameterFunction2D(0, refPoint)
view = otv.View(graphCol)

# %%
# We plot here the graphs :math:`t \mapsto q_p(Z_{t, \mbox{SOI}_0})`
# and :math:`\mbox{SOI} \mapsto q_p(Z_{t_0, \mbox{SOI}})` where :math:`Z_{t, \mbox{SOI}_0}`
# is the process whose excesses of :math:`u` follow the estimated GPD,
# depending on the covariates :math:`(t, SOI)`. Then :math:`q_p`
# is the quantile of order :math:`p`.
# Because of the constant third covariate, the last graph is reduced to a point.
p = 0.95
gridQuantile = result_Cov.drawQuantileFunction1D(p, refPoint)
view = otv.View(gridQuantile)

# %%
# To adapt the labels and get rid of the last graph:
graphCol = gridQuantile.getGraphCollection()
graphQuant1 = graphCol[0]
graphQuant1.setTitle(r"$t \mapsto q_p(Z(t, SOI_0))$, $SOI_0$ = {0:.2f}".format(refSOI))
graphQuant1.setXTitle("t")
graphQuant1.setYTitle(r"$q_p$")
graphQuant2 = graphCol[1]
graphQuant2.setTitle(r"$SOI \mapsto q_p(Z(t_0, SOI))$, $t_0 = $" + str(refTime))
graphQuant2.setXTitle("SOI")
graphQuant2.setYTitle(r"$q_p$")
newGridLayout = ot.GridLayout(1, 2)
newGridLayout.setGraph(0, 0, graphQuant1)
newGridLayout.setGraph(0, 1, graphQuant2)
view = otv.View(newGridLayout)

# %%
otv.View.ShowAll()
