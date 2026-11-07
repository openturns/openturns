%feature("docstring") OT::TensorizedCovarianceModel
R"RAW(Multivariate covariance function defined as a tensorization of covariance models.

Parameters
----------
coll : sequence of :class:`~openturns.CovarianceModel`
    Collection of covariance models :math:`(C_k)_{1 \leq k \leq K}` of dimension :math:`d_k`.

Notes
-----
The tensorized covariance model defines a multivariate covariance model of dimension :math:`d\geq 1` 
from the tensorization of a given collection of covariance models. 
This allows one to create a higher dimension covariance model by combining models of smaller dimensions.
The dimension of each model in the collection does not necessarily have to be equal to 1. 
The input dimension of each covariance model in the collection must be the same. 

We consider the stochastic process :math:`X: \Omega \times\cD \mapsto \Rset^{\processDim}`, where :math:`\omega \in \Omega` is an event, :math:`\cD` is a domain of :math:`\Rset^{\domainDim}`.

Its covariance function :math:`C : \cD \times \cD \rightarrow  \cS_d^+(\Rset)` is defined from the collection of covariance functions :math:`C_k: \cD \times \cD \mapsto  \cS_{d_k}^+(\Rset)` where :math:`d_1+\dots+d_k=d`, as follows:

.. math::
    C(\vect{s}, \vect{t}) =    \left(
        \begin{array}{cccc}
        C_1(\vect{s}, \vect{t}) & 0 & \dots & 0 \\
        0 & C_2(\vect{s}, \vect{t}) & 0 &  \\
        \dots & \dots & \ddots & \dots \\
        0 & \dots & \dots &  C_K(\vect{s}, \vect{t})
        \end{array} \right)

The amplitude of the covariance function is :math:`\Tr{\sigma} =(\Tr{\sigma}_{1}, \dots, \Tr{\sigma}_{K})` and each model :math:`C_k` is parameterized by its scale :math:`\vect{\theta}_k \in \Rset^{\domainDim}`.

The effective scale of the tensorized model, as returned by :meth:`getScale`, is the scale :math:`\vect{\theta}_1` of the first marginal model :math:`C_1`.

The method :meth:`setScale` with argument :math:`\vect{\theta}` sets the scale of :math:`C_1` to :math:`\vect{\theta}` and rescales the other marginal models proportionally. Let :math:`\vect{\theta}_k=(\theta_{k,1},\hdots,\theta_{k,\domainDim})` be the scale of :math:`C_k` before the call. After the call, :math:`C_k` with :math:`k\geq 2` has the scale :math:`(\theta_1 \theta_{k,1}/\theta_{1,1},\hdots,\theta_{\domainDim} \theta_{k,\domainDim}/\theta_{1,\domainDim})`, so that the scale ratios between the marginal models are preserved.

Examples
--------

Create a tensorized covariance function from the tensorization of an absolute exponential function, a squared exponential function and an exponential function:

>>> import openturns as ot
>>> inputDimension = 2

Create the each covariance models:

>>> myCov1 = ot.AbsoluteExponential([3.0] * inputDimension)
>>> myCov2 = ot.SquaredExponential([2.0] * inputDimension)

Define the scale of the tensorized model:

>>> scale = [0.3, 0.8]

Create the tensorized model:

>>> covarianceModel = ot.TensorizedCovarianceModel([myCov1, myCov2], scale)

Set the scale of the first marginal model to one:

>>> covarianceModel.setScale([1.0] * inputDimension)
)RAW"

// ----------------------------------------------------------------------------

%feature("docstring") OT::TensorizedCovarianceModel::getCollection
R"RAW(Covariance model collection accessor.

Returns
-------
coll : sequence of :class:`~openturns.CovarianceModel`
    Collection of covariance models :math:`(C_k)_{1 \leq k \leq K}` of dimension :math:`d_k`.
)RAW"
