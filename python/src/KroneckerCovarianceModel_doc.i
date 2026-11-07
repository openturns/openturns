%feature("docstring") OT::KroneckerCovarianceModel
R"RAW(Multivariate stationary Kronecker covariance function.

Available constructors:
    KroneckerCovarianceModel(*inputDimension*)

    KroneckerCovarianceModel(*rho, amplitude*)

    KroneckerCovarianceModel(*rho, amplitude, spatialCorrelation*)

    KroneckerCovarianceModel(*rho, sigma*)

Parameters
----------
inputDimension : int
    Input dimension :math:`\domainDim`.
    By default, the input dimension is deduced from the :math:`\vect{\theta}` parameter. If this one is not specified, then :math:`\domainDim=1`.
rho : :class:`~openturns.CovarianceModel`
    Covariance model
amplitude : sequence of positive floats
    Amplitude of the process :math:`\vect{\sigma}\in \Rset^{\processDim}`.
spatialCorrelation : :class:`~openturns.CorrelationMatrix`
    Correlation matrix :math:`\mat{R} \in \cS_{\processDim}^+([-1, 1])`
    By default, :math:`\mat{R}= \mat{I}_{\processDim}` where the dimension :math:`\processDim` is deduced from the amplitude :math:`\vect{\sigma}`.
sigma : :class:`~openturns.CovarianceMatrix`
    Covariance matrix :math:`C^{spatial} \in \cS_{\processDim}^+(\Rset)`.

Notes
-----
We consider the stochastic process :math:`X: \Omega \times\cD \rightarrow \Rset^{\processDim}`, where :math:`\Omega` is a sample space and :math:`\cD` is a subdomain of :math:`\Rset^{\domainDim}`.

The :class:`KroneckerCovarianceModel` class builds a covariance function with output dimension :math:`\processDim\geq1` from a correlation function :math:`\rho` with output dimension :math:`1` and a :math:`\processDim \times \processDim` covariance matrix. The latter is to be, for any :math:`\vect{s} \in \cD`, the covariance matrix of the random vector :math:`X(\cdot, \vect{s})`.

This covariance function is defined by

.. math::

    C(\vect{s}, \vect{t}) = \rho\left(\dfrac{\vect{s}}{\theta}, \dfrac{\vect{t}}{\theta}\right) C^{spatial},

where the spatial covariance matrix :math:`C^{spatial}(\vect{s}, \vect{t})` is given by

.. math::
    C^{spatial}= \mbox{Diag}(\vect{\sigma}) \, \mat{R} \,  \mbox{Diag}(\vect{\sigma}).


Examples
--------
Create a Kronecker covariance model with identity output correlation matrix.

>>> import openturns as ot
>>> scale = [4.0, 5.0]
>>> rho = ot.GeneralizedExponential(scale, 1.0)
>>> amplitude = [1.0, 2.0]
>>> myCovarianceModel = ot.KroneckerCovarianceModel(rho, amplitude)

Create a Kronecker covariance model and specify the output correlation matrix.

>>> scale = [4.0, 5.0]
>>> rho = ot.GeneralizedExponential(scale,  1.0)
>>> amplitude = [1.0, 2.0]
>>> spatialCorrelation = ot.CorrelationMatrix(2)
>>> spatialCorrelation[1,0] = 0.8
>>> myCovarianceModel = ot.KroneckerCovarianceModel(rho, amplitude, spatialCorrelation)

Create a Kronecker covariance model and specify the output covariance matrix.

>>> scale = [4.0, 5.0]
>>> rho = ot.GeneralizedExponential(scale, 1.0)
>>> sigma = ot.CovarianceMatrix(2)
>>> sigma[0,0] = 4.0
>>> sigma[1,1] = 5.0
>>> sigma[1,0] = 1.2
>>> myCovarianceModel = ot.KroneckerCovarianceModel(rho, sigma)
)RAW"
