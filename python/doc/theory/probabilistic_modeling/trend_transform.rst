.. _trend_transform:

Trend computation
-----------------

A multivariate stochastic process
:math:`X: \Omega \times\cD \rightarrow \Rset^d` of dimension :math:`d`
where :math:`\cD \in \Rset^n` may write as the sum of a trend function
:math:`f_{trend}: \Rset^n \rightarrow \Rset^d` and a centered
multivariate stochastic process
:math:`X_{c}: \Omega \times\cD \rightarrow \Rset^d` of dimension
:math:`d` as follows:

.. math::
  :label: tsDecomposition

    \forall \omega \in \Omega, \, \forall \vect{t} \in \cD, \,X(\omega,\vect{t}) = X_{c}(\omega,\vect{t}) + f_{trend}(\vect{t})

The objective here is to identify the trend function :math:`f_{trend}`
from a given field of the process :math:`X` and then to remove this
last one from the initial field. The resulting field is a realization
of the centered process :math:`X_{c}`.
The library also allows to define the
function :math:`f_{trend}` and to remove it from the initial field to
get the resulting centered field.

.. topic:: API:

    - See :class:`~openturns.TrendTransform`
    - See :class:`~openturns.InverseTrendTransform`
    - See :class:`~openturns.TrendFactory`

.. topic:: Examples:

    - See :doc:`/examples/probabilistic_modeling/add_trend`
    - See :doc:`/examples/probabilistic_modeling/trend_transform`
    - See :doc:`/examples/probabilistic_modeling/process_manipulation`
