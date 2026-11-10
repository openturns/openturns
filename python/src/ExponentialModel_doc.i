%feature("docstring") OT::ExponentialModel
R"RAW(Multivariate stationary exponential covariance function.

Available constructors:
    ExponentialModel(*inputDimension*)

    ExponentialModel(*scale, amplitude*)

    ExponentialModel(*scale, amplitude, spatialCorrelation*)

    ExponentialModel(*scale, spatialCovariance*)

Parameters
----------
inputDimension : int
    input dimension :math:`\domainDim`.
    By default, the input dimension is deduced from the :math:`\vect{\theta}` parameter. If this one is not specified, then :math:`\domainDim=1`.
scale : sequence of floats
     Scale coefficient :math:`\vect{\theta}\in \Rset^{\domainDim}`.
amplitude : sequence of positive floats
    Amplitude of the process :math:`\vect{\sigma}\in \Rset^{\processDim}`.
spatialCorrelation : :class:`~openturns.CorrelationMatrix`
    Correlation matrix :math:`\mat{R} \in \cS_{\processDim}^+([-1, 1])`
    By default, :math:`\mat{R}= \mat{I}_{\processDim}` where the dimension :math:`\processDim` is deduced from the amplitude :math:`\vect{\sigma}`.
spatialCovariance : :class:`~openturns.CovarianceMatrix`
    Covariance matrix :math:`C^{spatial} \in \cS_{\processDim}^+(\Rset)`.

Notes
-----
We consider the scalar stochastic process :math:`X: \Omega \times\cD \rightarrow \Rset^{\processDim}`, where :math:`\omega \in \Omega` is an event, :math:`\cD` is a domain of :math:`\Rset^{\domainDim}`.

The *exponential* function is a stationary covariance function with dimension :math:`\processDim\geq1`. It is defined by:

.. math::

    C(\vect{s}, \vect{t}) = \rho\left(\dfrac{\vect{s}}{\theta}, \dfrac{\vect{t}}{\theta}\right)\, C^{spatial},
    \quad \forall (\vect{s}, \vect{t}) \in \cD

where the correlation function :math:`\rho` is given by:

.. math::
    \rho(\vect{s}, \vect{t} ) = e^{-\left\| \vect{s}- \vect{t} \right\|_1} \quad \forall (\vect{s}, \vect{t}) \in \cD

and the spatial covariance matrix :math:`C^{spatial}(\vect{s}, \vect{t})` by:

.. math::
    C^{spatial} = \mbox{Diag}(\vect{\sigma}) \, \mat{R} \,  \mbox{Diag}(\vect{\sigma}).


Examples
--------
Create an exponential model from the amplitude :math:`\vect{\sigma}` and the scale :math:`\vect{\theta}`:

>>> import openturns as ot
>>> amplitude = [1.0, 2.0]
>>> scale = [4.0, 5.0]
>>> myCovarianceModel = ot.ExponentialModel(scale, amplitude)

Create an exponential model from the amplitude, scale and the correlation matrix:

>>> amplitude = [1.0, 2.0]
>>> spatialCorrelation = ot.CorrelationMatrix(2)
>>> spatialCorrelation[0,1] = 0.8
>>> myCovarianceModel = ot.ExponentialModel(scale, amplitude, spatialCorrelation)

Create an exponential model from the scale and covariance matrix:

>>> amplitude = [1.0, 2.0]
>>> spatialCovariance = ot.CovarianceMatrix(2)
>>> spatialCovariance[0,0] = 4.0
>>> spatialCovariance[1,1] = 5.0
>>> spatialCovariance[0,1] = 1.2
>>> myCovarianceModel = ot.ExponentialModel(scale, spatialCovariance)
)RAW"
