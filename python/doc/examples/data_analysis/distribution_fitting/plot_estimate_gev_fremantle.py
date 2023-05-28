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
# - estimate a stationary and a non stationary GEV,
# - estimate a return level,
#
# using:
#
# - the log-likelihood function,
# - the profile log-likelihood function.
#
# First, we load the Fremantle dataset of the annual maximum sea-levels. We start by looking at them
# through time. The data also contain the annual mean value of the Southern Oscillation Index (SOI),
# which is a proxy for meteorological volatility due to effects such as El Nino.
import openturns as ot
import openturns.viewer as otv
import openturns.experimental as otexp
from openturns.usecases import coles

data = coles.Coles().fremantle
print(data[:5])
graph = ot.Graph("Annual maximum sea-levels at Fremantle", "year", "level (m)", True, "")
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
print('Asymptotic distribution of the estimator : ')
print(parameterEstimate)

# %%
# We get the covariance matrix  and the standard deviation of :math:`(\hat{\mu}, \hat{\sigma}, \hat{\xi})`.
print('Cov matrix = ', parameterEstimate.getCovariance())
print('Standard dev = ', parameterEstimate.getStandardDeviation())

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
validation = otexp.GeneralizedExtremeValueValidation(result_LL, sample)
graph = validation.drawDiagnosticPlot()
view = otv.View(graph)

# %%
# **Stationary GEV modeling via the profile log-likelihood function**
#
# Now, we use the profile log-likehood function rather than log-likehood function  to estimate the parameters of the GEV.
result_PLL = factory.buildMethodOfProfileLikelihoodMaximizationEstimator(sample)

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
    print('Confidence interval for xi = ', result_PLL.getParameterConfidenceInterval())
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
# - the *CenterReduce* method where :math:`c = \dfrac{1}{m} \sum_{i=1}^m t_i` is the mean time stamps
#   and :math:`d = \sqrt{\dfrac{1}{m} \sum_{i=1}^m (t_i-c)^2}` is the standard deviation of the time stamps;
# - the *MinMax* method where :math:`c = t_1` is the first time and :math:`d =  = t_m-t_1` the final time;
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
basis_lin = ot.Basis([constant, ot.SymbolicFunction(["t"], ["t"])])
basis_cst = ot.Basis([constant])
# basis for mu, sigma, xi
basis_coll = [basis_lin, basis_cst, basis_cst]

# %% We need to get the time stamps (in years here).
timeStamps = data[:, 0]

# %%
# We can now estimate the list of coefficients :math:`\vect{\beta} = (\beta_1, \beta_2, \beta_3, \beta_4)` using the log-likelihood of the data.
#
# We test the 3 normalizing methods and both initial points in order to evaluate their impact on the results.
# We can see that:
#
# - both normalization methods lead to the same result,
# - both initial points lead to the same result when the data have been normalized,
# - it is very important to normalize all the data: if not, the result strongly depends on the initial point
#   and it differs from the result obtained with normalized data. The results are not optimal in that case
#   since the associated log-likelihood are much smaller than those obtained with normalized data.
#
initiPoint_list = list()
initiPoint_list.append("Gumbel")
initiPoint_list.append("Static")
normMethod_list = list()
normMethod_list.append("MinMax")
normMethod_list.append("CenterReduce")
normMethod_list.append("None")

print('Linear mu(t) model: ')
for normMeth in normMethod_list:
    for initPoint in initiPoint_list:
        print('normMeth, initPoint = ', normMeth, initPoint)
        # The ot.Function() is the identity function.
        result = factory.buildTimeVarying(sample, timeStamps, basis_coll, ot.Function(), initPoint, normMeth)
        beta = result.getOptimalParameter()
        print('beta1, beta2, beta3, beta4 = ', beta)
        print('Max log-likelihood =  ', result.getLogLikelihood())

# %%
# According to the previous results, we choose the *MinMax* normalization method and the *Gumbel* initial point.
# This initial point is cheaper than the *Static* one as it requires no optimization computation.
result_NonStatLL = factory.buildTimeVarying(sample, timeStamps, basis_coll, ot.Function(), "Gumbel", "MinMax")
beta = result_NonStatLL.getOptimalParameter()
print('beta1, beta2, beta3, beta_4 = ', beta)
print(f"mu(t) = {beta[0]:.4f} + {beta[1]:.4f} * tau")
print(f"sigma = {beta[2]:.4f}")
print(f"xi = {beta[3]:.4f}")

# %%
# You can get the expression of the normalizing function :math:`t \mapsto \tau(t)`:
print('Function tau(t): ', result_NonStatLL.getNormalizationFunction())

# %%
# You can get the function :math:`t \mapsto \vect{\theta}(t)` where :math:`\vect{\theta}(t) = (\mu(t), \sigma(t), \xi(t))`.
functionTheta = result_NonStatLL.getParameterFunction()

# %%
# We get the asymptotic distribution of :math:`\vect{\beta}` to compute some confidence intervals of
# the estimates, for example of order :math:`p = 0.95`.
dist_beta = result_NonStatLL.getParameterDistribution()
condifence_level = 0.95
for i in range(beta.getSize()):
    lower_bound = dist_beta.getMarginal(i).computeQuantile((1 - condifence_level) / 2)[0]
    upper_bound = dist_beta.getMarginal(i).computeQuantile((1 + condifence_level) / 2)[0]
    print('Conf interval for beta_' + str(i + 1) + ' = [' + str(lower_bound) + '; ' + str(upper_bound) + ']')

# %%
# In order to compare different modelings, we get the optimal log-likelihood of the data for both stationary
# and non stationary models. The difference is significant enough to be in favor of the non stationary model.
print('Max log-likelihood: ')
print('Stationary model =  ', result_LL.getLogLikelihood())
print('Non stationary linear mu(t) model =  ', result_NonStatLL.getLogLikelihood())

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
graph = ot.Graph(r"Annual maximum sea-levels at Fremantle - Linear $\mu(t)$", "year", "level (m)", True, "")
graph.setIntegerXTick(True)
# data
cloud = ot.Cloud(data[:, :2])
cloud.setColor("red")
graph.add(cloud)
# mean function
meandata = [result_NonStatLL.getDistribution(t).getMean()[0] for t in data[:, 0].asPoint()]
curve_meanPoints = ot.Curve(data[:, 0].asPoint(), meandata)
graph.add(curve_meanPoints)
# quantile function
graphQuantile = result_NonStatLL.drawQuantileFunction(0.95)
drawQuant = graphQuantile.getDrawable(0)
drawQuant = graphQuantile.getDrawable(0)
drawQuant.setLineStyle('dashed')
graph.add(drawQuant)
graph.setLegends(['data', 'mean function', 'quantile 0.95  function'])
graph.setLegendPosition('bottomright')
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
# This test confirms that the dependence through time is not negligible: it means that the linear :math:`mu(t)`
# component explains a large variation in the data.
llh_LL = result_LL.getLogLikelihood()
llh_NonStatLL = result_NonStatLL.getLogLikelihood()
resultLikRatioTest = ot.HypothesisTest.LikelihoodRatioTest(llh_LL, llh_NonStatLL, 0.05)
accepted = resultLikRatioTest.getBinaryQualityMeasure()
print(f"Hypothesis H0 (stationary model) vs H1 (linear mu(t) model):  accepted ? = {accepted}")

# %%
# We detail the statistics of the Likelihood Ratio test: the deviance statistics :math:`\mathcal{D}_p` follows
# a :math:`\chi^2_1` distribution.
# The model :math:`\mathcal{M}_0` is rejected if the deviance statistics estimated on the data is greater than
# the p-value :math:`c_{\alpha}` corresponding to the Type I error  :math:`\alpha`.
print(f"Dp={resultLikRatioTest.getStatistic():.2f}")
print(f"cAlpha={resultLikRatioTest.getThreshold():.2f}")

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
# to the stationary first model.
# We notice that there is no evidence to adopt a quadratic model for :math:`\mu(t)` nor a linear model
# for :math:`\mu(t)` and :math:`\sigma(t)`: the optimal log-likelihood for each model is very near the likelihood
# we obtained with a linear model for :math:`\mu(t)` only. It means that these both models do not bring significant
# improvements with respect to model tested before.
basis_quad = ot.Basis([constant, ot.SymbolicFunction(["t"], ["t"]), ot.SymbolicFunction(["t"], ["t^2"])])
basis_coll_2 = [basis_quad, basis_cst, basis_cst]
basis_coll_3 = [basis_lin, basis_lin, basis_cst]
result_NonStatLL_2 = factory.buildTimeVarying(sample, timeStamps, basis_coll_2, ot.Function(), "Gumbel", "MinMax")
result_NonStatLL_3 = factory.buildTimeVarying(sample, timeStamps, basis_coll_3, ot.Function(), "Gumbel", "MinMax")
print('Max log-likelihood = ')
print('Non stationary quadratic mu(t) model = ', result_NonStatLL_2.getLogLikelihood())
print('Non stationary linear mu(t) and sigma(t) model = ', result_NonStatLL_3.getLogLikelihood())
llh_LL = result_LL.getLogLikelihood()
llh_NonStatLL_2 = result_NonStatLL_2.getLogLikelihood()
llh_NonStatLL_3 = result_NonStatLL_3.getLogLikelihood()
resultLikRatioTest_2 = ot.HypothesisTest.LikelihoodRatioTest(llh_LL, llh_NonStatLL_2, 0.05)
resultLikRatioTest_3 = ot.HypothesisTest.LikelihoodRatioTest(llh_LL, llh_NonStatLL_3, 0.05)
accepted_2 = resultLikRatioTest_2.getBinaryQualityMeasure()
accepted_3 = resultLikRatioTest_3.getBinaryQualityMeasure()
print(f"Hypothesis H0 (stationary model) vs H1 (quadratic mu(t) model):  accepted ? = {accepted_2}")
print(f"Hypothesis H0 (stationary model) vs H1 (linear mu(t) and sigma(t) model):  accepted ? = {accepted_3}")

# %%
otv.View.ShowAll()
