%feature("docstring") OT::CirculantEmbeddingGaussianProcess
R"RAW(Circulant embedding Gaussian process.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

Available constructor:
    CirculantEmbeddingGaussianProcess(*covarianceModel, interval, discretization*)

Parameters
----------
covarianceModel : :class:`~openturns.CovarianceModel`
    Stationary covariance model :math:`C`.
    Supports scalar or vector-valued output (output dimension >= 1).
interval : :class:`~openturns.Interval`
    Spatial domain :math:`\cD` of dimension d.
discretization : sequence of int
    Number of cells per dimension. Each entry must be at least 1.

Notes
-----
CirculantEmbeddingGaussianProcess implements the fast and exact simulation
of stationary Gaussian processes using the circulant embedding method,
also known as the Davies-Harte algorithm [davies1987]_, [wood1994]_. The extension
to d-dimensional domains follows [coeurjolly2016]_.
[graham2018a]_ proves that for a sufficiently large embedding domain,
the circulant matrix is guaranteed to be positive definite.

An :class:`~openturns.IntervalMesher` is used internally to build a regular
mesh from the given interval and discretization.

The method embeds the Toeplitz covariance matrix into a circulant matrix
of size N per dimension, where each N is the smallest power of two greater
than or equal to :math:`2(n_d-1)` and :math:`n_d` is the number of
vertices along dimension d. The block-circulant matrix is diagonalized via FFT
in :math:`\cO(N_1\cdots N_d \log(N_1\cdots N_d))` operations.

For scalar output (output dimension = 1), eigenvalues are computed directly
from the FFT of the first row, avoiding Cholesky factorization. For vector
output (output dimension > 1), the spectral density matrix is factorized
via Cholesky at each frequency.

If the circulant matrix has negative eigenvalues, the embedding domain
is automatically increased until all eigenvalues are non-negative,
following the iterative strategy of [pichot2022]_.

Examples
--------
Create a 1D CirculantEmbeddingGaussianProcess with scalar output:

>>> import openturns as ot
>>> import openturns.experimental as otexp
>>> ot.RandomGenerator.SetSeed(0)
>>> amplitude = [1.0]
>>> scale = [1.0]
>>> covarianceModel = ot.ExponentialModel(scale, amplitude)
>>> interval = ot.Interval(0.0, 10.0)
>>> process = otexp.CirculantEmbeddingGaussianProcess(covarianceModel, interval, [100])
>>> realization = process.getRealization()
>>> print(realization.getValues()[0:5])
    [ y0       ]
0 : [ 0.520606 ]
1 : [ 0.93736  ]
2 : [ 1.09874  ]
3 : [ 0.507693 ]
4 : [ 0.239623 ]

Create a 2D process with scalar output:

>>> import openturns as ot
>>> import openturns.experimental as otexp
>>> ot.RandomGenerator.SetSeed(0)
>>> cov2D = ot.ExponentialModel([1.0, 1.0], [1.0])
>>> process2D = otexp.CirculantEmbeddingGaussianProcess(cov2D, ot.Interval([0.0]*2, [5.0]*2), [10, 8])
>>> realization2D = process2D.getRealization()
>>> print(realization2D.getOutputDimension())
1

Create a 2D process with vector-valued output (2 components):

>>> import openturns as ot
>>> import openturns.experimental as otexp
>>> ot.RandomGenerator.SetSeed(0)
>>> covVec = ot.ExponentialModel([1.0, 1.0], [1.0, 0.5])
>>> processVec = otexp.CirculantEmbeddingGaussianProcess(covVec, ot.Interval([0.0]*2, [3.0]*2), [6, 6])
>>> print(processVec.getOutputDimension())
2
>>> realizationVec = processVec.getRealization()
>>> print(realizationVec.getOutputDimension())
2

)RAW"
// ---------------------------------------------------------------------

%feature("docstring") OT::CirculantEmbeddingGaussianProcess::getCovarianceModel
"Get the covariance model.

Returns
-------
covarianceModel : :class:`~openturns.CovarianceModel`
    Temporal covariance model."
// ---------------------------------------------------------------------

%feature("docstring") OT::CirculantEmbeddingGaussianProcess::getCircularSize
"Get the size of the circulant matrix per dimension.

Returns
-------
circularSize : :class:`~openturns.Indices`
    Size N_d of the circulant matrix used for the embedding in each
    spatial dimension."
// ---------------------------------------------------------------------

%feature("docstring") OT::CirculantEmbeddingGaussianProcess::setCircularSize
"Set the size of the circulant matrix per dimension.

The circulant size must be a power of two. By default it is chosen as the
smallest power of two greater than or equal to 2(n_d-1). Setting a larger
value may help when the covariance function leads to negative eigenvalues
with the minimal embedding.

Parameters
----------
circularSize : sequence of int
    Size N_d of the circulant matrix in each spatial dimension
    (must be a power of two)."
// ---------------------------------------------------------------------

%feature("docstring") OT::CirculantEmbeddingGaussianProcess::getFFTAlgorithm
"Get the FFT algorithm used to generate realizations.

Returns
-------
fftAlgo : :class:`~openturns.FFT`
    FFT algorithm used to generate realizations.
    By default, it is the :class:`~openturns.KissFFT` algorithm."
// ---------------------------------------------------------------------

%feature("docstring") OT::CirculantEmbeddingGaussianProcess::setFFTAlgorithm
"Set the FFT algorithm used to generate realizations.

Parameters
----------
fftAlgo : :class:`~openturns.FFT`
    FFT algorithm that will be used to generate realizations."
// ---------------------------------------------------------------------

%feature("docstring") OT::CirculantEmbeddingGaussianProcess::getInterval
R"RAW(Get the spatial domain interval.

Returns
-------
interval : :class:`~openturns.Interval`
    Spatial domain :math:`\cD` on which the process is defined.
)RAW"
// ---------------------------------------------------------------------

%feature("docstring") OT::CirculantEmbeddingGaussianProcess::getDiscretization
"Get the number of cells per dimension.

Returns
-------
discretization : :class:`~openturns.Indices`
    Number of cells per spatial dimension."
