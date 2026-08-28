%feature("docstring") OT::StatisticsEngine
"Statistical operations on DataContainer objects.

This class is experimental.

StatisticsEngine provides static methods for statistical operations
on DataContainer objects, using TBBImplementation for parallelism.

Sample convention: row = observation, column = variable.
Input data must be in ROW_MAJOR layout."

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

%feature("docstring") OT::StatisticsEngine::ComputeQuantile
"Compute quantiles of each column.

Parameters
----------
sample : DataContainer
    Input sample (ROW_MAJOR, n x d).
q : float
    Quantile level in [0, 1].

Returns
-------
quantile : DataContainer
    Quantile vector of size d."

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
