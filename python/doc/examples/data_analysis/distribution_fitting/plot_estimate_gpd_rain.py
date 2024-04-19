"""
Estimate a GPD on the daily rainfall data
=========================================
"""
# %%
# In this example, we illustrate various techniques of extreme value modeling applied
# to the daily rainfall accumulations in south-west England, over the period 1914-1962.
# Readers should refer to [coles2001]_ to get more details.
#
# We illustrate techniques to:
#
# - estimate a stationary and a non stationary GPD,
# - estimate a return level,
#
# using:
#
# - the log-likelihood function,
# - the profile log-likelihood function.
#
import openturns as ot
import openturns.experimental as otexp
import openturns.viewer as otv
from openturns.usecases import coles

# %%
# First, we load the Rain dataset. We start by looking at it through time.
dataRain = coles.Coles().rain
print(dataRain[:10])
graph = ot.Graph(
    "Daily rainfall accumulations SW England", "day", "level (mm)", True, ""
)
days = ot.Sample([[i] for i in range(len(dataRain))])
cloud = ot.Cloud(days, dataRain)
cloud.setColor("red")
cloud.setPointStyle(",")
graph.add(cloud)
graph.setIntegerXTick(True)
view = otv.View(graph)

# %%
# In order to select a threshold upon which the GPD model will be fitted, we draw
# the mean residual life plot with approximate :math:`95\%` confidence interval.
# It appears that the graph is linear from the threshold around
# :math:`u=30`. Then, it decays sharply although with a linear trend. We
# should be tempted to choose :math:`u=60` but there are only 6
# exceedances of the threshold :math:`u=60`. So it is not enough
# to make meaningful inferences. Moreover, the graph is not reliable
# for large values of :math:`u` due to the limited amount of data on
# which the estimate and the confidence interval are based.
# For all these reasons, it appears preferable to chose :math:`u=30`.
factory = ot.GeneralizedParetoFactory()
graph = factory.drawMeanResidualLife(dataRain)
view = otv.View(graph)

# %%
# To support that choice, we draw the parameter stability plots.
# We see that the perturbations appear small relatively to sampling errors.
# We can see that the change in pattern observed in the mean
# residual life plot is still apparent here for high thresholds.
# Hence, we choose the threshold :math:`u=30`.
u_range = ot.Interval(0.5, 50.0)
graph = factory.drawParameterThresholdStability(dataRain, u_range)
view = otv.View(graph, figure_kw={"figsize": (6.0, 6.0)})

# %%
# **Stationary GPD modeling via the log-likelihood function**
#
# We first assume that the dependence through time is negligible, so we first model the data as
# independent observations over the observation period.
# We estimate the parameters of the GPD distribution by maximizing
# the log-likelihood of the data for the selecte threshold :math:`u=30`.
u = 30
result_LL = factory.buildMethodOfLikelihoodMaximizationEstimator(dataRain, u)

# %%
# We get the fitted GPD and its parameters of :math:`(\hat{\sigma}, \hat{\xi}, u)`.
fitted_GPD = result_LL.getDistribution()
desc = fitted_GPD.getParameterDescription()
param = fitted_GPD.getParameter()
print(", ".join([f"{p}: {value:.3f}" for p, value in zip(desc, param)]))
print("log-likelihood = ", result_LL.getLogLikelihood())

# %%
# We get the asymptotic distribution of the estimator :math:`(\hat{\mu}, \hat{\sigma}, \hat{\xi})`.
# In that case, the asymptotic distribution is normal.
parameterEstimate = result_LL.getParameterDistribution()
print("Asymptotic distribution of the estimator : ")
print(parameterEstimate)

# %%
# We get the covariance matrix and the standard deviation of :math:`(\hat{\sigma}, \hat{\xi}, \hat{\xi})`.
print("Cov matrix = \n", parameterEstimate.getCovariance())
print("Standard dev = ", parameterEstimate.getStandardDeviation())

# %%
# We get the marginal confidence intervals of order 0.95.
order = 0.95
for i in range(2):  # exclude u parameter (fixed)
    ci = parameterEstimate.getMarginal(i).computeBilateralConfidenceInterval(order)
    print(desc[i] + ":", ci)

# %%
# At last, we can validate the inference result thanks to the 4 usual diagnostic plots:
#
# - the probability-probability pot,
# - the quantile-quantile pot,
# - the return level plot,
# - the data histogram and the density of the fitted model.
#
validation = otexp.GeneralizedParetoValidation(result_LL, dataRain)
graph = validation.drawDiagnosticPlot()
view = otv.View(graph)

# %%
# **Stationary GPD modeling via the profile log-likelihood function**
#
# Now, we use the profile log-likehood function with respect
# to the :math:`\xi` parameter rather than log-likehood function
# to estimate the parameters of the GPD.
result_PLL = factory.buildMethodOfXiProfileLikelihoodEstimator(dataRain, u)

# %%
# The following graph allows one to get the profile log-likelihood plot.
# It also indicates the optimal value of :math:`\xi`, the maximum profile log-likelihood and
# the confidence interval for :math:`\xi` of order 0.95 (which is the default value).
order = 0.95
result_PLL.setConfidenceLevel(order)
view = otv.View(result_PLL.drawProfileLikelihoodFunction())

# %%
# We can get the numerical values of the confidence interval: it appears to be a bit smaller
# with the interval obtained from the profile log-likelihood function than with the log-likelihood
# function.
# Note that if the order requested is too high, the confidence interval might not be calculated because
# one of its bound is out of the definition domain of the log-likelihood function.
try:
    print("Confidence interval for xi = ", result_PLL.getParameterConfidenceInterval())
except Exception as ex:
    print(type(ex))
    pass

# %%
# **Return level estimate from the estimated stationary GPD**
#
# We evaluate the :math:`T`-year return level which corresponds to the
# :math:`m`-observation return level, where :math:`m = T*n_y` with :math:`n_y`
# the number of observations per year. Here, we have daily observations, hence
# :math:`n_y = 365`. As we assumed that the observations were independent, the extremal index is :math:`\theta=1` which is the default value.
#
# The method also provides the asymptotic distribution of the estimator :math:`\hat{z}_m`.
ny = 365
T10 = 10
zm_10 = factory.buildReturnLevelEstimator(result_LL, dataRain, T10 * ny)
return_level_10 = zm_10.getMean()
print("Maximum log-likelihood function : ")
print(f"10-year return level = {return_level_10}")
return_level_ci10 = zm_10.computeBilateralConfidenceInterval(0.95)
print(f"CI = {return_level_ci10}")

T100 = 100
zm_100 = factory.buildReturnLevelEstimator(result_LL, dataRain, T100 * ny)
return_level_100 = zm_100.getMean()
print(f"100-year return level = {return_level_100}")
return_level_ci100 = zm_100.computeBilateralConfidenceInterval(0.95)
print(f"CI = {return_level_ci100}")

# %%
# **Return level estimate via the profile log-likelihood function of a stationary GPD**
#
# We can estimate the :math:`m`-observation return level :math:`z_m` directly from the data using the profile
# likelihood with respect to :math:`z_m`.
result_zm_100_PLL = factory.buildReturnLevelProfileLikelihoodEstimator(dataRain, u, T100 * ny)
zm_100_PLL = result_zm_100_PLL.getParameter()
print(f"100-year return level (profile) = {zm_100_PLL}")

# %%
# We can get the confidence interval of :math:`z_m`: once more, it appears to be a bit smaller
# than the interval obtained from the log-likelihood function.
result_zm_100_PLL.setConfidenceLevel(0.95)
return_level_ci100 = result_zm_100_PLL.getParameterConfidenceInterval()
print("Maximum profile log-likelihood function : ")
print(f"CI={return_level_ci100}")

# %%
# We can also plot the profile log-likelihood function and get the confidence interval, the optimal value
# of :math:`z_m` and its confidence interval.
view = otv.View(result_zm_100_PLL.drawProfileLikelihoodFunction())

# %%
# **Non stationary GPD modeling via the log-likelihood function**
#
# Now, we want to check whether it is necessary to model the time dependency over
# the observation period.
#
# We have to define the functional basis for each parameter of the GPD model. Even if we have
# the possibility to affect a time-varying model to each of the 2 parameters
# :math:`(\sigma, \xi)`,
# it is strongly recommended not to vary the shape parameter :math:`\xi`.
#
# We suppose that :math:`\sigma` is linear with time, and that the other parameters remain constant.
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
# .. math::
#     :nowrap:
#
#     \begin{align*}
#       \sigma(t) & = \beta_1 + \beta_2\tau(t) \\
#       \xi(t) & = \beta_3
#     \end{align*}
#
constant = ot.SymbolicFunction(["t"], ["1.0"])
basis = ot.Basis([ot.SymbolicFunction(["t"], ["t"]), constant])
# basis for mu, sigma, xi
sigmaIndices = [0, 1]  # linear
xiIndices = [1]  # stationary

# %%
# We need to get the time stamps (in days here).
timeStamps = ot.Sample([[i + 1] for i in range(len(dataRain))])

# %%
# We can now estimate the list of coefficients :math:`\vect{\beta} = (\beta_1, \beta_2, \beta_3)` using
# the log-likelihood of the data.
result_NonStatLL = factory.buildTimeVarying(dataRain, u, timeStamps, basis, sigmaIndices, xiIndices)
beta = result_NonStatLL.getOptimalParameter()
print(f"beta = {beta}")
print(f"sigma(t) = {beta[1]:.4f} * tau(t) + {beta[0]:.4f}")
print(f"xi = {beta[2]:.4f}")
print(f"Max log-likelihood = {result_NonStatLL.getLogLikelihood()}")

# %%
# We get the asymptotic distribution of :math:`\vect{\beta}` to compute some confidence intervals of
# the estimates, for example of order :math:`p = 0.95`.
dist_beta = result_NonStatLL.getParameterDistribution()
condifence_level = 0.95
for i in range(beta.getSize()):
    lower_bound = dist_beta.getMarginal(i).computeQuantile((1 - condifence_level) / 2)[
        0
    ]
    upper_bound = dist_beta.getMarginal(i).computeQuantile((1 + condifence_level) / 2)[
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
# You can get the expression of the normalizing function :math:`t \mapsto \tau(t)`:
normFunc = result_NonStatLL.getNormalizationFunction()
print("Function tau(t): ", normFunc)
print("c = ", normFunc.getEvaluation().getImplementation().getCenter()[0])
print("1/d = ", normFunc.getEvaluation().getImplementation().getLinear()[0, 0])

# %%
# You can get the function :math:`t \mapsto \vect{\theta}(t)` where
# :math:`\vect{\theta}(t) = (\mu(t), \sigma(t), \xi(t))`.
functionTheta = result_NonStatLL.getParameterFunction()

# %%
# In order to compare different modelings, we get the optimal log-likelihood of the data for both stationary
# and non stationary models. The difference seems to be non significant enough, which means that the non
# stationary model does not really improve the quality of the modeling.
print("Max log-likelihood: ")
print("Stationary model =  ", result_LL.getLogLikelihood())
print("Non stationary linear sigma(t) model =  ", result_NonStatLL.getLogLikelihood())

# %%
# In order to draw some diagnostic plots similar to those drawn in the stationary case, we refer to the
# following result: if :math:`Y_t` is a non stationary GPD model parametrized by :math:`(\sigma(t), \xi(t), u)`,
# then the standardized variables :math:`\hat{Y}_t` defined by:
#
# .. math::
#
#    \hat{Y}_t = \dfrac{1}{\xi(t)} \log \left[1+ \xi(t)\left( \dfrac{Y_t-u}{\sigma(t)} \right)\right]
#
# have the Exponential distribution which is the GPD model with :math:`(\sigma, \xi, u) = (1, 0, 0)`.
#
# As a result, we can validate the inference result thanks the 4 usual diagnostic plots:
#
# - the probability-probability pot,
# - the quantile-quantile pot,
# - the return level plot,
# - the data histogram and the density of the fitted model.
#
# using the transformed data compared to the Exponential model. We can see that the adequation seems similar to the graph
# of the stationary model.
graph = result_NonStatLL.drawDiagnosticPlot()
view = otv.View(graph)

# %%
# We can draw the mean function :math:`t \mapsto \Expect{\mbox{GPD}(t)}`, defined for :math:`\xi <1` only:
#
# .. math::
#     \Expect{\mbox{GPD}(t)} = u + \dfrac{\sigma(t)}{1 - \xi(t)}
#
# We can also draw the function :math:`t \mapsto q_p(t)` where :math:`q_p(t)` is the quantile of
# order :math:`p` of the GPD distribution at time :math:`t`.
# Here, :math:`\sigma(t)` is a linear function and the other parameters are constant, so the mean and the quantile
# functions are also linear functions.
graph = ot.Graph(
    r"Maximum rain - Linear $\sigma(t)$",
    "day",
    "level (mm)",
    True,
    "",
)
graph.setIntegerXTick(True)
# data
cloud = ot.Cloud(timeStamps, dataRain)
cloud.setColor("red")
graph.add(cloud)
# mean function
meandata = [
    result_NonStatLL.getDistribution(t).getMean()[0] for t in timeStamps.asPoint()
]
curve_meanPoints = ot.Curve(timeStamps.asPoint(), meandata)
graph.add(curve_meanPoints)
# quantile function
graphQuantile = result_NonStatLL.drawQuantileFunction(0.95)
drawQuant = graphQuantile.getDrawable(0)
drawQuant = graphQuantile.getDrawable(0)
drawQuant.setLineStyle("dashed")
graph.add(drawQuant)
graph.setLegends(["data", "mean function", "quantile 0.95 function"])
graph.setLegendPosition("lower right")
view = otv.View(graph)

# %%
# At last, we can test the validity of the stationary model :math:`\mathcal{M}_0`
# relative to the model with time varying parameters  :math:`\mathcal{M}_1`. The
# model :math:`\mathcal{M}_0` is parametrized
# by :math:`(\beta_1, \beta_3, \beta_4)` and the model :math:`\mathcal{M}_1` is parametrized
# by :math:`(\beta_1, \beta_2, \beta_3, \beta_4)`: so we have :math:`\mathcal{M}_0 \subset \mathcal{M}_1`.
#
# We use the Likelihood Ratio test. The null hypothesis is the stationary model :math:`\mathcal{M}_0`.
# The Type I error :math:`\alpha` is taken equal to 0.05.
#
# This test confirms that there is no evidence of a linear trend for :math:`\mu`.
llh_LL = result_LL.getLogLikelihood()
llh_NonStatLL = result_NonStatLL.getLogLikelihood()
modelM0_Nb_param = 2
modelM1_Nb_param = 3
resultLikRatioTest = ot.HypothesisTest.LikelihoodRatioTest(
    modelM0_Nb_param, llh_LL, modelM1_Nb_param, llh_NonStatLL, 0.05
)
accepted = resultLikRatioTest.getBinaryQualityMeasure()
print(
    f"Hypothesis H0 (stationary model) vs H1 (linear mu(t) model):  accepted ? = {accepted}"
)

# %%
# We detail the statistics of the Likelihood Ratio test: the deviance statistics
# :math:`\mathcal{D}_p` follows a :math:`\chi^2_1` distribution.
# The model :math:`\mathcal{M}_0` is rejected if the deviance statistics estimated on the data is greater than
# the threshold :math:`c_{\alpha}` or if the p-value is less than the Type I error  :math:`\alpha = 0.05`.
print(f"Dp={resultLikRatioTest.getStatistic():.2f}")
print(f"alpha={resultLikRatioTest.getThreshold():.2f}")
print(f"p-value={resultLikRatioTest.getPValue():.2f}")

# %%
otv.View.ShowAll()
