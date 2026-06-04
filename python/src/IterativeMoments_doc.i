%feature("docstring") OT::IterativeMoments
"Iterative moments.

Parameters
----------
order : int
    Maximum order of the moments wanted

dimension : int
    Dimension of the input data

Notes
-----
This class iteratively computes the central moments of an iteratively increasing
dataset without storing any data in memory.

See also
--------
IterativeAlgorithm

Examples
--------

In the following example, we iteratively compute the sample statistics of a
sample.
This is based on a sample with size equal to 2000.
In the `for` loop, each realization of the distribution is used to update
the statistics up to the 4th order moments.

>>> import openturns as ot
>>> distNormal = ot.Normal()
>>> order = 4
>>> dim = 1
>>> iterMoments = ot.IterativeMoments(order, dim)
>>> size = 2000
>>> meanEvolution = ot.Sample()
>>> for i in range(size):
...     point = distNormal.getRealization()
...     iterMoments.increment(point)
>>> print('Mean:     ', iterMoments.getMean())
Mean:      [-0.00726852]
>>> print('Variance: ', iterMoments.getVariance())
Variance:  [0.99156]
>>> print('Skewness: ', iterMoments.getSkewness())
Skewness:  [0.0577277]
>>> print('Kurtosis: ', iterMoments.getKurtosis())
Kurtosis:  [3.11128]

The statistics can also be updated with a single sample.

>>> distNormal = ot.Normal()
>>> dim = 1
>>> order = 1
>>> iterMoments = ot.IterativeMoments(order, dim)
>>> size = 2000
>>> sample = distNormal.getSample(size)
>>> iterMoments.increment(sample)
>>> print('Mean: ', iterMoments.getMean())
Mean:  [-0.017342]"

// ---------------------------------------------------------------------

%feature("docstring") OT::IterativeMoments::getKurtosis
"Returns the current value of the unbiased estimator of the kurtosis

Returns
-------
kurtosis : :class:`~openturns.Point`
    Current values of the iterative kurtosis."

// ---------------------------------------------------------------------

%feature("docstring") OT::IterativeMoments::getSkewness
"Returns the current value of the unbiased estimator of the skewness

Returns
-------
skewness : :class:`~openturns.Point`
    Current values of the skewness."

// ---------------------------------------------------------------------

%feature("docstring") OT::IterativeMoments::getVariance
"Returns the current value of the unbiased estimator of the variance

Returns
-------
variance : :class:`~openturns.Point`
    Current values of the variance."

// ---------------------------------------------------------------------

%feature("docstring") OT::IterativeMoments::getStandardErrorOfTheMean
"Returns the standard error of the mean

Returns
-------
error : :class:`~openturns.Point`
    Current value of the standard error of the mean."

// ---------------------------------------------------------------------

%feature("docstring") OT::IterativeMoments::getCoefficientOfVariation
"Returns the coefficient of variation

Returns
-------
coefficientOfvariation : :class:`~openturns.Point`
    Current value of the coefficients of variation."


// ---------------------------------------------------------------------

%feature("docstring") OT::IterativeMoments::getStandardDeviation
"Returns the standard deviation

Returns
-------
standardDeviation : :class:`~openturns.Point`
    Current value of the standard deviation."

// ---------------------------------------------------------------------

%feature("docstring") OT::IterativeMoments::getMean
"Returns the values of the mean

Returns
-------
mean : :class:`~openturns.Point`
    Current value of the mean."

// ---------------------------------------------------------------------

%feature("docstring") OT::IterativeMoments::getCentralMoments
"Returns the central moments

Returns
-------
centralMoments : :class:`~openturns.Sample`
    Current value of the central moments."

// ---------------------------------------------------------------------

%feature("docstring") OT::IterativeMoments::getOrder
"Returns the maximum order prescribed

Returns
-------
order : int
    Prescribed order of the iterative object."

// ---------------------------------------------------------------------


