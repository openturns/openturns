%feature("docstring") OT::StatisticsEngine
"Statistical operations on DataContainer objects.

This class is experimental and likely to be modified in future releases.

StatisticsEngine provides static methods for statistical operations
on DataContainer objects, using TBBImplementation for parallelism.

Sample convention: row = observation, column = variable.
Input data must be in ROW_MAJOR layout.

Notes
-----
The following :class:`~openturns.ResourceMap` keys are used:

- ``StatisticsEngine-DefaultBlockSize`` (``UnsignedInteger``, default: ``4096``): number of rows per block for out-of-core blockwise statistics.
- ``StatisticsEngine-ParallelThreshold`` (``UnsignedInteger``, default: ``100000``): minimum row count for parallel reductions; smaller inputs use serial reductions for bitwise reproducibility."

// ---------------------------------------------------------------------

%feature("docstring") OT::StatisticsEngine::ComputeMean
"Compute mean of each column.

Parameters
----------
sample : DataContainer
    Input sample (ROW_MAJOR, n x d).

Returns
-------
mean : DataContainer
    Mean vector of size d."

// ---------------------------------------------------------------------

%feature("docstring") OT::StatisticsEngine::ComputeVariance
"Compute variance of each column.

Parameters
----------
sample : DataContainer
    Input sample (ROW_MAJOR, n x d).

Returns
-------
variance : DataContainer
    Variance vector of size d."

// ---------------------------------------------------------------------

%feature("docstring") OT::StatisticsEngine::ComputeStandardDeviation
"Compute standard deviation of each column.

Parameters
----------
sample : DataContainer
    Input sample (ROW_MAJOR, n x d).

Returns
-------
stdDev : DataContainer
    Standard deviation vector of size d."

// ---------------------------------------------------------------------

%feature("docstring") OT::StatisticsEngine::ComputeCovariance
"Compute covariance matrix.

Parameters
----------
sample : DataContainer
    Input sample (ROW_MAJOR, n x d).

Returns
-------
covariance : DataContainer
    Covariance matrix of size d x d."

// ---------------------------------------------------------------------

%feature("docstring") OT::StatisticsEngine::ComputePearsonCorrelation
"Compute Pearson correlation matrix.

Parameters
----------
sample : DataContainer
    Input sample (ROW_MAJOR, n x d).

Returns
-------
correlation : DataContainer
    Pearson correlation matrix of size d x d."

// ---------------------------------------------------------------------

%feature("docstring") OT::StatisticsEngine::ComputeSpearmanCorrelation
"Compute Spearman rank correlation matrix.

Parameters
----------
sample : DataContainer
    Input sample (ROW_MAJOR, n x d).

Returns
-------
correlation : DataContainer
    Spearman correlation matrix of size d x d."

// ---------------------------------------------------------------------

%feature("docstring") OT::StatisticsEngine::ComputeKendallTau
"Compute Kendall rank correlation matrix (with ties correction).

Parameters
----------
sample : DataContainer
    Input sample (ROW_MAJOR, n x d).

Returns
-------
correlation : DataContainer
    Kendall correlation matrix of size d x d."

// ---------------------------------------------------------------------

%feature("docstring") OT::StatisticsEngine::ComputeQuantile
"Compute quantiles of each column, at one or several probability levels.

Parameters
----------
sample : DataContainer
    Input sample (ROW_MAJOR, n x d).
q : float or DataContainer
    Quantile level in [0, 1], or levels read in flat storage order.
type7 : bool, optional
    Interpolation method. If False (default), use the historical rule
    with knots at (k + 1/2) / n clamped to the sample range. If True,
    use the Hyndman-Fan Type-7 rule with knots at k / (n - 1), as in R
    and NumPy default settings.

Returns
-------
quantile : DataContainer
    Quantile vector of size d (single level), or levels x d matrix
    (several levels)."

// ---------------------------------------------------------------------

%feature("docstring") OT::StatisticsEngine::ComputeEmpiricalCDF
"Compute the empirical CDF (or survival function) at a location.

Parameters
----------
sample : DataContainer
    Input sample (ROW_MAJOR, n x d).
location : DataContainer
    Evaluation point, coordinates read in flat storage order.
tail : bool
    If False, count the points with all coordinates below or equal.
    If True, count the points with all coordinates strictly above.

Returns
-------
cdf : float
    Empirical probability at the location."

// ---------------------------------------------------------------------

%feature("docstring") OT::StatisticsEngine::Sort
"Sort sample by a given column.

Parameters
----------
sample : DataContainer
    Input sample (ROW_MAJOR, n x d).
columnIndex : int, optional
    Column to sort by. Default is 0.

Returns
-------
sorted : DataContainer
    Sorted copy of the sample."

// ---------------------------------------------------------------------

%feature("docstring") OT::StatisticsEngine::SortLexicographic
"Sort sample rows in lexicographic order.

Parameters
----------
sample : DataContainer
    Input sample (ROW_MAJOR, n x d).

Returns
-------
sorted : DataContainer
    Sorted copy of the sample."

// ---------------------------------------------------------------------

%feature("docstring") OT::StatisticsEngine::ComputeMin
"Compute element-wise minimum across rows.

Parameters
----------
sample : DataContainer
    Input sample (ROW_MAJOR, n x d).

Returns
-------
minimum : DataContainer
    Minimum vector of size d."

// ---------------------------------------------------------------------

%feature("docstring") OT::StatisticsEngine::ComputeMax
"Compute element-wise maximum across rows.

Parameters
----------
sample : DataContainer
    Input sample (ROW_MAJOR, n x d).

Returns
-------
maximum : DataContainer
    Maximum vector of size d."

// ---------------------------------------------------------------------

%feature("docstring") OT::StatisticsEngine::ComputeSum
"Compute sum of each column.

Parameters
----------
sample : DataContainer
    Input sample (ROW_MAJOR, n x d).

Returns
-------
sum : DataContainer
    Sum vector of size d."

// ---------------------------------------------------------------------

%feature("docstring") OT::StatisticsEngine::ComputeRawMoment
"Compute raw moment of order k.

Parameters
----------
sample : DataContainer
    Input sample (ROW_MAJOR, n x d).
k : int
    Moment order.

Returns
-------
moment : DataContainer
    Raw moment vector of size d."

// ---------------------------------------------------------------------

%feature("docstring") OT::StatisticsEngine::ComputeCentralMoment
"Compute central moment of order k.

Parameters
----------
sample : DataContainer
    Input sample (ROW_MAJOR, n x d).
k : int
    Moment order.

Returns
-------
moment : DataContainer
    Central moment vector of size d."

// ---------------------------------------------------------------------

%feature("docstring") OT::StatisticsEngine::ComputeSkewness
"Compute skewness of each column.

Parameters
----------
sample : DataContainer
    Input sample (ROW_MAJOR, n x d).
adjusted : bool, optional
    Whether to apply the Fisher bias correction.
    Default is True. If False, return the raw moment ratio.

Returns
-------
skewness : DataContainer
    Skewness vector of size d."

// ---------------------------------------------------------------------

%feature("docstring") OT::StatisticsEngine::ComputeKurtosis
"Compute kurtosis of each column.

Parameters
----------
sample : DataContainer
    Input sample (ROW_MAJOR, n x d).
adjusted : bool, optional
    Whether to apply the Fisher bias correction (excess kurtosis).
    Default is True. If False, return the raw moment ratio.

Returns
-------
kurtosis : DataContainer
    Kurtosis vector of size d."

// ---------------------------------------------------------------------

%feature("docstring") OT::StatisticsEngine::ComputeMeanBlockwise
"Block-based mean for out-of-core processing.

Parameters
----------
sample : DataContainer
    Input sample (ROW_MAJOR, n x d).
blockSize : int, optional
    Number of rows per block. Default from ResourceMap key ``StatisticsEngine-DefaultBlockSize``.

Returns
-------
mean : DataContainer
    Mean vector of size d."

// ---------------------------------------------------------------------

%feature("docstring") OT::StatisticsEngine::ComputeVarianceBlockwise
"Block-based variance for out-of-core processing.

Parameters
----------
sample : DataContainer
    Input sample (ROW_MAJOR, n x d).
blockSize : int, optional
    Number of rows per block. Default from ResourceMap key ``StatisticsEngine-DefaultBlockSize``.

Returns
-------
variance : DataContainer
    Variance vector of size d."

// ---------------------------------------------------------------------

%feature("docstring") OT::StatisticsEngine::ComputeCovarianceBlockwise
"Block-based covariance for out-of-core processing.

Parameters
----------
sample : DataContainer
    Input sample (ROW_MAJOR, n x d).
blockSize : int, optional
    Number of rows per block. Default from ResourceMap key ``StatisticsEngine-DefaultBlockSize``.

Returns
-------
covariance : DataContainer
    Covariance matrix of size d x d."
