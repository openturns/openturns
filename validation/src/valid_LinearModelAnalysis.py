"""
We compare openturns models with R models in case of intercept/no intercept
we use small tests (5 exactly) mixing various situations
"""
import numpy as np
from rpy2.robjects import Formula, r

try:
    from rpy2.robjects.numpy2ri import numpy2ri, ri2py
except ImportError:
    from rpy2.robjects.numpy2ri import numpy2rpy as numpy2ri
    from rpy2.robjects.numpy2ri import rpy2py as ri2py

#from rpy2.robjects.numpy2ri import numpy2ri, ri2py
from rpy2.robjects.packages import importr, data

import openturns as ot
# Require 

stats = importr("stats")
faraway = importr("faraway")
savings_data = data(faraway).fetch("savings")["savings"]
#data(faraway).fetch("savings")["savings"]

# Model 1 : 2 param, non intercept
sr = ri2py(savings_data)["sr"]
r.assign('sr', numpy2ri(sr))
pop15 = ri2py(savings_data)["pop15"]
r.assign('pop15', numpy2ri(pop15))
pop75 = ri2py(savings_data)["pop75"]
r.assign('pop75', numpy2ri(pop75))

formula = Formula('sr ~ pop75 + pop15 - 1')
fit = stats.lm(formula)
summary = stats.summary_lm(fit)

"""
list(summary.names) provides
['call', 'terms', 'residuals', 'coefficients', 'aliased', 'sigma', 'df', 'r.squared', 'adj.r.squared', 'fstatistic', 'cov.unscaled']
 """
r2 = summary[7][0]
ar2 = summary[8][0]
ftest = summary[9][0]

# openturns like
X = ot.Sample(len(pop15), 0)
P = ot.Sample.BuildFromPoint(pop15)
X.stack(P)
P = ot.Sample.BuildFromPoint(pop75)
X.stack(P)
X.setDescription(["pop15", "pop75"])
Y = ot.Sample.BuildFromPoint(sr)
# Model with pop15 + pop75 without intercept
# We do not run mode with pop15 only for the moment
f = ot.SymbolicFunction(["pop15", "pop75"], ["pop15"])
g = ot.SymbolicFunction(["pop15", "pop75"], ["pop75"])
h = ot.SymbolicFunction(["pop15", "pop75"], ["1"])
basis = ot.Basis([f, g])
model = ot.LinearModelAlgorithm(X, basis, Y)
model.run()
result = model.getResult()
analysis = ot.LinearModelAnalysis(result)

np.testing.assert_almost_equal(result.getRSquared(), r2, 14)
np.testing.assert_almost_equal(result.getAdjustedRSquared(), ar2, 14)
np.testing.assert_almost_equal(analysis.getFisherScore(), ftest, 14)

#---------------------------------------------------------------------
# Model 2 : 2 param + intercepts

formula = Formula('sr ~ pop75 + pop15')
fit = stats.lm(formula)
summary = stats.summary_lm(fit)

r2 = summary[7][0]
ar2 = summary[8][0]
ftest = summary[9][0]

# openturns like
model = ot.LinearModelAlgorithm(X, Y)
model.run()
result = model.getResult()
analysis = ot.LinearModelAnalysis(result)

np.testing.assert_almost_equal(result.getRSquared(), r2, 14)
np.testing.assert_almost_equal(result.getAdjustedRSquared(), ar2, 14)
np.testing.assert_almost_equal(analysis.getFisherScore(), ftest, 14)

#---------------------------------------------------------------------
# Model 3 : 1 param (pop15) + intercepts
formula = Formula('sr ~ pop15')
fit = stats.lm(formula)
summary = stats.summary_lm(fit)

r2 = summary[7][0]
ar2 = summary[8][0]
ftest = summary[9][0]

# openturns like
basis = ot.Basis([f, h])
model = ot.LinearModelAlgorithm(X, basis, Y)
model.run()
result = model.getResult()
analysis = ot.LinearModelAnalysis(result)

np.testing.assert_almost_equal(result.getRSquared(), r2, 14)
np.testing.assert_almost_equal(result.getAdjustedRSquared(), ar2, 14)
np.testing.assert_almost_equal(analysis.getFisherScore(), ftest, 14)

#---------------------------------------------------------------------
# Model 4 : 1 param (pop15) + no intercept intercepts
formula = Formula('sr ~ pop15 - 1')
fit = stats.lm(formula)
summary = stats.summary_lm(fit)

r2 = summary[7][0]
ar2 = summary[8][0]
ftest = summary[9][0]

# openturns like
basis = ot.Basis([f])
model = ot.LinearModelAlgorithm(X, basis, Y)
model.run()
result = model.getResult()
analysis = ot.LinearModelAnalysis(result)

np.testing.assert_almost_equal(result.getRSquared(), r2, 14)
np.testing.assert_almost_equal(result.getAdjustedRSquared(), ar2, 14)
np.testing.assert_almost_equal(analysis.getFisherScore(), ftest, 13)

#---------------------------------------------------------------------
# Model 5 : only intercept

formula = Formula('sr ~ 1')
fit = stats.lm(formula)
summary = stats.summary_lm(fit)

"""
list(summary.names) provides
['call', 'terms', 'residuals', 'coefficients', 'aliased', 'sigma', 'df', 'r.squared', 'adj.r.squared', 'cov.unscaled']
 """

r2 = summary[7][0]
ar2 = summary[8][0]

basis = ot.Basis([h])
model = ot.LinearModelAlgorithm(X, basis, Y)
model.run()
result = model.getResult()
analysis = ot.LinearModelAnalysis(result)

np.testing.assert_almost_equal(result.getRSquared(), r2, 14)
np.testing.assert_almost_equal(result.getAdjustedRSquared(), ar2, 14)
